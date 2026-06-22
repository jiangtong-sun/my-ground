#pragma once

#include "FLineController.h"
#include "StageController.h"
#include <QtQuick>

class QCustomPlot;
class QCPAbstractPlottable;
class CustomPlotItem : public QQuickPaintedItem {
    Q_OBJECT
  public:
    CustomPlotItem(QQuickItem *parent = 0);
    virtual ~CustomPlotItem();

    bool isSameStage(StageController*);
    void paint(QPainter *painter);

    void setCurveColor(QString strTerrain,QString strAirLine);

    Q_INVOKABLE void initCustomPlot();
    Q_INVOKABLE void resetDrawTerrain();
    Q_INVOKABLE void redrawTerrainStage(StageController *);
    Q_INVOKABLE void redrawTerrainLine(FlineStageDetail *);

  protected:

    void routeMouseEvents(QMouseEvent *event);
    void routeWheelEvents(QWheelEvent *event);

    virtual void mousePressEvent(QMouseEvent *event);
    virtual void mouseReleaseEvent(QMouseEvent *event);
    virtual void mouseMoveEvent(QMouseEvent *event);
    virtual void mouseDoubleClickEvent(QMouseEvent *event);
    virtual void wheelEvent(QWheelEvent *event);
    virtual void timerEvent(QTimerEvent *event);

  signals:
    void stageLineChanged();
    void taskLineChanged();

  private:
    std::mutex dataUptMutex;
    QVector<double> xAxisData;
    QVector<double> yAxisData1;
    QVector<double> yAxisData2;

    StageController * _terrain_stage = nullptr;
    FlineStageDetail * _terrain_fline = nullptr;

    QCustomPlot *customPlot = nullptr;
    std::atomic_bool dataUpdate = false;
    int yAxisMaxValue = 0;
    int yAxisMinValue = -10;
    int drawTimerId = 0;


  private slots:
    void graphClicked(QCPAbstractPlottable *plottable);
    void onCustomReplot();
    void updateCustomPlotSize();
};
