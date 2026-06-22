#include "SimpleTerrainPlot.h"
#include "customplot/qcustomplot.h"
#include "log_category.h"

#include <QDebug>

CustomPlotItem::CustomPlotItem(QQuickItem *parent)
    : QQuickPaintedItem(parent), customPlot(nullptr) {
    setFlag(QQuickItem::ItemHasContents, true);
    setAcceptedMouseButtons(Qt::AllButtons);
    connect(this, &QQuickPaintedItem::widthChanged, this, &CustomPlotItem::updateCustomPlotSize);
    connect(this, &QQuickPaintedItem::heightChanged, this, &CustomPlotItem::updateCustomPlotSize);
}

CustomPlotItem::~CustomPlotItem() {
    if (drawTimerId != 0) {
        killTimer(drawTimerId);
    }

    delete customPlot;
    customPlot = nullptr;
}

void CustomPlotItem::initCustomPlot() {
    customPlot = new QCustomPlot();
    customPlot->setGeometry(0, 0, (int) width(), (int) height());
    customPlot->setViewport(QRect(0, 0, (int) width(), (int) height()));
    customPlot->setBackground(QBrush(QColor(60, 60, 60)));
    customPlot->addGraph();
    customPlot->addGraph();
    customPlot->graph(0)->setPen(QPen(QColor(0, 250, 60)));
    customPlot->graph(1)->setPen(QPen(QColor(76, 166, 224)));
    customPlot->xAxis->setLabel(QString("位置"));
    customPlot->yAxis->setLabel(QString("高度"));
    customPlot->yAxis->setRange(-100, 100);
    customPlot->xAxis->setRange(0, 10);
    customPlot->xAxis->setLabelColor(Qt::lightGray);
    customPlot->xAxis->setTickLabelColor(Qt::lightGray);
    customPlot->yAxis->setLabelColor(Qt::lightGray);
    customPlot->yAxis->setTickLabelColor(Qt::lightGray);
    customPlot->xAxis->setBasePen(QPen(Qt::lightGray, 1));
    customPlot->yAxis->setBasePen(QPen(Qt::lightGray, 1));
    customPlot->xAxis->grid()->setVisible(false);
    customPlot->yAxis->grid()->setVisible(false);
    QCPScatterStyle scatterStyle;
    scatterStyle.setShape(QCPScatterStyle::ssCircle); // 设置形状为圆形
    scatterStyle.setSize(4); // 设置点的大小（直径，单位像素）
    // scatterStyle.setPen(QPen(Qt::black)); // 设置轮廓线颜色（黑色）
    // scatterStyle.setBrush(Qt::white); // 设置填充颜色（白色）
    customPlot->graph(0)->setScatterStyle(scatterStyle);
    customPlot->graph(1)->setScatterStyle(scatterStyle);
    // customPlot->xAxis->grid()->setPen(QPen(QColor(90, 90, 90), 1, Qt::DotLine));
    // customPlot->yAxis->grid()->setPen(QPen(QColor(90, 90, 90), 1, Qt::DotLine));
    connect(customPlot, &QCustomPlot::afterReplot, this, &CustomPlotItem::onCustomReplot);
    customPlot->setInteractions(QCP::iRangeDrag | QCP::iRangeZoom);
    customPlot->replot();
    /// 绘图更新数据的频率10HZ
    ///drawTimerId = startTimer(100);
}

void CustomPlotItem::timerEvent(QTimerEvent *event) {
    if (dataUpdate) {
        std::lock_guard<std::mutex> lock(dataUptMutex);
        QCPGraph * graph1 = customPlot->graph(0);
        QCPGraph * graph2 = customPlot->graph(1);
        if (graph2 != nullptr && graph1 != nullptr){
            graph1->setData(xAxisData, yAxisData1);
            graph2->setData(xAxisData, yAxisData2);
        }
        dataUpdate = false;
        int xAxisCount = xAxisData.count();
        xAxisCount = xAxisCount - xAxisCount%10;
        customPlot->xAxis->setRange(0, xAxisCount + 10);
        customPlot->yAxis->setRange(yAxisMinValue, 100+yAxisMaxValue);
        customPlot->replot();
    }
}

bool CustomPlotItem::isSameStage(StageController * stage){
    std::lock_guard<std::mutex> lock(dataUptMutex);
    if (_terrain_stage == stage) {
        return true;
    }

    COMMON_WARN("stage not equail {} ==> {}", (int)_terrain_stage, (int)stage);
    return false;
}

void CustomPlotItem::redrawTerrainLine(FlineStageDetail * stage){

}

