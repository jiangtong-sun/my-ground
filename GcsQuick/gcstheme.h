#pragma once

#include <QtQml/qqml.h>

#include <QColor>
#include <QMutex>
#include <QObject>

#include "gcsstdafx.h"

class CONTROL_EXPORT GcsTheme : public QObject {
    Q_OBJECT
    Q_PROPERTY(bool dark READ dark NOTIFY darkChanged)

    Q_PROPERTY_AUTO(QColor, red)
    Q_PROPERTY_AUTO(QColor, orange)
    Q_PROPERTY_AUTO(QColor, yellow)
    Q_PROPERTY_AUTO(QColor, green)
    Q_PROPERTY_AUTO(QColor, blue)
    Q_PROPERTY_AUTO(QColor, Transparent)
    Q_PROPERTY_AUTO(QColor, Black)
    Q_PROPERTY_AUTO(QColor, White)
    Q_PROPERTY_AUTO(QColor, Gray)

    Q_PROPERTY_AUTO(QColor, primaryColor)
    Q_PROPERTY_AUTO(QColor, setvalueColor)
    Q_PROPERTY_AUTO(QColor, backgroundColor)
    Q_PROPERTY_AUTO(QColor, dividerColor)
    Q_PROPERTY_AUTO(QColor, windowBorderColor)
    Q_PROPERTY_AUTO(QColor, fontPrimaryColor)
    Q_PROPERTY_AUTO(QColor, frameColor)
    Q_PROPERTY_AUTO(QColor, itemNormalColor)
    Q_PROPERTY_AUTO(QColor, itemEnableColor)
    Q_PROPERTY_AUTO(QColor, itemWarnColor)
    Q_PROPERTY_AUTO(QColor, itemPressColor)
    Q_PROPERTY_AUTO(QColor, itemGrayColor)
    Q_PROPERTY_AUTO(QString, iconfront)
    Q_PROPERTY_AUTO(int, darkMode)
    QML_NAMED_ELEMENT(GcsTheme)
    QML_SINGLETON

  protected:
    void refreshColors();

  signals:
    void darkChanged();

  public:
    explicit GcsTheme(QObject *parent = nullptr);

    static GcsTheme *instance();
    static GcsTheme *create(QQmlEngine *, QJSEngine *) { return instance(); }

    bool dark() const;

  private:
    bool _systemDark;
    QMutex _mutex;
};
