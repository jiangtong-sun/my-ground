#include "DecoderController.h"
#include "log_category.h"
#include <QDebug>
#include <QUrl>
#include <cuda.h>
#include <cuda_runtime_api.h>

inline static int InterruptCb(void *context) {
    DecoderController *reader = (DecoderController *) context;
    if (reader->isTimeout()) {
        return 1;
    }
    return 0;
}

static AVPixelFormat hardwarePixelFormat(AVCodecContext *ctx, const enum AVPixelFormat *fmt) {
    const enum AVPixelFormat *p;
    DecoderController *decoder = (DecoderController *) ctx->opaque;
    for (p = fmt; (*p != -1) && decoder; p++) {
        if (*p == decoder->hwPixelFormat()) {
            return *p;
        }
    }

    return AV_PIX_FMT_NONE;
}

static int hardwareDecoderInit(AVCodecContext *ctx, const enum AVHWDeviceType type) {
    int err = 0;
    DecoderController *decoder = (DecoderController *) ctx->opaque;
    if ((err = av_hwdevice_ctx_create(&decoder->device_ctx, type, nullptr, nullptr, 0)) < 0) {
        return err;
    }
    ctx->hw_device_ctx = av_buffer_ref(decoder->device_ctx);
    ctx->get_format = hardwarePixelFormat;
    return err;
}

void DecoderController::setTimeout(int time) { _timeout = time; }

DecoderController::DecoderController() : media_state(nullptr), _timeout(4000) {}

DecoderController::~DecoderController() {
    COMMON_INFO("exit media decoder");
    closeMedia();
}

void DecoderController::setMediaState(MediaStateCallback state) { media_state = state; }

bool DecoderController::isWorking() {
    if (_reading) {
        return true;
    }

    return false;
}

bool DecoderController::isTimeout() {
    uint64_t code = _lastTime + _timeout;
    return milliseconds_since_epoch() >= code;
}

int DecoderController::frameRate() {
    assert(in_stream != nullptr);
    if (in_stream == nullptr) {
        return -1;
    }

    AVRational rate = in_stream->r_frame_rate;
    AVRational avg = in_stream->avg_frame_rate;

    if (avg.num > 0 && avg.den > 0) {
        return (int) ceil((double) (avg.num) / avg.den);
    } else if (rate.num > 0 && rate.den > 0) {
        return (int) ceil((double) (rate.num) / rate.den);
    }

    return -1;
}

void DecoderController::freeResource() {

    if (_input_fmtctx) {
        avformat_close_input(&_input_fmtctx);
        _input_fmtctx = nullptr;
    }

    if (decoder_ctx) {
        avcodec_free_context(&decoder_ctx);
        avcodec_close(decoder_ctx);
        decoder_ctx = nullptr;
    }

    if (device_ctx) {
        av_buffer_unref(&device_ctx);
        device_ctx = nullptr;
    }

    if (img_convert_ctx) {
        sws_freeContext(img_convert_ctx);
        img_convert_ctx = nullptr;
    }
}

void DecoderController::readMediaData() {
    AVPacket *pkt = av_packet_alloc();
    while (_reading) {
        _lastTime = milliseconds_since_epoch();
        if (av_read_frame(_input_fmtctx, pkt) < 0) {
            COMMON_INFO("av_read_frame finish {}", _packageCount);
            break;
        }

        if ((pkt->stream_index != videoindex) || pkt->size < 1) {
            COMMON_INFO("unknow video data {}", _packageCount);
            av_packet_unref(pkt);
            continue;
        }

        if (AV_NOPTS_VALUE == pkt->pts) {
            double calc_duration = (double) AV_TIME_BASE / av_q2d(in_stream->avg_frame_rate);
            double interval = (double) (av_q2d(in_stream->time_base) * AV_TIME_BASE);
            pkt->pts = (int64_t)((_packageCount * calc_duration) / interval);
            pkt->duration = (int64_t)(calc_duration / interval);
            pkt->dts = pkt->pts;
            pkt->pos = -1;
        }

        _packageCount++;
        int err = avcodec_send_packet(decoder_ctx, pkt);
        if (err != AVERROR(EAGAIN) && err != AVERROR_EOF && err < 0) {
            COMMON_INFO("avcodec send package function failed {}", err);
            av_packet_unref(pkt);
            continue;
        }

        AVFramePtr frameData = videomat_queue.getCacheFrame(cpu_decoder);
        err = avcodec_receive_frame(decoder_ctx, frameData->frame);
        if (err == AVERROR(EAGAIN) || (err == AVERROR_EOF)) {
            COMMON_INFO("avcodec recv package failed {}", err);
            av_packet_unref(pkt);
            continue;
        }

        if (err < 0) {
            COMMON_INFO("the decoder has been fully flushed");
            av_packet_unref(pkt);
            break;
        } else {
            if (frameData->frame->format == 0) {
                frameData->setPixelFormat(AV_PIX_FMT_YUV420P);
            } else {
                frameData->setPixelFormat(frameData->frame->format);
            }

            COMMON_TRACE("package count {}, size {}, format {}", _packageCount, pkt->size, frameData->frame->format);
            videomat_queue.setRenderFrame(frameData);
            av_packet_unref(pkt);
        }

        // if (is_local_file && (frameRate() > 0)) {
        //     uint64_t curr = milliseconds_since_epoch();
        //     uint32_t timedelay = 1000 / frameRate();
        //     if (curr - _lastTime + 5 < timedelay) {
        //         timedelay -= (curr - _lastTime);
        //         std::this_thread::sleep_for(std::chrono::milliseconds(timedelay));
        //     }
        // }
    }

    av_packet_free(&pkt);

    std::thread::id val = std::this_thread::get_id();
    _Thrd_t t = *(_Thrd_t *) (char *) &val;

    COMMON_INFO("exit media read thread {}", t._Id);

    if (media_state != nullptr) {
        media_state(false);
    }

    freeResource();
}

