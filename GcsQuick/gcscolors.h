#pragma once

#include <QtQml/qqml.h>
#include <QObject>
#include "accentcolor.h"
#include "gcsstdafx.h"

class CONTROL_EXPORT GcsColors : public QObject {
    Q_OBJECT
    Q_PROPERTY_AUTO_P(AccentColor *, SFont)
    QML_NAMED_ELEMENT(GcsColors)
    QML_SINGLETON

  public:
    explicit GcsColors(QObject *parent = nullptr);
    static GcsColors *instance();
    static GcsColors *create(QQmlEngine *, QJSEngine *) { return instance(); }
};
