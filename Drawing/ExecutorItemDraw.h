#pragma once

#include "DrawingDefine.h"
#include <QQuickPaintedItem>

class QuickExecutorElem : public QQuickPaintedItem {
    Q_OBJECT
    DRAW_PROPERTY_AUTO(QColor, color)
    QML_NAMED_ELEMENT(QuickExecutorElem)

  public:
    QuickExecutorElem(QQuickItem *parent = nullptr);

    void paint(QPainter *painter) override;

  private slots:
    void doUpdateQuick();

  signals:
    void nodeChanged();

  protected:
};