bool DecoderController::isCpuDecoder() { return cpu_decoder; }

void DecoderController::openMedia(const QString &url, bool cpumode) {
    if (isWorking()) {
        COMMON_INFO("media is working");
        return;
    }

    QUrl lfile = QUrl::fromLocalFile(url);
    is_local_file = lfile.isLocalFile();

    // AVHWDeviceType type = AV_HWDEVICE_TYPE_NONE;
    // while ((type = av_hwdevice_iterate_types(type)) != AV_HWDEVICE_TYPE_NONE) {
    //     const char *ctype = av_hwdevice_get_type_name(type);
    //     if (ctype) {
    //         hw_deviceTypes.append(QString::fromStdString(ctype));
    //         COMMON_INFO("the hardware decoder {}", ctype);
    //         if (QString::fromStdString(ctype).contains("cuda")) {
    //             int count = -1;
    //             if (cudaGetDeviceCount(&count) == cudaSuccess && count > 0) {
    //                 _deviceName = QString::fromStdString(ctype);
    //             }
    //         }
    //     }
    // }

    _mediaUrl = url;
    _reading.store(true);
    cpu_decoder = cpumode;
    COMMON_INFO("open url {}", url.toStdString());
    _readThread = std::thread([&]() {
        while (_reading) {
            std::this_thread::sleep_for(std::chrono::milliseconds(_reopen_time));
            if (_reading && readStream()) {
                readMediaData();
            } else {
                freeResource();
            }
        }
    });
}

void DecoderController::closeMedia() {
    bool state = _reading.load();
    _reading.store(false); //设置退出符号
    if (state && _readThread.joinable()) {
        _readThread.join();
    }
}

bool DecoderController::isRtspStream() {
    if (_mediaUrl.indexOf("rtsp://") >= 0) {
        return true;
    }

    return false;
}

const AVCodec *DecoderController::hardwareCodec(AVHWDeviceType devType, AVCodecID codecID) {
    const char *hw_name = nullptr;
    switch (devType) {
    case AV_HWDEVICE_TYPE_CUDA:
        hw_name = "cuvid";
        break;
    case AV_HWDEVICE_TYPE_QSV:
        hw_name = "qsv";
        break;
    case AV_HWDEVICE_TYPE_VAAPI:
        hw_name = "vaapi";
        break;
    case AV_HWDEVICE_TYPE_VDPAU:
        hw_name = "vdpau";
        break;
    case AV_HWDEVICE_TYPE_DXVA2:
    case AV_HWDEVICE_TYPE_D3D11VA:
        hw_name = "-";
        break;
    default:
        break;
    }

    const AVCodec *codec = nullptr;
    if (hw_name) {
        if (hw_name[0] == '-') {
            codec = avcodec_find_decoder(codecID);
        } else {
            char hw_codec_name[128] = {0};
            sprintf(hw_codec_name, "%s_%s", avcodec_get_name(codecID), hw_name);
            codec = avcodec_find_decoder_by_name(hw_codec_name);
        }
    }

    return codec;
}

