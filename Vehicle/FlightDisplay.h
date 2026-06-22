#pragma once

#include "VehicleMetadef.h"
#include "factdecodermgr.h"
#include "transmetadata.h"
#include <QColor>
#include <QVector>

class VEHICLE_EXPORT FlightDisplay : public QObject {
    Q_OBJECT
    Q_DISABLE_COPY(FlightDisplay)
  public:
    FlightDisplay();
    ~FlightDisplay() = default;

    Q_PROPERTY(int frameCode READ frameCode CONSTANT)
    Q_PROPERTY(int showIndex READ showIndex CONSTANT)
    Q_PROPERTY(int showMode READ showMode CONSTANT)
    Q_PROPERTY(QString frameName READ frameName CONSTANT)
    Q_PROPERTY(QString itemName READ itemName CONSTANT)
    Q_PROPERTY(double rateValue READ rateValue CONSTANT)
    Q_PROPERTY(int showColor READ showColor CONSTANT)
    Q_PROPERTY(QString display READ display NOTIFY displayValueChanged)
    Q_PROPERTY(QStringList enumValStr READ enumValStr CONSTANT)
    Q_PROPERTY(QString vertName READ vertName CONSTANT)
    Q_PROPERTY(QString formatStr READ formatStr CONSTANT)
    Q_PROPERTY(QString name READ name CONSTANT)
    Q_PROPERTY(QString unit READ unit CONSTANT)
    Q_PROPERTY(QString range READ range CONSTANT)

    double rateValue() const { return _rateValue; }

    int frameCode() const { return _frameCode; }
    int showIndex() const { return _showIndex; }
    int showColor() const { return _showColor; }
    int showMode() const { return _showMode; }
    int rawValue() const { return _rawValue; }

    void notifyRawDisplay();

    QString frameName() const { return _frameName; }
    QString itemName() const { return _itemName; }
    QString vertName() const { return _vertName; }
    QString vert2Name() const { return _vert2Name; }
    QString vert3Name() const { return _vert3Name; }
    QString display() const { return _valueStr; }
    QString formatStr() const { return _formatStr; }

    void setShowIndex(const QString &va);
    void setFrameCode(const QString &va);
    void setShowMode(int va) { _showMode = va; }
    void setFrameCode(int va) { _frameCode = va; }
    void setShowIndex(int va) { _showIndex = va; }
    void setFormatStr(const QString &va) { _formatStr = va; }
    void setFrameName(const QString &va) { _frameName = va; }
    void setItemName(const QString &va);
    void setCondition(const QString &cond);

    ///
    /// \brief insertFactElem
    /// \param elem
    ///
    void insertFactElem(FactMetaData *elem);
    ///
    /// \brief insertFactElemExt
    /// \param left
    /// \param right
    ///
    void insertFactElemExt(FactMetaData *left, FactMetaData *right);

    ///
    /// \brief parseDispaySets
    /// \param sets
    ///
    void parseDispaySets(FactDecoderMgr *decoder, const QString &sets);
    void parseDispayColor(const QString &sets);

    void logicalDispayColor(const QString &sets, int idx);
    void airthmeticDispayColor(const QString &sets, int idx);

    Q_INVOKABLE void manualUpdate(int chl);

    bool stateValue(int chl) const;
    void unittestDisplay(int value, int chl);
    void setDisplay(const QString &name);
    void afterUpdateDisplay();

    int checkDisplayColor(int chl);

    QVector<ColorSet> &ranges() {
        assert(factdata != nullptr);
        return factdata->ranges();
    }

    QStringList enumValStr() const;
    QString range() const;
    QString name() const;
    QString unit() const;

  private slots:
    void doDisplayUpdate(QString val, int color);
    void doDisplayUpdateExt(int color, int chl);
    void doRawDisplayChanged(const QString &val, int color, int);
    void checkRawDataChanged(int color, int channel);

  signals:
    void displayValueChanged();
    void updateColorValueExt(QString val, int color);
    void updateColorValue(QString val, int color);

  private:
    ValueOperate _operate = InvalidOperate;
    int _frameCode = 0;
    int _showIndex = 0;
    int _showColor = 0;
    int _showMode = 0;
    int _rawValue = 0;
    int _settingColor = 0;
    double _rateValue = 0;
    QString _formatStr = "";
    QString _valueStr = "NA";
    QString _valueStr2 = "NA";
    QString _frameName = "";
    QString _vertName = "";
    QString _vert2Name = "";
    QString _vert3Name = "";
    QString _itemName = "";
    FactMetaData *factdata = nullptr;
    FactMetaData *factdata2 = nullptr;

    QList<FactMetaData *> displaySets;
    enum LogicalType { LogicalInvalid, LogicalAnd, LogicalOr, LogicalEqual };
    enum AirthOperate { MoreThan, LessThan };

    struct ConditionNode {
        int airth;
        int compare;
        int offset;
    };

    QList<ConditionNode> _condList;

    struct AirthmeticSub {
        float apoint = -1;
        float bpoint = -1;
        int abpoint = -1;
        int index = 0;
        int dtype = -1;
    };

    struct AirthmeticPart {
        float apoint = -1;
        float bpoint = -1;
        int dtype = -1;
        int abpoint = -1;
        int index = 0;

        QList<std::shared_ptr<AirthmeticSub>> subNode;
    };

    struct AirthmeticOperation {
        int color = 0;
        int optype = LogicalInvalid;
        QList<std::shared_ptr<AirthmeticPart>> airthOpts;
    };

    struct LogicalOptPart {
        int optype = LogicalInvalid;
        QList<int> logical;
        bool checkDisplayColor(QList<FactMetaData *> &, int chl);
    };

    struct AirthmeticOptPart {
        int optype = LogicalInvalid;
        QList<std::shared_ptr<AirthmeticPart>> airthOpts;
        bool checkDisplayColor(QList<FactMetaData *> &, int chl);
    };

    struct LogicalOperation {
        int color = 0;
        int optype = LogicalInvalid;
        AirthmeticOptPart airthpart;
        QList<std::shared_ptr<LogicalOptPart>> part1;
    };

    QList<AirthmeticOperation> airthmeticOpts;
    QList<std::shared_ptr<LogicalOperation>> logicalOpts;

    void calculateColor(const QString &data, AirthmeticOperation &);
    void calculateColorExt(const QString &data, AirthmeticOperation &);
};
