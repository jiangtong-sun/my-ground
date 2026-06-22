#include "axis.h"
#include "grid.h"
#include "ticker.h"
#include <stdexcept>

Axis::Axis(QCPAxis *axis, QCustomPlot *parentPlot, QObject *parent)
    : m_parentPlot(parentPlot), m_axis(axis), QObject(parent) {
    if (parentPlot == nullptr || axis == nullptr)
        throw std::invalid_argument(nullptr);
    connect(parentPlot, &QCustomPlot::beforeReplot, this, &Axis::updateProperty);
    connect(axis, &QCPAxis::destroyed, this, &Axis::deleteLater);
    m_ticker = new Ticker(axis, m_parentPlot, this);
    m_grid = new Grid(axis->grid(), m_parentPlot, this);
    updateProperty();
}

Axis::~Axis() {}

void Axis::transparentAxis() {
    if (m_axis) {
        m_axis->setLabelColor(Qt::transparent);
        m_axis->setTickLabelColor(Qt::transparent);
        m_axis->setBasePen(QPen(Qt::transparent, 1));
    }
}

void Axis::setRange(float position, float size, Qt::AlignmentFlag align) noexcept {
    m_axis->setRange(position, size, align);
    m_parentPlot->replot();
}

Q_INVOKABLE void Axis::setRange(float lower, float upper) noexcept { m_axis->setRange(lower, upper); }

void Axis::setTicker(QSharedPointer<QCPAxisTicker> ticker) noexcept {
    m_axis->setTicker(ticker);
    m_parentPlot->replot();
}

void Axis::set_visible(bool value) noexcept {
    m_visible = m_axis->visible();
    if (m_visible == value)
        return;
    m_visible = value;
    m_axis->setVisible(value);
    Q_EMIT visibleChanged(m_visible);
    m_parentPlot->replot();
}

void Axis::set_label(const QString &value) noexcept {
    m_label = m_axis->label();
    if (m_label == value)
        return;
    m_label = value;
    m_axis->setLabel(value);
    Q_EMIT labelChanged(m_label);
    m_parentPlot->replot();
}

void Axis::set_upper(float value) noexcept {
    m_upper = m_axis->range().upper;
    if (m_upper == value)
        return;
    m_upper = value;
    m_axis->setRangeLower(value);
    Q_EMIT upperChanged(m_upper);
    m_parentPlot->replot();
}

void Axis::set_lower(float value) noexcept {
    m_lower = m_axis->range().lower;
    if (m_lower == value)
        return;
    m_lower = value;
    m_axis->setRangeUpper(value);
    Q_EMIT lowerChanged(m_lower);
    m_parentPlot->replot();
}

void Axis::updateProperty() noexcept {
    m_visible = m_axis->visible();
    m_label = m_axis->label();
    m_upper = m_axis->range().upper;
    m_lower = m_axis->range().lower;
    Q_EMIT visibleChanged(m_visible);
    Q_EMIT labelChanged(m_label);
    Q_EMIT upperChanged(m_upper);
    Q_EMIT lowerChanged(m_lower);
}
