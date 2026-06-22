#include "simpleplot.h"
#include "axis.h"
#include "graph.h"
#include <QDebug>

SimplePlot::SimplePlot(QQuickItem *parent) : BasePlot(parent), m_timer(new QTimer(this)) {
    connect(m_timer, &QTimer::timeout, this, &SimplePlot::timeOutPrcess);
    // xAxis()->setRange(0, 100);
}

SimplePlot::~SimplePlot() {}


void SimplePlot::clearCurveData(const QString &name){
    auto graph = getGraph(name);
    if (graph != nullptr) {
        graph->clearData();
    }
}

void SimplePlot::timeOutPrcess() noexcept {}

void SimplePlot::drawSpecialLine(double posx, const QString &name) {
    auto graph = getGraph(name);
    if (graph != nullptr) {
        graph->addData(0, posx);
        graph->addData(10000, posx);
    }
}

void SimplePlot::addLateralPosValue(const QString &name, double dst, double value) noexcept {
    auto graph = getGraph(name);
    if ((graph != nullptr) && (dst > 0.1)) {
        int curIdx = ++currIndex;
        int curAxis = currAxisMin++;
        graph->addData(curIdx, value);
        xAxis()->setRange(curAxis, 100 + curAxis);
        if (curIdx > 500) {
            graph->removeDataBefore(100);
        }
    }
}

void SimplePlot::addGraphHighValue(const QString &name, double dst, double high) noexcept {
    auto graph = getGraph(name);
    if ((graph != nullptr) && (dst > 0.1) && (currYAxisMax != 0) && (high < currYAxisMax)) {
        graph->addData(dst + 500, high);
    }
}

void SimplePlot::addLandSetpoints(const QString &name, FlightLanding *conf) noexcept {
    if (conf == nullptr) {
        return;
    }

    double rad2deg = 3.1415 / 180;
    double gama1 = std::tan(conf->gamaSteep() * rad2deg);
    double gama2 = std::tan(conf->gamaShallow() * rad2deg);
    double expK = (conf->pullStart() - conf->pullNext()) * gama1 / (gama1 - gama2);
    double expTao = (conf->pullStart() - conf->pullNext()) / (gama2 - gama1);
    double expH = (conf->pullStart() * gama2 - conf->pullNext() * gama1) / (gama1 - gama2);
    double startD = 0, nextD = 0;
    double mx = expTao * std::log((expH + conf->pullNext()) / expK);
    startD = conf->pullNext() / (-1.0 * gama2) - mx + conf->dstOffset();
    nextD = conf->pullNext() / (-1.0 * gama2) + conf->dstOffset();

    int xpos = 9000;
    auto graph = getGraph(name);

    setpointKeys.clear();
    setpointValues.clear();
    for (int ix = xpos; ix >= 0; ix--) {
        double high = 0;
        if (ix > startD) {
            high = -1.0 * (ix - startD) * gama1 + conf->pullStart();
        } else if (ix > nextD) {
            high = expK * std::exp((ix - startD) / expTao) - expH;
        } else {
            high = -1.0f * (ix - nextD) * gama2 + conf->pullNext();
        }
        setpointKeys.append(ix + 500);
        high = high > conf->landEnter() ? conf->landEnter() : high;
        currYAxisMax = currYAxisMax > high ? currYAxisMax : high;
        setpointValues.append(high);
    }

    graph->setData(setpointKeys, setpointValues);
}

void SimplePlot::addGraphPoints(const QString &name, const QVariantList &values) noexcept {
    auto graph = getGraph(name);
    int maxHeight = yAxis()->upper();
    for (auto it = values.begin(); graph && it != values.end(); ++it) {
        QPoint elem = (*it).toPoint();
        graph->addData(elem.x(), elem.y());
        maxHeight = maxHeight > elem.y() ? maxHeight : elem.y();
    }

    if (maxHeight != yAxis()->upper()) {
        int height = maxHeight - maxHeight % 100;
        yAxis()->set_upper(height + 100);
    }
}

void SimplePlot::timerEvent(QTimerEvent *event) { customPlot()->replot(); }
