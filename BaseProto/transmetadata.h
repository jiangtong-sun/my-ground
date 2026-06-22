#pragma once

#include "basemetadata.h"
#include "factlistmodel.h"
#include "flightwarning.h"
#include <QRegExpValidator>

enum WarningLevel { LevelOne = 1, LevelTwo = 2, LevelThree = 3, LevelFour = 4 };

enum ValueChecking {
    InvalidValue = 0,
    EqualValue = 1,
    GreateEqual = 2,
    LessEqual = 3,
    ColorValue = 4
};

enum ValueOperate {
    AndOperate = 1,
    OrOperate = 2,
    NotOperate = 3,
    AddOperate = 4,
    SubOperate = 5,
    EachOperate = 6,
    DirectOperate = 7,
    InvalidOperate = 10,
};

struct ReferenceDevice {
    ValueChecking cmp = InvalidValue;
    int16_t expect = -1;
    int16_t frame = -1;
    int16_t item = -1;
};

struct ReferenceValueCmp {
    bool _enable = false;
    ValueOperate _operate;
    ReferenceDevice _ref[2];
    void enableRefer() { _enable = true; }
    void SimpleRefer(const QString &val, int ix);
    void AndRefer(const QString &val);
    void OrRefer(const QString &val);
    void parseRefer(const QString &val);
};

using ReferDeviceCallback = std::function<bool(ReferenceValueCmp *ref, int chl)>;

struct BASEPROTO_EXPORT WarnConfig {
    uint8_t _frameItem = 0;
    int16_t _errorRow = 0;
    int16_t _moreOrLess = 0;
    int32_t _referFrame = 0;
    uint8_t _priorityValue[LevelFour + 1] = {0};
    uint8_t _checkCount[LevelFour + 1] = {0};
    int32_t _checkValue[LevelFour + 1] = {0};
    int16_t _referRow[LevelFour + 1] = {0};
    int16_t _referOpt[LevelFour + 1] = {0};
    QString _warnString[LevelFour + 1];
    QString _errString = "";
    QString _frameCode = "";

    uint32_t decoder(const QString &);
};

int64_t currentSysTimeMs();

using CheckWarnCallback = std::function<void(PWarnElemNode)>;

class BASEPROTO_EXPORT FactMetaData : public QObject {
    Q_OBJECT
    Q_ENUM(MetaValueType)
    Q_ENUM(WarningLevel)
    Q_PROPERTY(bool checked READ checked WRITE setChecked NOTIFY checkedChanged)
    Q_PROPERTY(int enumSelect READ enumSelect WRITE setEnumSelect NOTIFY enumSelChanged)
    Q_PROPERTY(QString rawInValue READ rawInValue WRITE setRawValue NOTIFY rawValueChanged)
    Q_PROPERTY(QRegExp regExp READ regExpress WRITE setRegExpress NOTIFY regExpChanged)
    Q_PROPERTY(QStringList enumValStr READ enumValStr CONSTANT)
    Q_PROPERTY(QString frameName READ frameName CONSTANT)
    Q_PROPERTY(QString groupName READ groupName CONSTANT)
    Q_PROPERTY(QString name READ name CONSTANT)
    Q_PROPERTY(QString bitSet READ getBitSet CONSTANT)
    Q_PROPERTY(QString units READ units CONSTANT)
    Q_PROPERTY(QString place READ place CONSTANT)
    Q_PROPERTY(int acolor READ acolor CONSTANT)
    Q_PROPERTY(bool visible READ visible CONSTANT)
    Q_PROPERTY(int byteCount READ byteCount CONSTANT)
    Q_PROPERTY(bool isString READ isString CONSTANT)
    Q_PROPERTY(bool isStruct READ isStruct CONSTANT)
    Q_PROPERTY(bool isEnum READ isEnum CONSTANT)

  public:
    FactMetaData() = default;
    FactMetaData(const FactMetaData &other);
    FactMetaData &operator=(const FactMetaData &other);

    static QString vertString(const QString &value);
    static QString vert2String(const QString &value);
    static QString vert3String(const QString &value);

    Q_INVOKABLE void manualUpdateData(int chl);
    Q_INVOKABLE void setRawValue(QString);
    Q_INVOKABLE void enumDefValue();

    void notifyRawDisplay();
    void setRawDisplayValue(int chl, int value); // for test

    QRegExp regExpress() const { return _regExpress; }
    void setRegExpress(const QRegExp &val) { _regExpress = val; }

