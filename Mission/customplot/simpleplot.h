#pragma once

#include "ConfigNetCom.h"
#include "baseplot.h"

#include <QGeoCoordinate>
#include <QTimer>

class SimplePlot : public BasePlot {
    Q_OBJECT
  public:
    SimplePlot(QQuickItem *parent = nullptr);
    ~SimplePlot();

    Q_INVOKABLE void addLandSetpoints(const QString &name, FlightLanding *values) noexcept;
    Q_INVOKABLE void addGraphPoints(const QString &name, const QVariantList &values) noexcept;
    Q_INVOKABLE void addGraphHighValue(const QString &name, double dst, double high) noexcept;
    Q_INVOKABLE void addLateralPosValue(const QString &name, double dst, double value) noexcept;
    Q_INVOKABLE void drawSpecialLine(double pos, const QString &name);
    Q_INVOKABLE void clearCurveData(const QString &name);

  public slots:
    void timeOutPrcess() noexcept;

  protected:
    void timerEvent(QTimerEvent *event) override;

  private:
    int currIndex = 70;
    int currAxisMin = 0;
    int currYAxisMax = 0;
    QVector<double> setpointKeys;
    QVector<double> setpointValues;
    QTimer *m_timer = nullptr;
};