void CustomPlotItem::resetDrawTerrain(){
    if (customPlot == nullptr) {
        COMMON_WARN("invalid plot");
        return;
    }

    int maxValue = 0;
    int minValue = 0;
    xAxisData.clear();
    yAxisData1.clear();
    yAxisData2.clear();
    yAxisMaxValue = maxValue - maxValue % 100 + 100;
    yAxisMinValue = minValue - minValue % 100 - 100;
    QCPGraph * graph1 = customPlot->graph(0);
    QCPGraph * graph2 = customPlot->graph(1);
    if (graph2 != nullptr && graph1 != nullptr){
        graph1->setData(xAxisData, yAxisData1);
        graph2->setData(xAxisData, yAxisData2);
    }
    int xAxisCount = xAxisData.count();
    xAxisCount = xAxisCount - xAxisCount%10;
    customPlot->xAxis->setRange(0, xAxisCount + 10);
    customPlot->yAxis->setRange(yAxisMinValue, 100+yAxisMaxValue);
    customPlot->replot();
    dataUpdate = true;
}

void CustomPlotItem::redrawTerrainStage(StageController * stage){
    if (customPlot == nullptr) {
        COMMON_WARN("invalid plot");
        return;
    }

    int maxValue = 0;
    int minValue = 0;
    xAxisData.clear();
    yAxisData1.clear();
    yAxisData2.clear();
    for (int ix = 0; ix < stage->counts(); ix++){
        const QVisualItem * curr = stage->at(ix);
        int altitude = curr->getAlt();
        int terrain = curr->terrain();
        if (ix < xAxisData.count()) {
            xAxisData[ix]  = ix;
            yAxisData1[ix] = altitude;
            yAxisData2[ix] = terrain;
        } else {
            xAxisData.append(ix);
            yAxisData1.append(altitude);
            yAxisData2.append(terrain);
        }
        maxValue = qMax(maxValue, qMax(altitude, terrain));
        minValue = qMin(minValue, qMin(altitude, terrain));
    }

    yAxisMaxValue = maxValue - maxValue % 100 + 100;
    yAxisMinValue = minValue - minValue % 100 - 100;

    QCPGraph * graph1 = customPlot->graph(0);
    QCPGraph * graph2 = customPlot->graph(1);
    if (graph2 != nullptr && graph1 != nullptr){
        graph1->setData(xAxisData, yAxisData1);
        graph2->setData(xAxisData, yAxisData2);
    }

    int xAxisCount = xAxisData.count();
    xAxisCount = xAxisCount - xAxisCount%10;
    customPlot->xAxis->setRange(0, xAxisCount + 10);
    customPlot->yAxis->setRange(yAxisMinValue, 100+yAxisMaxValue);
    customPlot->replot();

    dataUpdate = true;
}

void CustomPlotItem::setCurveColor(QString strTerrain,QString strAirLine) {
    customPlot->graph( 0 )->setPen( QPen(strTerrain));
    customPlot->graph( 1 )->setPen( QPen(strAirLine));
    customPlot->replot();
}

void CustomPlotItem::paint(QPainter *painter) {
    if (customPlot) {
        QPixmap picture(boundingRect().size().toSize());
        QCPPainter qcpPainter(&picture);
        customPlot->toPainter(&qcpPainter);
        setRenderTarget(QQuickPaintedItem::FramebufferObject);
        painter->drawPixmap(QPoint(), picture);
    }
}

void CustomPlotItem::mousePressEvent(QMouseEvent *event) { routeMouseEvents(event); }

void CustomPlotItem::mouseReleaseEvent(QMouseEvent *event) { routeMouseEvents(event); }

void CustomPlotItem::mouseMoveEvent(QMouseEvent *event) { routeMouseEvents(event); }

void CustomPlotItem::mouseDoubleClickEvent(QMouseEvent *event) { routeMouseEvents(event); }

void CustomPlotItem::wheelEvent(QWheelEvent *event) { routeWheelEvents(event); }

void CustomPlotItem::graphClicked(QCPAbstractPlottable *plottable) {
    qDebug() << Q_FUNC_INFO << QString("Clicked on graph '%1 ").arg(plottable->name());
}

void CustomPlotItem::routeMouseEvents(QMouseEvent *event) {
    if (customPlot) {
        QMouseEvent *newEvent = new QMouseEvent(event->type(), event->localPos(), event->button(),
                                                event->buttons(), event->modifiers());
        QCoreApplication::postEvent(customPlot, newEvent);
    }
}

void CustomPlotItem::routeWheelEvents(QWheelEvent *event) {
    if (customPlot) {
        QWheelEvent *newEvent = new QWheelEvent(event->pos(), event->delta(), event->buttons(),
                                                event->modifiers(), event->orientation());
        QCoreApplication::postEvent(customPlot, newEvent);
    }
}

void CustomPlotItem::updateCustomPlotSize() {
    if (customPlot) {
        customPlot->setGeometry(0, 0, (int) width(), (int) height());
        customPlot->setViewport(QRect(0, 0, (int) width(), (int) height()));
    }
}

void CustomPlotItem::onCustomReplot() { update(); }