    uint32_t rawValue(int chl) const;
    QString rawInValue(void) const;

    double rateValue(double) const;

    QString originValue(uint32_t input) {
        switch (_metaData.metaValueType()) {
        case valueTypeUint8: {
            uint8_t val = (uint8_t)(input * resolution() + _metaData.offset());
            return QString::number(val);
        }
        case valueTypeInt8: {
            int8_t val = (int8_t)(input * resolution() + _metaData.offset());
            return QString::number(val);
        }
        case valueTypeUint16: {
            uint16_t val = (uint16_t)(input * resolution() + _metaData.offset());
            return QString::number(val);
        }
        case valueTypeInt16: {
            int16_t val = (int16_t)(input * resolution() + _metaData.offset());
            return QString::number(val);
        }
        case valueTypeUint32: {
            uint32_t val = (uint32_t)(input * resolution() + _metaData.offset());
            return QString::number(val);
        }
        case valueTypeInt32: {
            int32_t val = (int32_t)(input * resolution() + _metaData.offset());
            return QString::number(val);
        }
        case valueTypeEnum: {
            return _metaData.enumStrById(input);
        }
        default:
            return QString();
        }
    }

    bool visible() const { return _visible; }

    bool checked() const { return _checked; }

    int enumSelect() const { return _selected & 0x7f; }

    void setChecked(bool vl) {
        _checked = vl;
        emit checkedChanged();
    }

    void setEnumSelect(int val) {
        _selected = val;
        emit enumSelChanged();
    }

    void itemVisible(int val) {
        _visible = val & 0x80 ? false : true;
        _selected = val & 0x7f;
        emit enumSelChanged();
    }

    ///
    /// \brief updated
    /// \param chl
    /// \return
    ///
    bool isUpdated(int chl);

    ///
    /// \brief isWarnUpdated
    ///     如果数据更新或时间超过1000ms进行数据状态告警分析
    /// \param chl
    ///     取某选择的数据通道
    /// \return
    ///
    bool isWarnUpdated(int chl);

    ///
    /// \brief setColor
    /// \param val
    ///
    void setColor(uint32_t val) { _metaData.setColor(val); }

    int acolor() const { return _metaData.acolor(); }

    ///
    /// \brief rangeValue
    ///     获取数据的范围最大最小值*
    /// \param low
    /// \param high
    /// \return
    ///
    int rangeValue(double &low, double &high) {
        ///
        return _metaData.rangeValue(low, high);
    }

    void setFrameName(const QString &val) { _frameName = val; }
    void setGroupName(const QString &val) { _groupName = val; }
    QString frameName() const { return _frameName; }
    QString groupName() const { return _groupName; }
    QString place(void) const;

    QVector<ColorSet> &ranges() { return _metaData.ranges(); }

    bool isString(void) const;
    bool isEnum(void) const;
    bool isStruct(void) const;
    int enumValue(int idx) const;
    int enumValueCheck(int idx) const;
    int enumValue(const QString &val) const;
    uint32_t value() const;
    QString enumStrById(int idx) const;
    QString enumStrByKey(int idx) const;
    QStringList enumValStr(void) const;
    MetaValueType metaValueType(void) const;
    MetaDataType metaDataType() const;
    double resolution() const;
    QString hexValue(int chl) const;
    QString getBitSet() const;
    int16_t itemNum() const;
    int8_t byteCount() const;
    QString name(void) const;
    QString display(int chl) const;
    QString units(void) const;

    void enableMultiDisplay() { _multiDisp = true; }
    void notifyDisplay(int chl);

    QString formatTimestamp(int chl, int sel, bool big);
    void setByteCount(int8_t val) { _metaData.setByteCount(val); }
    void setOffset(int32_t val) { _metaData.setOffset(val); }
    void setBitSet(const QString &val) { _metaData.setBitSet(val); }
    void setName(const QByteArray &val) { _metaData.setName(val); }
    void setMetaDataType(MetaDataType val) { _metaData.setMetaDataType(val); }
    void setMetaValueType(MetaValueType val) { _metaData.setMetaValueType(val); }
    void setUnits(const QString &val) { _metaData.setUnits(val); }
    void setResolution(double val) { _metaData.setResolution(val); }
    void setDestDataType(DestDataType val) { _metaData.setDestDataType(val); }
    void parseRange(const QString &val) { _metaData.parseRange(val); }
    void parseRange(const QString &val, QMap<int, QString> &values);
    void parseRangeExt(const QString &val, QMap<int, QString> &values);
    void parseColor(const QString &val) { _metaData.parseColor(val); }
    void setItemNum(const QString &val) { _metaData.setItemNum(val); }
    void parseRefer(const QString &val);

