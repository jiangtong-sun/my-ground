#pragma once

#include <QtQml/qqml.h>

#include <QColor>
#include <QObject>

#include "gcsstdafx.h"

class CONTROL_EXPORT AccentColor : public QObject {
    Q_OBJECT
    Q_PROPERTY_AUTO(QColor, darkest)
    Q_PROPERTY_AUTO(QColor, darker)
    Q_PROPERTY_AUTO(QColor, dark)
    Q_PROPERTY_AUTO(QColor, normal)
    Q_PROPERTY_AUTO(QColor, light)
    QML_NAMED_ELEMENT(AccentColor)

  public:
    explicit AccentColor(QObject *parent = nullptr);
};
