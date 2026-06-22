#pragma once

#include "CommTableModel.h"

class FeedbackDataNode : public CommElemNode {
    Q_OBJECT
    VEH_PROPERTY_RW(bool, warn);
    VEH_PROPERTY_RW(qint64, key);
    VEH_PROPERTY_RW(QString, delay);
    VEH_PROPERTY_RW(QString, time);
    VEH_PROPERTY_RW(QString, content);
    VEH_PROPERTY_RW(QString, feedback);

  public:
    FeedbackDataNode() : _delay(""), _key(0), _time(""), _content(""), _feedback(""), _warn(0) {}
    QVariant elemState() override { return _warn; }
    QVariant columnValue(int column) override {
        if (column == 0) {
            return _time;
        } else if (column == 1) {
            return _content + "/" + _feedback;
        }
        return QVariant();
    }
};

class VEHICLE_EXPORT VehicleFeedback : public CommTableModel {
    Q_OBJECT
  public:
    VehicleFeedback(QObject *parent = nullptr) : CommTableModel(parent) { ; }
    ~VehicleFeedback() = default;

    int recordCommand(QString comandName);
    bool updateFeedback(const QString &expect, int row, int during, bool state);
    void initColumnHeader();
  signals:
    void currCommandChanged(QString req, QString res, int);
};