  signals:
    void multiDisplayChanged(int color, int);
    void displayChanged(QString val, int color);
    void rawDisplayChanged(const QString &val, int color, int);
    void defValueChanged();
    void rawValueChanged();
    void checkedChanged();
    void hexValueChanged();
    void enumSelChanged(void);
    void regExpChanged();

  protected:
    bool _visible = true;
    bool _checked = false;
    bool _rawNofify = false;
    bool _multiDisp = false;
    int _updateFreq = 1;
    int _selected = 0;
    int64_t updateTm = 0;
    struct ShowItem {
        uint16_t _refValue = 0;
        uint32_t _oldValue = 0xffffffff;
        uint32_t _rawValue = 0xffffffff;
        char _display[64] = {'N', 'A', '\0', '\0'};
    } datas[3];

    QString _rawInValue = "";
    QString _frameName = "";
    QString _groupName = "";
    QRegExp _regExpress;
    TypeMetaData _metaData;
    ReferenceValueCmp _refer;
};

Q_DECLARE_METATYPE(FactMetaData)

struct BASEPROTO_EXPORT WarnConfNode {
    int16_t value = 0;
    int16_t level = 0;
    int16_t color = 0;
    int16_t priority = 1;
    QString warnStr = "";
};

Q_DECLARE_METATYPE(WarnConfNode)

class BASEPROTO_EXPORT WarnConfigExt : public QObject {
    Q_OBJECT
    Q_DISABLE_COPY(WarnConfigExt)
  public:
    WarnConfigExt() = default;
    Q_PROPERTY(int popupWin READ popupWin CONSTANT)
    Q_PROPERTY(int sequence READ sequence CONSTANT)
    Q_PROPERTY(FactMetaData *display READ display CONSTANT)
    Q_PROPERTY(QString frameName READ frameName CONSTANT)
    Q_PROPERTY(QString warnItemName READ warnItemName CONSTANT)
    Q_PROPERTY(int warnColor READ warnColor NOTIFY warnColorChanged)

    int sequence() { return _sequence; }
    int popupWin() { return _popupWin; }
    int warnColor() { return _warnColor; }
    void resetWarnColor(int timeout);

    FactMetaData *display() { return _display; }
    QString frameName() { return _frameName; }
    QString warnItemName() { return _warnItemName; }
    std::shared_ptr<WarnConfNode> findByValue(int value);

  signals:
    void warnColorChanged(int);
    void connStateChanged();

  public:
    int _warnColor = 0;
    int _popupWin = 0;
    uint32_t _sequence = 0;
    uint32_t _frameCode = 0;
    uint32_t _frameOffset = 0;
    qint64 _warnTimeout = 0;
    QString _frameName = "";
    QString _warnItemName = "";
    FactMetaData *_display = nullptr;
    QMap<int, std::shared_ptr<WarnConfNode>> warns;
};

class BASEPROTO_EXPORT WarnConfigGroupVal : public QObject {
    Q_OBJECT
    Q_DISABLE_COPY(WarnConfigGroupVal)
    Q_PROPERTY(FactListModel *getAllMode READ getAllMode CONSTANT)
  public:
    WarnConfigGroupVal() = default;
    void resetWarnColor(int timeout);
    FactListModel *getAllMode() { return &allDataModel; }
    void addNode(std::shared_ptr<WarnConfigExt> node);
    std::shared_ptr<WarnConfigExt> findBySeq(uint32_t seq);

  private:
    FactListModel allDataModel;
    QVector<std::shared_ptr<WarnConfigExt>> groupElems;
};

class BASEPROTO_EXPORT WarnConfigGroupKey : public QObject {
    Q_OBJECT
    Q_DISABLE_COPY(WarnConfigGroupKey)
  public:
    WarnConfigGroupKey();

  public:
    uint8_t _referItem[4];
    uint8_t _referValue[4];
    uint32_t _referFrame[4];
    QString groupName = "";
};

typedef std::shared_ptr<WarnConfigGroupVal> PWarnConfGrpV;
typedef std::shared_ptr<WarnConfigGroupKey> PWarnConfGrpK;
