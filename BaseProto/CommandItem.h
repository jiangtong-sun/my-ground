#pragma once

#include "factlistmodel.h"
#include "protometadef.h"
#include "transmetadata.h"
#include <QObject>

class BASEPROTO_EXPORT RCCommandItem : public QObject {
    Q_OBJECT
    Q_DISABLE_COPY(RCCommandItem)

  public:
    RCCommandItem();

    Q_PROPERTY(int index READ index CONSTANT)
    Q_PROPERTY(int popup READ popup CONSTANT)
    Q_PROPERTY(QString group READ group CONSTANT)
    Q_PROPERTY(QString vName READ vName CONSTANT)
    Q_PROPERTY(QString refAdjust READ refAdjust CONSTANT)
    Q_PROPERTY(int feed READ feed WRITE setFeed NOTIFY feedChanged)
    Q_PROPERTY(bool result READ result WRITE setResult NOTIFY resultChanged)
    Q_PROPERTY(bool working READ working WRITE setWorking NOTIFY workingChanged)
    Q_PROPERTY(QString name READ name WRITE setName NOTIFY nameChanged)

    void toString();

    int feed() { return _feed; }
    int index() const { return _index; }
    int popup() const { return _popup; }
    bool result() const { return _result; }
    bool working() const { return _working; }
    QString vName() const { return _vName; }
    QString name() const { return _name; }
    QString group() const { return _group; }
    QString refAdjust() const { return _refAdjust; }

    void setFeed(int val) { _feed = val; }
    void setIndex(int val) { _index = val; }
    void setResult(bool val) { _result = val; }
    void setPopup(bool val) { _popup = val; }
    void setWorking(bool val) {
        _working = val;
        emit workingChanged();
    }

    void setRefAdjust(const QString &val) { _refAdjust = val; }
    void setGroup(const QString &val) { _group = val; }
    void setName(const QString &val) {
        _vName = FactMetaData::vert3String(val);
        _name = val;
    }

  signals:
    void feedChanged();
    void nameChanged();
    void resultChanged();
    void workingChanged();

  protected:
    int _feed = -1;
    int _popup = 0;
    int _index = 0;
    bool _result = false;
    bool _working = false;
    QString _refAdjust = "";
    QString _vName = "";
    QString _group = "";
    QString _type = "";
    QString _name = "";
};

class BASEPROTO_EXPORT RCAdjustItem : public QObject {
    Q_OBJECT
    Q_DISABLE_COPY(RCAdjustItem)

  public:
    RCAdjustItem();
    ~RCAdjustItem();
    Q_PROPERTY(bool defSelect READ defSelect WRITE setDefSelect NOTIFY defSelectChanged)
    Q_PROPERTY(int selectId READ selectId WRITE setSelectId NOTIFY selectIdChanged)
    Q_PROPERTY(int mapSelect READ mapSelect WRITE setMapSelect NOTIFY mapSelectChanged)
    Q_PROPERTY(int selectSub READ selectSub WRITE setSelectSub NOTIFY selectSubChanged)
    Q_PROPERTY(int feedback READ feedback WRITE setFeedback NOTIFY feedbackChanged)
    Q_PROPERTY(bool working READ working WRITE setWorking NOTIFY workingChanged)
    Q_PROPERTY(bool result READ result WRITE setResult NOTIFY resultChanged)

    Q_PROPERTY(FactListModel *moreMetaModel READ moreMetaModel CONSTANT)
    Q_PROPERTY(QStringList enumValStr READ enumValStr CONSTANT)
    Q_PROPERTY(FactMetaData *metaData READ metaData CONSTANT)
    Q_PROPERTY(QString placeText READ placeText CONSTANT)
    Q_PROPERTY(QString groupName READ groupName CONSTANT)
    Q_PROPERTY(QRegExp regExpress READ regExpress CONSTANT)
    Q_PROPERTY(QString defValue READ defValue CONSTANT)
    Q_PROPERTY(QString vertName READ vertName CONSTANT)
    Q_PROPERTY(QString name READ name CONSTANT)
    Q_PROPERTY(QString units READ units CONSTANT)

    bool defSelect() { return _defSelect; }
    int feedback() { return _feedback; }
    bool working() { return _working; }
    bool result() { return _result; }

    Q_INVOKABLE void setRawValue(QString val) { _metaData.setRawValue(val); }

    QRegExp regExpress() const { return _metaData.regExpress(); }

    FactListModel *moreMetaModel() { return &_moreMetaData; }

    QString enumStrById(int idx) const { return _metaData.enumStrById(idx); }

    QString enumStrByKey(int idx) const { return _metaData.enumStrByKey(idx); }

    QStringList enumValStr() const { return _metaData.enumValStr(); }

    QString name() const { return _metaData.name(); }

    QString units() const { return _metaData.units(); }

    QString groupName() const { return _groupName; }

    QString defValue() const { return _defaultValue; }

    uint32_t value() const { return _metaData.value(); }

    uint32_t byteCount() const { return _metaData.byteCount() / 8; }

    QString placeText() const { return _metaData.place(); }

    int countSubMeta() const { return _moreMetaData.count(); }

    void setDefValue(const QString &val) { _defaultValue = val; }

    int selectId() const { return _selectId; }

    int mapSelect() const { return _mapSelect; }

    int itemNum() const { return _metaData.itemNum(); }

    int selectSub() const { return _subElemIdx; }

    void setSelectSub(int idx) { _subElemIdx = idx; }

    static QString vertSplitStr(const QString &value);

    int commandType() const { return _commandType; }

    void setGroupName(const QString &value) { _groupName = value; }

    void setCommandType(const QString &value) {
        if (value.contains("0x") || value.contains("0X")) {
            bool ok = false;
            _commandType = value.trimmed().toInt(&ok, 16);
        } else {
            bool ok = false;
            QByteArray binStr(value.trimmed().toUtf8());
            _commandType = binStr.toInt(&ok, 2);
        }
    }

    QString vertName() const;

    void setMapSelect(int no) {
        _mapSelect = no;
        emit mapSelectChanged();
    }

    void setSelectId(int sel) {
        _selectId = sel;
        emit selectIdChanged();
    }

    void setDefSelect(bool val) {
        _defSelect = val;
        emit defSelectChanged();
    }

    void setFeedback(int val) {
        _feedback = val;
        emit feedbackChanged();
    }

    void setWorking(bool val) {
        _working = val;
        emit workingChanged();
    }

    void setResult(bool val) {
        _result = val;
        emit resultChanged();
    }

    FactMetaData *metaData() { return &_metaData; }

    void addMetaData(FactMetaData *);

    const FactMetaData *mData() { return metaData(); }

    const FactMetaData *fectElem(int);

    QString getCmdNameByCode(int code);

    FactMetaData *getElem(int);

  signals:
    void mapSelectChanged();
    void defSelectChanged();
    void workingChanged();
    void resultChanged();
    void feedbackChanged();
    void selectIdChanged();
    void selectSubChanged();

  protected:
    FactMetaData _metaData;
    FactListModel _moreMetaData;
    QString _defaultValue;
    QString _groupName;
    bool _working = false;
    bool _result = false;
    bool _defSelect = false;
    int _mapSelect = -1;
    int _commandType = 0;
    int _subElemIdx = -1;
    int _selectId = -1;
    int _feedback = 0;
};
