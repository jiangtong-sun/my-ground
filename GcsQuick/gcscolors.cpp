#include "gcscolors.h"

#include <QGlobalStatic>

Q_GLOBAL_STATIC(GcsColors, gColor)

GcsColors *GcsColors::instance() { return gColor(); }

GcsColors::GcsColors(QObject *parent) : QObject{parent} {
    auto sfont = new AccentColor(this);
    sfont->darkest(QColor(10, 10, 10));
    sfont->darker(QColor(50, 50, 50));
    sfont->dark(QColor(0, 255, 212));
    sfont->normal(QColor(10, 240, 248));
    sfont->light(QColor(250, 250, 250));
    _SFont = sfont;
}
