#pragma once

#include "stringtranslate.h"

#include <qglobal.h>

#if defined(VEHICLE_SHAREDLIB)
#if defined(VEHICLE_EXPORTS)
#define VEHICLE_EXPORT Q_DECL_EXPORT
#else
#define VEHICLE_EXPORT Q_DECL_IMPORT
#endif
#else
#define VEHICLE_EXPORT
#endif

#define DisplayIndex transToUnicode("块索引")
#define DisplaySubModule transToUnicode("子模块")
#define DisplayModuleAbbr transToUnicode("缩写")
#define DisplayRefFrame transToUnicode("帧名")
#define DisplayRefFType transToUnicode("帧类型")
#define DisplayRefData transToUnicode("数据项ID")
#define DisplayDataName transToUnicode("数据项名")
#define DisplayColorSet transToUnicode("显示方式")
#define DisplayDataIndex transToUnicode("数据索引")

#define VEH_PROPERTY_RW(TYPE, M)                                                                   \
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

#define VEH_PROPERTY_SETGET(TYPE, M)                                                               \
    Q_PROPERTY(TYPE M MEMBER _##M CONSTANT)                                                        \
  public:                                                                                          \
    void M(const TYPE &in_##M) {                                                                   \
        _##M = in_##M;                                                                             \
    }                                                                                              \
    TYPE M() { return _##M; }                                                                      \
                                                                                                   \
  protected:                                                                                       \
    TYPE _##M;
