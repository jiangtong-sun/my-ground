#pragma once

#include "protometadef.h"
#include <stdint.h>
//#include <string>
#include <QString>

BASEPROTO_EXPORT int64_t currentTimeMs();
BASEPROTO_EXPORT QString WCharQStr(const std::wstring &value);

#define LinkStr(x) L##x
#define transToUnicode(ax) WCharQStr(std::wstring(LinkStr(ax)))
#define wstrUnicode(ax) std::wstring(LinkStr(ax))
