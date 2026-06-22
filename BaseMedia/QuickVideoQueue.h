#pragma once

#include "MediaMetadef.h"
#include "readerwriterqueue.h"
#include <QObject>

class BASEMEDIA_EXPORT AVFrameData {
    Q_DISABLE_COPY(AVFrameData)
  public:
    bool cpu_decoding = true;
    int pixel_format = -1;
    AVFrame *frame = nullptr;

  public:
    bool cpuFrame() { return cpu_decoding; }
    int format() { return pixel_format; }
    void setPixelFormat(int val) { pixel_format = val; }
    int imageWidth() { return frame ? frame->width : 0; }
    int imageHeight() { return frame ? frame->height : 0; }
    int linesizeV() { return frame ? frame->linesize[2] : 0; }
    int linesizeU() { return frame ? frame->linesize[1] : 0; }
    int linesizeY() { return frame ? frame->linesize[0] : 0; }
    int linesizeUV() { return frame ? frame->linesize[1] : 0; }
    uint8_t *ydata() { return frame ? frame->data[0] : nullptr; }
    uint8_t *udata() { return frame ? frame->data[1] : nullptr; }
    uint8_t *vdata() { return frame ? frame->data[2] : nullptr; }
    uint8_t *uvdata() { return frame ? frame->data[1] : nullptr; }

    AVFrameData(bool cpu);
    ~AVFrameData();

    void dataTransfor();
    void recordFrameData();
};

typedef std::shared_ptr<AVFrameData> AVFramePtr;

class BASEMEDIA_EXPORT QuickVideoQueue : public QObject {
    Q_OBJECT
    Q_DISABLE_COPY(QuickVideoQueue)

  public:
    AVFramePtr getRenderFrame();

    bool setRenderFrame(AVFramePtr mat);

    AVFramePtr getCacheFrame(bool cpu);

    bool setCacheFrame(AVFramePtr mat);

    QuickVideoQueue();

  private:
    moodycamel::ReaderWriterQueue<AVFramePtr> _gpu_render_frames;
    moodycamel::ReaderWriterQueue<AVFramePtr> _gpu_cache_frames;
};
