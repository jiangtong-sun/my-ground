#include "TextStyle.h"
#include <QFontDatabase>
#include <QGlobalStatic>

Q_GLOBAL_STATIC(TextStyle, gTxtStyle)

TextStyle *TextStyle::instance() { return gTxtStyle(); }

TextStyle::TextStyle(QObject *parent) : QObject{parent} {
    _family = QString("微软雅黑");
    _fontSize = 18;
    Caption(20);
    Body(18);
    BodySmall(16);
}
