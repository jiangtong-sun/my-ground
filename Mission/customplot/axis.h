#pragma once

#include "macros.h"
#include "qcustomplot.h"
#include <QtCore/QObject>

class Grid;
class Ticker;

class Axis : public QObject {
    Q_OBJECT

    QML_READ_WRITE_NOTIFY_PROPERTY(bool, visible)
    QML_READ_WRITE_NOTIFY_PROPERTY(QString, label)
    QML_READ_WRITE_NOTIFY_PROPERTY(float, upper)
    QML_READ_WRITE_NOTIFY_PROPERTY(float, lower)

    QML_READ_CONSTANT(Grid *, grid)
    QML_READ_CONSTANT(Ticker *, ticker)

  public:
    Axis(QCPAxis *axis, QCustomPlot *parentPlot, QObject *parent = nullptr);
    ~Axis();

    Q_INVOKABLE void setRange(float position, float size, Qt::AlignmentFlag align) noexcept;
    Q_INVOKABLE void setRange(float lower, float upper) noexcept;
    Q_INVOKABLE void transparentAxis();

    void setTicker(QSharedPointer<QCPAxisTicker> ticker) noexcept;

    void set_visible(bool value) noexcept;
    void set_label(const QString &value) noexcept;
    void set_upper(float value) noexcept;
    void set_lower(float value) noexcept;

  private:
    void updateProperty() noexcept;

  private:
    QCustomPlot *m_parentPlot = nullptr;
    QCPAxis *m_axis = nullptr;
};
