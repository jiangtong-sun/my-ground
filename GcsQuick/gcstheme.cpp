#include "gcstheme.h"

#include <QGlobalStatic>
#include <QGuiApplication>
#include <QPalette>

#include "constants.h"

Q_GLOBAL_STATIC(GcsTheme, gTheme)
GcsTheme *GcsTheme::instance() { return gTheme(); }

GcsTheme::GcsTheme(QObject *parent) : QObject{parent} {
    _iconfront = "iconfont.ttf";
    _darkMode = ThemeType::DarkMode::Dark;
    _systemDark = false;

    QGuiApplication::instance()->installEventFilter(this);

    refreshColors();

    connect(this, &GcsTheme::darkModeChanged, this, [=] { Q_EMIT darkChanged(); });
    connect(this, &GcsTheme::darkChanged, this, [=] { refreshColors(); });
}

void GcsTheme::refreshColors() {
    auto isDark = dark();

    red(QColor(235, 0, 28));
    yellow(QColor(249, 188, 84));
    orange(QColor(249, 108, 17));
    green(QColor(14, 180, 14));
    blue(QColor(14, 14, 239));

    _Transparent = QColor(0, 0, 0, 0);
    _Black = QColor(0, 0, 0);
    _White = QColor(255, 255, 255);
    _Gray = QColor(155, 155, 155, 230);

    setvalueColor(QColor(205, 10, 250));
    itemWarnColor(QColor(250, 0, 0, 255));
    primaryColor(QColor(76, 166, 224, 255));
    itemPressColor(QColor(25, 155, 255, 255));

    dividerColor(isDark ? QColor(76, 166, 224, 255) : QColor(188, 188, 188, 255));
    itemGrayColor(isDark ? QColor(62, 62, 62, 255) : QColor(188, 188, 188, 255));
    backgroundColor(isDark ? QColor(0, 0, 0) : QColor(250, 250, 250));
    windowBorderColor(isDark ? QColor(48, 48, 48, 255) : QColor(188, 188, 188, 255));
    fontPrimaryColor(isDark ? QColor(248, 248, 248, 255) : QColor(7, 7, 7, 255));
    frameColor(isDark ? QColor(56, 56, 56, 255) : QColor(243, 243, 243, 255));
    itemNormalColor(isDark ? QColor(62, 62, 62, 255) : QColor(254, 254, 254, 255));
}

bool GcsTheme::dark() const {
    if (_darkMode == ThemeType::DarkMode::Dark) {
        return true;
    } else {
        return false;
    }
}
