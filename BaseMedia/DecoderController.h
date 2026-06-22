#pragma once

#include "QuickVideoQueue.h"
#include <atomic>
#include <thread>

using MediaStateCallback = std::function<void(bool)>;
class BASEMEDIA_EXPORT DecoderController : public QObject {

    Q_OBJECT
    Q_DISABLE_COPY(DecoderController)
    MEDIA_PROPERTY_AUTO(QString, deviceName)
    Q_PROPERTY(QStringList deviceList READ deviceList CONSTANT)
  public:
    DecoderController();
    ~DecoderController();

    void setMediaState(MediaStateCallback);

    QStringList deviceList() { return hw_deviceTypes; }

    AVBufferRef *device_ctx = nullptr;
    AVPixelFormat hwPixelFormat() { return hw_pix_fmt; }
    QuickVideoQueue &videoQueue() { return videomat_queue; }
    const AVCodec *hardwareCodec(AVHWDeviceType devType, AVCodecID codecID);
    void openMedia(const QString &url, bool cpumode);
    void setTimeout(int time);
    bool isCpuDecoder();
    void closeMedia();
    bool isTimeout();
    bool isWorking();
    int frameRate();

  private:
    bool readStream();
    void readMediaData();
    void freeResource();
    bool isRtspStream();

  signals:
    void videoInfoReady(int width, int height);

  protected:
    AVFormatContext *_input_fmtctx = nullptr;
    AVCodecContext *decoder_ctx = nullptr;
    AVHWDeviceType hw_device_type;
    AVPixelFormat hw_pix_fmt;
    SwsContext *img_convert_ctx = nullptr;
    MediaStateCallback media_state = nullptr;
    const AVCodec *in_decoder = nullptr;
    AVStream *in_stream = nullptr;
    AVCodecID in_codec_id = AV_CODEC_ID_NONE;
    AVCodecParameters *in_codecpar = nullptr;
    int videoindex = 0;

    QString _mediaUrl = "";

    uint32_t _packageCount = 0;
    uint64_t _lastTime = 0;
    uint32_t _timeout = 2000;
    uint32_t _reopen_time = 500;

    QuickVideoQueue videomat_queue;

    bool cpu_decoder = false;
    bool is_local_file = false;

    QStringList hw_deviceTypes;
    std::atomic_bool _reading{false};
    std::thread _readThread;
};
