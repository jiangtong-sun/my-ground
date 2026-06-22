#pragma once

#include "protometadef.h"
#include <QColor>
#include <QMap>
#include <QString>
#include <QVariant>
#include <type_traits>

typedef enum {
    valueTypeUndef,
    valueTypeEnum,
    valueTypeUint8,
    valueTypeInt8,
    valueTypeUint16,
    valueTypeInt16,
    valueTypeUint32,
    valueTypeInt32,
    valueTypeStruct,
    valueTypeDouble,
    valueTypeString,
} MetaValueType;

typedef enum {
    enumType = 0x01,
    signedType = 0x02,
    unsignedType = 0x04,
    structType = 0x08,
    stringType = 0x10,
    undefineType = 0x20,
} MetaDataType;

typedef enum {
    translatorNull,   // = N/A
    translatorInt32,  // = int32
    translatorEnums,  // = int32
    translatorString, // = string
    translatorDouble, // = double
    translatorStruct  // = struct
} DestDataType;

typedef enum {
    GreenColor = 0x00,
    RedColor = 0x01,
    OrangeColor = 0x02,
    YellowColor = 0x03,
    WhiteColor = 0x04,
    UnsetColor = 0x10
} MetaColorType;

template <typename T>
bool datatype_is_signed(T v) {
    return std::is_signed<T>::value;
}

template <typename T>
bool datatype_is_integral(T v) {
    return std::is_integral<T>::value;
}

struct ColorSet {
    float ivalue = 0;
    float value = 0;
    int32_t color = 0;
    int16_t type = 0;
};

class BASEPROTO_EXPORT TypeMetaData {
  public:
    TypeMetaData();
    TypeMetaData(const TypeMetaData &other);
    TypeMetaData &operator=(const TypeMetaData &other);

    //设置告警颜色
    void clearColor() { _color = 0; }

    //设置字节数
    void setByteCount(int8_t);

    //设置范围
    void setRange(const QString &sz);

    //设置偏移量
    void setOffset(int32_t val);

    //设置行ID
    void setItemNum(const QString &val);

    //设置名字
    void setName(const QByteArray &sz);

    //设置BIT数，例如 "8+7..4" == 表示第一字节与第二字节高4bit
    void setBitSet(const QString &sz);

    //设置数据类型
    void setMetaDataType(MetaDataType name);

    //设置数据值类型
    void setMetaValueType(MetaValueType val);

    //设置单位
    void setUnits(const QString &name);

    //设置分辨率
    void setResolution(double resolution);

    //设置转换后数据类型
    void setDestDataType(DestDataType val);

    //设置数据值范围
    void parseRange(const QString &);
    void parseRange(const QString &, QMap<int, QString> &values);
    void parseRangeExt(const QString &, QMap<int, QString> &values);

    void parseColor(const QString &);
    void sortColorSet(QVector<ColorSet> &, ColorSet &&value);

    int rangeValue(double &low, double &high) const;
    double rateValue(double value) const;

    void setColor(uint32_t col) { _color |= col; }

    MetaValueType metaValueType(void) const;
    MetaDataType metaDataType() const;
    QString place(void) const;

    QVector<ColorSet>& ranges();

    QStringList enumStr() const;
    QString name(void) const;
    QString units(void) const;
    QString range(void) const;
    QString getBitSet() const;
    int8_t byteCount() const;
    int16_t itemNum() const;
    int32_t offset() const;
    int32_t acolor() const;
    QString enumStrById(int idx) const;
    QString enumStrByKey(int idx) const;
    int enumValueBy(const QString &) const;
    int enumValueCheck(int idx) const;
    int enumValue(int idx) const;

    double protoValue() const;
    double resolution() const;
    bool isString(void) const;
    bool isEnum(void) const;

    // 用户输入到协议数据
    uint32_t value(const QString &_rawValue) const;

    // 遥测数据到显示
    bool transferValue(const QString &_rawValue, char *_display, int len);
    bool transferValue(uint32_t _rawValue, char *_display, int len);

    template <class Type>
    Type rangeClamp(Type val) {
        if (_rangeSet.size() <= 1) {
            return val;
        }
        if (val > _rangeSet.at(1)) {
            return _rangeSet.at(1);
        }
        if (val < _rangeSet.at(0)) {
            return _rangeSet.at(0);
        }
        return val;
    }

    template <class Type>
    void colorCalculate(Type value) {
        bool setcolr = false;
        for (int im = 0; im < _colorSet.size(); im++) {
            if (_colorSet[im].type > 0) {
                //（）
                if ((_colorSet[im].type & 0x10) && (_colorSet[im].type & 0x01)){
                    if ((value > (Type) _colorSet[im].ivalue) &&
                        (value < (Type) _colorSet[im].value)) {
                        _color = _colorSet[im].color;
                        setcolr = true;
                        break;
                    }
                }
                // (]
                else if ((_colorSet[im].type & 0x10) && !(_colorSet[im].type & 0x01)){
                    if ((value > (Type) _colorSet[im].ivalue) &&
                        (value <= (Type) _colorSet[im].value)) {
                        _color = _colorSet[im].color;
                        setcolr = true;
                        break;
                    }
                }
                // [)
                else if (!(_colorSet[im].type & 0x10) && (_colorSet[im].type & 0x01)){
                    if ((value >= (Type) _colorSet[im].ivalue) &&
                        (value < (Type) _colorSet[im].value)) {
                        _color = _colorSet[im].color;
                        setcolr = true;
                        break;
                    }
                }
            } else {
                if ((value >= (Type) _colorSet[im].ivalue) &&
                    (value <= (Type) _colorSet[im].value)) {
                    _color = _colorSet[im].color;
                    setcolr = true;
                    break;
                }
                if ((abs(_resolution) <= 0.2) && (value >= (Type) _colorSet[im].ivalue) &&
                    (value <= (Type) (_colorSet[im].value + _resolution))) {
                    _color = _colorSet[im].color;
                    setcolr = true;
                    break;
                }
            }
        }
        if (!setcolr) {
            _color = 0;
        }
    }

  private:
    void initialize();

  private:
    bool _hasinit = false;
    int8_t _byteCount = 0;
    int32_t _dataOffset = 0;
    int16_t _itemNum = 0;
    int32_t _color = 0;
    double _resolution = 1;

    QString _bitset = 0;
    QString _units = "";
    QString _range = "";
    std::string _formats = "";
    QByteArray _name;

    QVector<ColorSet> _colorSet;
    MetaDataType _mdatatype = undefineType;
    DestDataType _transType = translatorNull;
    MetaValueType _valuetype = valueTypeUndef;
    QList<double> _rangeSet;
    QStringList _listStrData;
    QMap<int, QString> _enumValStr;
};

Q_DECLARE_METATYPE(TypeMetaData)
