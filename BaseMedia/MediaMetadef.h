#pragma once

#include <qglobal.h>
#include <stdint.h>

#if defined(BASEMEDIA_SHAREDLIB)
#if defined(BASEMEDIA_EXPORTS)
#define BASEMEDIA_EXPORT Q_DECL_EXPORT
#else
#define BASEMEDIA_EXPORT Q_DECL_IMPORT
#endif
#else
#define BASEMEDIA_EXPORT
#endif

#ifdef __cplusplus
extern "C" {
#endif
#include <libavcodec/avcodec.h>
#include <libavdevice/avdevice.h>
#include <libavformat/avformat.h>
#include <libavutil/hwcontext.h>
#include <libavutil/imgutils.h>
#include <libavutil/mem.h>
#include <libswresample/swresample.h>
#include <libswscale/swscale.h>
#ifdef __cplusplus
}
#endif

BASEMEDIA_EXPORT uint64_t milliseconds_since_epoch();

#define MEDIA_PROPERTY_AUTO(TYPE, M)                                                               \
    Q_PROPERTY(TYPE M MEMBER _##M NOTIFY M##Changed)                                               \
  public:                                                                                          \
    Q_SIGNAL void M##Changed();                                                                    \
    void M(const TYPE &in_##M) {                                                                   \
        _##M = in_##M;                                                                             \
        Q_EMIT M##Changed();                                                                       \
    }                                                                                              \
    TYPE M() { return _##M; }                                                                      \
                                                                                                   \
  private:                                                                                         \
    TYPE _##M;
