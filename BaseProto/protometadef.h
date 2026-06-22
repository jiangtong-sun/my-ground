#pragma once

#include <qglobal.h>

#if defined(BASEPROTO_SHAREDLIB)
#if defined(BASEPROTO_EXPORTS)
#define BASEPROTO_EXPORT Q_DECL_EXPORT
#else
#define BASEPROTO_EXPORT Q_DECL_IMPORT
#endif
#else
#define BASEPROTO_EXPORT
#endif

#define PROTO_PROPERTY_RW(TYPE, M)                                                                 \
    Q_PROPERTY(TYPE M MEMBER _##M NOTIFY M##Changed)                                               \
  public:                                                                                          \
    Q_SIGNAL void M##Changed();                                                                    \
    void M(const TYPE &in_##M) {                                                                   \
        _##M = in_##M;                                                                             \
        Q_EMIT M##Changed();                                                                       \
    }                                                                                              \
    TYPE M() { return _##M; }                                                                      \
                                                                                                   \
  protected:                                                                                       \
    TYPE _##M;