bool DecoderController::readStream() {
    char error[256] = {0};
    int res = 0;

    if (_mediaUrl.isEmpty()) {
        return false;
    }

    // hw_device_type = av_hwdevice_find_type_by_name(_deviceName.toStdString().c_str());
    // if (hw_device_type == AV_HWDEVICE_TYPE_NONE) {
    //     COMMON_ERROR("note found hw device {}", _deviceName.toStdString().c_str());
    //     cpu_decoder = true;
    // }

    _lastTime = milliseconds_since_epoch();
    _input_fmtctx = avformat_alloc_context();
    if (_input_fmtctx == nullptr) {
        COMMON_ERROR("avformat_alloc_context failed");
        return false;
    }

    // 非阻塞读取
    //_input_fmtctx->flags |= AVFMT_FLAG_NONBLOCK;
    _input_fmtctx->interrupt_callback = AVIOInterruptCB{InterruptCb, this};
    _input_fmtctx->video_codec_id = AV_CODEC_ID_NONE;
    _input_fmtctx->audio_codec_id = AV_CODEC_ID_NONE;

    AVDictionary *options = nullptr;
    av_dict_set(&options, "preset", "medium", 0);
    av_dict_set(&options, "tune", "zerolatency", 0);
    av_dict_set(&options, "max_req_times", "2", 0);

    if (isRtspStream()) { //以udp方式打开rtsp流
        res = av_dict_set(&options, "rtsp_transport", "udp", 0);
        if (res < 0) {
            av_strerror(res, error, sizeof(error));
            COMMON_ERROR("av_dict_set failed, {}", error);
            return false;
        }
    }

    res = avformat_open_input(&_input_fmtctx, _mediaUrl.toStdString().c_str(), nullptr, &options);
    if (options) {
        av_dict_free(&options);
    }
    if (res < 0) {
        _reopen_time = rand() % 500 + 1000;
        av_strerror(res, error, sizeof(error));
        COMMON_ERROR("avformat_open_input failed, {}", error);
        return false;
    } else {
        _reopen_time = 500;
        COMMON_INFO("avformat_open_input {}", _mediaUrl.toStdString().c_str());
    }

    res = avformat_find_stream_info(_input_fmtctx, NULL);
    if (res < 0) {
        av_strerror(res, error, sizeof(error));
        COMMON_ERROR("avformat_find_stream_info failed, {}", error);
        avformat_close_input(&_input_fmtctx);
        return false;
    }

    int videoindex = av_find_best_stream(_input_fmtctx, AVMEDIA_TYPE_VIDEO, -1, -1, &in_decoder, 0);
    if (videoindex >= 0) {
        in_stream = _input_fmtctx->streams[videoindex];
        in_codecpar = in_stream->codecpar;
        in_codec_id = in_codecpar->codec_id;
        videoindex = videoindex;

        emit videoInfoReady(in_codecpar->width, in_codecpar->height);

        COMMON_INFO("av_find_best_stream {}", videoindex);

        if (in_decoder != nullptr) {
            for (int i = 0; !cpu_decoder; i++) {
                const AVCodecHWConfig *config = avcodec_get_hw_config(in_decoder, i);
                if (config == nullptr) {
                    COMMON_ERROR("Decoder {} does not support device type {} ", in_decoder->name,
                                 _deviceName.toStdString().c_str());
                    in_decoder = avcodec_find_decoder(in_codec_id);
                    cpu_decoder = true;
                    break;
                }
                if (config->methods & AV_CODEC_HW_CONFIG_METHOD_HW_DEVICE_CTX &&
                    config->device_type == hw_device_type) {
                    hw_pix_fmt = config->pix_fmt;
                    break;
                }
            }
        } else {
            if (cpu_decoder) {
                in_decoder = avcodec_find_decoder(in_codec_id);
            } else {
                in_decoder = hardwareCodec(hw_device_type, in_codec_id);
            }
        }

        if (in_decoder == nullptr) {
            COMMON_ERROR("not found media decoder");
            freeResource();
            return false;
        }

        // if (!cpu_decoder && (img_convert_ctx != nullptr) && (in_codecpar->format !=
        // AV_PIX_FMT_NV12)) {
        //     int width = in_codecpar->width;
        //     int height = in_codecpar->height;
        //     img_convert_ctx = sws_getContext(width, height, (enum
        //     AVPixelFormat)in_codecpar->format,
        //                                      width, height, (enum AVPixelFormat)AV_PIX_FMT_NV12,
        //                                      SWS_POINT, NULL, NULL, NULL);
        // }

        decoder_ctx = avcodec_alloc_context3(in_decoder);
        if (decoder_ctx == nullptr) {
            COMMON_ERROR("avcodec_alloc_context3 failed");
            freeResource();
            return false;
        }

        if (avcodec_parameters_to_context(decoder_ctx, in_codecpar) < 0) {
            COMMON_ERROR("avcodec_parameters_to_context failed");
            freeResource();
            return false;
        }

        //// 允许不符合规范的加速技巧
        decoder_ctx->flags2 |= AV_CODEC_FLAG2_FAST;
        if (!cpu_decoder) {
            decoder_ctx->opaque = this;
            if (hardwareDecoderInit(decoder_ctx, hw_device_type) < 0) {
                COMMON_INFO("hardwareDecoderInit failed");
                freeResource();
                return false;
            }
        } else {
            decoder_ctx->thread_type = FF_THREAD_FRAME;
        }

        int err = avcodec_open2(decoder_ctx, in_decoder, NULL);
        if (err < 0) {
            av_strerror(err, error, sizeof(error));
            COMMON_ERROR("av_read_frame failed, {}", error);
            freeResource();
            return false;
        }

        if (media_state != nullptr) {
            media_state(true);
        }

        COMMON_INFO("avcodec_open2 result {}", err);
        return true;
    }

    return false;
}
