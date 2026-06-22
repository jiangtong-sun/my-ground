#include "QuickVideoQueue.h"
#include "log_category.h"

#define MAX_CACHE_FRAME_COUNT 5
void AVFrameData::dataTransfor() {
    if (pixel_format == AV_PIX_FMT_YUV420P || pixel_format == AV_PIX_FMT_YUV420P) {
        return;
    }
}

AVFrameData::~AVFrameData() {
    if (frame) {
        av_frame_free(&frame);
    }
}

AVFrameData::AVFrameData(bool cpu) : frame(nullptr), cpu_decoding(cpu) { frame = av_frame_alloc(); }

void AVFrameData::recordFrameData() {

    int bufsize0 = frame->height * frame->linesize[0];
    int bufsize1 = frame->height * frame->linesize[1] / 2;
    int bufsize2 = frame->height * frame->linesize[2] / 2;
    FILE *outputfile = nullptr;
    fopen_s(&outputfile, "tests.yuv", "wb+");
    fwrite(frame->data[0], 1, bufsize0, outputfile);
    fwrite(frame->data[1], 1, bufsize1, outputfile);
    fwrite(frame->data[2], 1, bufsize2, outputfile);
    fclose(outputfile);

    // AVFrame *host_frame = av_frame_alloc();
    // SwsContext *sws_ctx = nullptr;
    // sws_ctx =
    //     sws_getContext(frame->width, frame->height, AVPixelFormat(frame->format), frame->width,
    //                    frame->height, AV_PIX_FMT_BGR24, SWS_FAST_BILINEAR, NULL, NULL, NULL);
    // cv::Mat cpuMat;
    // cpuMat.create(frame->height, frame->width, CV_8UC3);
    // unsigned char *buffer = (unsigned char *) cpuMat.data;
    // av_image_fill_arrays(host_frame->data, host_frame->linesize, buffer, AV_PIX_FMT_BGR24,
    //                      frame->width, frame->height, 1);
    // sws_scale(sws_ctx, frame->data, frame->linesize, 0, frame->height, host_frame->data,
    //           host_frame->linesize);
    // cv::imwrite("cpumat.jpg", cpuMat);
    // av_frame_free(&host_frame);
    // sws_freeContext(sws_ctx);
}

bool QuickVideoQueue::setCacheFrame(AVFramePtr mat) {
    if (mat == nullptr) {
        int rtn = _gpu_cache_frames.enqueue(mat);
        if (MAX_CACHE_FRAME_COUNT < _gpu_cache_frames.size_approx()) {
            AVFramePtr temp = nullptr;
            _gpu_cache_frames.try_dequeue(temp);
        }

        return rtn;
    }

    COMMON_WARN("invalid param");
    return false;
}

AVFramePtr QuickVideoQueue::getCacheFrame(bool cpu) {
    if (_gpu_cache_frames.size_approx() <= 0) {
        return std::make_shared<AVFrameData>(cpu);
    }

    AVFramePtr temp = nullptr;
    _gpu_cache_frames.try_dequeue(temp);
    if (temp != nullptr) {
        return temp;
    }

    return std::make_shared<AVFrameData>(cpu);
}

bool QuickVideoQueue::setRenderFrame(AVFramePtr mat) {
    assert(mat != nullptr);
    if (mat != nullptr) {
        int rtn = _gpu_render_frames.enqueue(mat);
        /// 避免待渲染的图像缓存超过MAX_CACHE_FRAME_COUNT个数
        if (MAX_CACHE_FRAME_COUNT < _gpu_render_frames.size_approx()) {
            AVFramePtr temp = nullptr;
            _gpu_render_frames.try_dequeue(temp);
        }
        return rtn;
    }

    return false;
}

AVFramePtr QuickVideoQueue::getRenderFrame() {
    if (_gpu_render_frames.size_approx() <= 0) {
        return nullptr;
    }

    AVFramePtr temp = nullptr;
    _gpu_render_frames.try_dequeue(temp);
    if (temp != nullptr) {
        return temp;
    }

    return nullptr;
}

QuickVideoQueue::QuickVideoQueue() {}
