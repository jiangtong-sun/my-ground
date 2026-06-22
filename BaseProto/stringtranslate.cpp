#include "stringtranslate.h"
#include <QString>
#include <chrono>

QString WCharQStr(const std::wstring &value) {
    if (value.empty()) {
        return QString();
    }

    return QString::fromStdWString(value);
}

int64_t currentTimeMs() {
    typedef std::chrono::milliseconds ms_precision;
    typedef std::chrono::time_point<std::chrono::system_clock, ms_precision> time_precision;
    time_precision tp =
        std::chrono::time_point_cast<ms_precision>(std::chrono::system_clock::now());
    return tp.time_since_epoch().count();
}
