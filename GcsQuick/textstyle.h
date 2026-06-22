#pragma once

#include <QtQml/qqml.h>

#include <QFont>
#include <QObject>

#include "gcsstdafx.h"

class CONTROL_EXPORT TextStyle : public QObject {
    Q_OBJECT

  public:
    Q_PROPERTY_AUTO(int, fontSize)
    Q_PROPERTY_AUTO(int, Caption)
    Q_PROPERTY_AUTO(int, Body)
    Q_PROPERTY_AUTO(int, BodySmall)
    Q_PROPERTY_AUTO(QString, family)
    QML_NAMED_ELEMENT(TextStyle)
    QML_SINGLETON

  public:
    explicit TextStyle(QObject *parent = nullptr);

    static TextStyle *instance();
    static TextStyle *create(QQmlEngine *, QJSEngine *) { return instance(); }
};
