#include "basemetadata.h"
#include "log_category.h"
#include <QRegularExpression>

TypeMetaData::TypeMetaData() { _resolution = 1; }

TypeMetaData &TypeMetaData::operator=(const TypeMetaData &other) {

    if (&other == this) {
        return *this;
    }

    _itemNum = other._itemNum;
    _byteCount = other._byteCount;
    _dataOffset = other._dataOffset;
    _resolution = other._resolution;
    _mdatatype = other._mdatatype;
    _transType = other._transType;
    _valuetype = other._valuetype;
    _formats = other._formats;
    _bitset = other._bitset;
    _units = other._units;
    _name = other._name;

    for (int x = 0; x < other._listStrData.size(); x++) {
        _listStrData.push_back(other._listStrData[x]);
    }

    for (int x2 = 0; x2 < other._rangeSet.size(); x2++) {
        _rangeSet.push_back(other._rangeSet[x2]);
    }

    for (auto x23 = other._enumValStr.begin(); x23 != other._enumValStr.end(); x23++) {
        _enumValStr.insert(x23.key(), x23.value());
    }

    return *this;
}

TypeMetaData::TypeMetaData(const TypeMetaData &other) {

    _itemNum = other._itemNum;
    _byteCount = other._byteCount;
    _dataOffset = other._dataOffset;
    _resolution = other._resolution;
    _mdatatype = other._mdatatype;
    _transType = other._transType;
    _valuetype = other._valuetype;
    _formats = other._formats;
    _bitset = other._bitset;
    _units = other._units;
    _name = other._name;

    for (int x = 0; x < other._listStrData.size(); x++) {
        _listStrData.push_back(other._listStrData[x]);
    }

    for (int x2 = 0; x2 < other._rangeSet.size(); x2++) {
        _rangeSet.push_back(other._rangeSet[x2]);
    }

    for (auto x23 = other._enumValStr.begin(); x23 != other._enumValStr.end(); x23++) {
        _enumValStr.insert(x23.key(), x23.value());
    }
}

int TypeMetaData::rangeValue(double &low, double &high) const {
    if (_rangeSet.size() <= 1) {
        return _rangeSet.size();
    }

    high = _rangeSet.at(1);
    low = _rangeSet.at(0);
    return _rangeSet.size();
}

double TypeMetaData::rateValue(double value) const {
    if (_rangeSet.size() <= 1) {
        return 1;
    }
    double val1 = _rangeSet.at(1);
    double val0 = _rangeSet.at(0);
    if (_rangeSet.size() >= 2 && val1 > val0) {
        return double(value - val0) / (val1 - val0);
    }
    return 1;
}

QVector<ColorSet>& TypeMetaData::ranges(){
    return _colorSet;
}

QString TypeMetaData::place(void) const {
    if (_rangeSet.size() >= 2) {
        return QString("%1~%2").arg(_rangeSet.at(0)).arg(_rangeSet.at(1));
    }
    return QString("");
}

QString TypeMetaData::range(void) const { return _range; }
QString TypeMetaData::getBitSet() const { return _bitset; }
int8_t TypeMetaData::byteCount() const { return _byteCount; }
int16_t TypeMetaData::itemNum() const { return _itemNum; }
int32_t TypeMetaData::offset() const { return _dataOffset; }
int32_t TypeMetaData::acolor() const { return _color; }
QString TypeMetaData::units(void) const { return _units; }
QString TypeMetaData::name(void) const { return QString::fromUtf8(_name); }
double TypeMetaData::resolution() const { return _resolution; }
bool TypeMetaData::isString(void) const { return _valuetype == valueTypeString; }
bool TypeMetaData::isEnum(void) const { return _valuetype == valueTypeEnum; }
QStringList TypeMetaData::enumStr() const { return _listStrData; }
MetaDataType TypeMetaData::metaDataType() const { return _mdatatype; }
MetaValueType TypeMetaData::metaValueType() const { return _valuetype; }

void TypeMetaData::setDestDataType(DestDataType val) { _transType = val; }
void TypeMetaData::setOffset(int32_t val) { _dataOffset = val; }
void TypeMetaData::setName(const QByteArray &nm) { _name = nm; }

void TypeMetaData::setItemNum(const QString &val) {
    if (val.contains("0x") || val.contains("0X")) {
        bool ok = false;
        _itemNum = val.trimmed().toInt(&ok, 16);
    } else {
        _itemNum = val.toInt();
    }
}

void TypeMetaData::setMetaDataType(MetaDataType group) { _mdatatype = group; }
void TypeMetaData::setMetaValueType(MetaValueType val) { _valuetype = val; }
void TypeMetaData::setUnits(const QString &units) { _units = " " + units; }
void TypeMetaData::setResolution(double resolution) { _resolution = resolution; }
void TypeMetaData::setBitSet(const QString &sz) { _bitset = sz; }
void TypeMetaData::setByteCount(int8_t val) { _byteCount = val; }
void TypeMetaData::setRange(const QString &sz) { _range = sz; }

QString TypeMetaData::enumStrByKey(int idx) const {
    auto key = _enumValStr.begin();
    for (; key != _enumValStr.end(); ++key) {
        if (idx == key.key()) {
            return key.value();
        }
    }

    return QString();
}

QString TypeMetaData::enumStrById(int idx) const {
    if (idx >= 0 && _listStrData.count() > 0) {
        idx = idx % _listStrData.count();
        return _listStrData.at(idx);
    }
    return QString();
}

int TypeMetaData::enumValueBy(const QString &val) const {
    auto key = _enumValStr.begin();
    for (; key != _enumValStr.end(); ++key) {
        if (val.contains(key.value())) {
            return key.key();
        }
    }
    return -1;
}

int TypeMetaData::enumValueCheck(int idx) const {
    auto key = _enumValStr.begin();
    for (; key != _enumValStr.end(); ++key) {
        if (idx == key.key()) {
            return key.key();
        }
    }
    return -1;
}

int TypeMetaData::enumValue(int idx) const {
    if (_listStrData.count() > 0 && idx >= 0) {
        idx = idx % _listStrData.count();
        QString value = _listStrData.at(idx);
        auto key = _enumValStr.begin();
        for (; key != _enumValStr.end(); ++key) {
            if (value.contains(key.value())) {
                return key.key();
            }
        }
    }

    return -1;
}

uint32_t TypeMetaData::value(const QString &_rawValue) const {
    double value = _rawValue.toDouble();

    /// when an integral data is parsed as double,
    /// the resolution of data is changed.
    if (fabs(_resolution) < 0.5f) {
        double val = fabs(_resolution / 2) * 1e-6;
        value += (value > 0 ? val : -val);
    }

    if (_rangeSet.size() > 1) {
        value = value < _rangeSet.at(0) ? _rangeSet.at(0) : value;
        value = value > _rangeSet.at(1) ? _rangeSet.at(1) : value;
    }

    switch (_valuetype) {
    case valueTypeUint8:
        return (uint8_t)((value - _dataOffset) / _resolution);
    case valueTypeInt8:
        return (uint8_t)((value - _dataOffset) / _resolution);
    case valueTypeUint16:
        return (uint16_t)((value - _dataOffset) / _resolution);
    case valueTypeInt16:
        return (uint16_t)((value - _dataOffset) / _resolution);
    case valueTypeUint32:
        return (uint32_t)((value - _dataOffset) / _resolution);
    case valueTypeInt32:
        return (uint32_t)((value - _dataOffset) / _resolution);
    default:
        return uint32_t(value);
    }
}

bool TypeMetaData::transferValue(uint32_t _rawValue, char *_display, int len) {
    if (!_hasinit) {
        _hasinit = true;
        initialize();
    }

    if (_transType == translatorInt32) {
        if ((_mdatatype == signedType) || (_dataOffset < 0)) {
            int32_t value = ((int32_t) _rawValue) * _resolution;
            value += _dataOffset;
            snprintf(_display, len, _formats.c_str(), value);
            colorCalculate(value);
        } else {
            uint32_t value = _rawValue * _resolution;
            value += _dataOffset;
            snprintf(_display, len, _formats.c_str(), value);
            colorCalculate(value);
        }

    } else if (_transType == translatorDouble) {
        if (_mdatatype == signedType) {
            double value = ((int32_t) _rawValue) * _resolution;
            value += _dataOffset;
            snprintf(_display, len, _formats.c_str(), value);
            colorCalculate(value);
        } else {
            double value = _rawValue * _resolution;
            value += _dataOffset;
            snprintf(_display, len, _formats.c_str(), value);
            colorCalculate(value);
        }

    } else if (_transType == translatorEnums) {
        std::string detail = _enumValStr[_rawValue].toStdString();
        snprintf(_display, len, _formats.c_str(), detail.c_str());
        colorCalculate(_rawValue);

    } else if (_transType == translatorStruct) {
        uint32_t value = _rawValue * _resolution;
        snprintf(_display, len, _formats.c_str(), value + _dataOffset);

    } else {
        snprintf(_display, len, "%d", _rawValue);
    }
    return true;
}

bool TypeMetaData::transferValue(const QString &_rawValue, char *_display, int len) {

    if (!_hasinit) {
        _hasinit = true;
        initialize();
    }

    if (_transType == translatorInt32) {
        if (_mdatatype == signedType) {
            int32_t value = _rawValue.toUInt() * _resolution;
            snprintf(_display, len, _formats.c_str(), value + _dataOffset);
        } else {
            uint32_t value = _rawValue.toUInt() * _resolution;
            snprintf(_display, len, _formats.c_str(), value + _dataOffset);
        }
    } else if (_transType == translatorDouble) {
        if (_mdatatype == signedType) {
            double value = _rawValue.toInt() * _resolution;
            double val = value + _dataOffset;
            snprintf(_display, len, _formats.c_str(), val);
        } else {
            double value = _rawValue.toUInt() * _resolution;
            double val = value + _dataOffset;
            snprintf(_display, len, _formats.c_str(), val);
        }
    } else if (_transType == translatorEnums) {
        std::string detail = _enumValStr[_rawValue.toUInt()].toStdString();
        snprintf(_display, len, _formats.c_str(), detail.c_str());
    } else if (_transType == translatorStruct) {
        uint32_t value = _rawValue.toUInt() * _resolution;
        snprintf(_display, len, _formats.c_str(), value + _dataOffset);
    } else {
        std::string detail = _rawValue.toStdString();
        snprintf(_display, len, _formats.c_str(), detail.c_str());
    }
    return true;
}

void TypeMetaData::initialize() {
    if (_transType == translatorInt32) {
        _formats = "%d";
    } else if (_transType == translatorDouble) {
        if (_resolution >= 0.1 && _resolution < 1) {
            _formats = "%.1f";
        } else if (_resolution >= 0.01 && _resolution < 0.1) {
            _formats = "%.2f";
        } else if (_resolution >= 0.001 && _resolution < 0.01) {
            _formats = "%.3f";
        } else if (_resolution >= 0.0000001 && _resolution < 0.001) {
            _formats = "%.7f";
        } else {
            _formats = "%.2f";
        }
    } else if (_transType == translatorString) {
        _formats = "%s";
    } else if (_transType == translatorStruct) {
        _formats = "%d";
    } else {
        _formats = "%s";
    }
}

void TypeMetaData::sortColorSet(QVector<ColorSet> &sets, ColorSet &&value) {
    int ix = 0;
    for (ix = 0; ix < sets.length(); ix++) {
        if (sets[ix].ivalue > value.ivalue) {
            break;
        }
    }
    if (ix >= 0 && ix < sets.length()) {
        sets.insert(ix, value);
    } else {
        sets.append(value);
    }
}

void TypeMetaData::parseColor(const QString &invalue) {
    if (invalue.isEmpty()) {
        return;
    }
    static QRegularExpression regexBinStr(R"(^[01]+$)");
    static QRegularExpression mLineSplitExpress("[\\n;]");
    static QRegularExpression gValSplitExpress("[=:]");
    static QRegularExpression gValSplitExpress2("[|]");
    static QRegularExpression gValSplitExpress3("[,~]");
    QStringList sets = invalue.split(mLineSplitExpress);
    foreach (QString var, sets) {
        QStringList em = var.split(gValSplitExpress);
        int32_t defColor = GreenColor;
        if (em[0].contains("Red")) {
            defColor = RedColor;
        } else if (em[0].contains("Orange")) {
            defColor = OrangeColor;
        } else if (em[0].contains("Yellow")) {
            defColor = YellowColor;
        } else if (em[0].contains("Green")) {
            defColor = GreenColor;
        } else if (em[0].contains("White")) {
            defColor = WhiteColor;
        }

        if (em.size() >= 2) {
            QStringList emv = em[1].split(gValSplitExpress2);
            foreach (QString vars, emv) {
                QString value = vars;

                int16_t typeLR = 0;
                int last = value.lastIndexOf("]");
                if (last < 0) {
                    last = value.lastIndexOf(")");
                    if (last > 0) {
                        typeLR |= 0x01;
                    }
                }

                int frist = value.lastIndexOf("[");
                if (frist < 0) {
                    frist = value.lastIndexOf("(");
                    if (frist >= 0) {
                        typeLR |= 0x10;
                    }
                }

                if (last > 0 && frist >= 0) {
                    QString tmValue = value.mid(frist + 1, last - frist - 1);
                    QStringList emv = tmValue.split(gValSplitExpress3);
                    if (emv.size() >= 2) {
                        sortColorSet(_colorSet, ColorSet{emv[0].toFloat(), emv[1].toFloat(), defColor, typeLR});
                    } else {
                        COMMON_INFO("unable to parse input param: {}", invalue.toStdString().c_str());
                    }
                } else {
                    int code = 0;
                    bool ok = false;
                    if (value.contains("0x") || value.contains("0X")) {
                        code = value.trimmed().toInt(&ok, 16);
                        sortColorSet(_colorSet, ColorSet{(float) code, (float) code, defColor, 0});
                    } else {
                        QString val = value.trimmed();
                        if (regexBinStr.match(val).hasMatch()) {
                            QByteArray binStr(val.toUtf8());
                            code = binStr.toInt(&ok, 2);
                        } else {
                            code = val.toInt(&ok, 2);
                        }
                        sortColorSet(_colorSet, ColorSet{(float) code, (float) code, defColor, 0});
                    }
                }
            }
        }
    }
}

void TypeMetaData::parseRangeExt(const QString &value, QMap<int, QString> &valueRange){
    static QRegularExpression splitExpressA("[~]");
    static QRegularExpression splitExpressB("[,]");
    QStringList sets = value.split(splitExpressA);
    if (sets.size() > 1) {
        int st = sets[0].toInt();
        int et = sets[1].toInt();
        for (int var = st; var <= et; ++var) {
            auto mx = valueRange.find(var);
            if (mx != valueRange.end()) {
                _enumValStr[var] = mx.value();
                _listStrData.push_back(mx.value());
            }
        }
    } else {
        sets = value.split(splitExpressB);
        if (sets.size() >= 1) {
            foreach (QString var, sets) {
                auto mx = valueRange.find(var.toInt());
                if (mx != valueRange.end()) {
                    _enumValStr[var.toInt()] = mx.value();
                    _listStrData.push_back(mx.value());
                }
            }
        }
    }

    if (value.compare("All", Qt::CaseInsensitive) == 0) {
        auto mx = valueRange.begin();
        while (mx != valueRange.end()) {
            _enumValStr[mx.key()] = mx.value();
            _listStrData.push_back(mx.value());
            ++mx;
        }
    }
}

void TypeMetaData::parseRange(const QString &value, QMap<int, QString> &valueRange) {
    static QRegularExpression splitExpressA("[~]");
    static QRegularExpression splitExpressB("[,]");
    QStringList sets = value.split(splitExpressA);
    if (sets.size() > 1) {
        int st = sets[0].toInt();
        int et = sets[1].toInt();
        for (int var = st; var <= et; ++var) {
            auto mx = valueRange.find(var);
            if (mx != valueRange.end()) {
                _enumValStr[var] = mx.value();
            }
        }
    } else {
        sets = value.split(splitExpressB);
        if (sets.size() >= 1) {
            foreach (QString var, sets) {
                auto mx = valueRange.find(var.toInt());
                if (mx != valueRange.end()) {
                    _enumValStr[var.toInt()] = mx.value();
                }
            }
        }
    }

    if (value.compare("All", Qt::CaseInsensitive) == 0) {
        auto mx = valueRange.begin();
        while (mx != valueRange.end()) {
            _enumValStr[mx.key()] = mx.value();
            ++mx;
        }
    }

    auto key = _enumValStr.begin();
    for (; key != _enumValStr.end(); ++key) {
        _listStrData.push_back(key.value());
    }
}

void TypeMetaData::parseRange(const QString &value) {
    static QRegularExpression regexBinStr(R"(^[01]+$)");
    static QRegularExpression splitExpress1("[~]");
    static QRegularExpression splitExpress2(QString::fromStdWString(L"[～]"));
    static QString datasplit = QStringLiteral("±");
    if (value.isEmpty()) {
        return;
    }

    _rangeSet.clear();

    static QRegularExpression mLineSplitExpress(QStringLiteral("[\\n,;；]"));
    static QRegularExpression gValSplitExpress(QStringLiteral("[-=:：]"));
    if (value.contains(QStringLiteral("~"))) {
        QStringList set = value.split(splitExpress1);
        assert(set.size() > 1);
        _rangeSet.append((set[0].toDouble()));
        _rangeSet.append((set[1].toDouble()));
    } else if (value.contains(QString::fromStdWString(L"～"))) {
        QStringList set = value.split(splitExpress2);
        assert(set.size() > 1);
        _rangeSet.append((set[0].toDouble()));
        _rangeSet.append((set[1].toDouble()));
    } else if (value.contains(datasplit)) {
        QString curValue(value);
        QString last = curValue.replace(datasplit, QStringLiteral(" "));
        _rangeSet.append(-last.toDouble());
        _rangeSet.append(last.toDouble());
    } else if (_mdatatype == enumType) {
        QStringList sets = value.split(mLineSplitExpress);
        if (sets.size() > 1) {
            foreach (QString var, sets) {
                QStringList em = var.split(gValSplitExpress);
                if (em.size() >= 2) {
                    if (em[0].contains("0x") || em[0].contains("0X")) {
                        bool ok = false;
                        int value = em[0].trimmed().toInt(&ok, 16);
                        _enumValStr[value] = em[1];
                    } else {
                        bool ok = false;
                        QString val = em[0].trimmed();
                        if (regexBinStr.match(val).hasMatch()) {
                            QByteArray binStr(val.toUtf8());
                            _enumValStr[binStr.toInt(&ok, 2)] = em[1];
                        } else {
                            _enumValStr[em[0].toInt()] = em[1];
                        }
                    }
                }
            }
        }
    } else if (_valuetype == valueTypeEnum) {
        QStringList sets = value.split(mLineSplitExpress);
        if (sets.size() >= 1) {
            foreach (QString var, sets) {
                QStringList em = var.split(gValSplitExpress);
                if (em.size() >= 2) {
                    if (em[0].contains("0x") || em[0].contains("0X")) {
                        bool ok = false;
                        int value = em[0].trimmed().toInt(&ok, 16);
                        _enumValStr[value] = em[1];
                    } else {
                        bool ok = false;
                        QString val = em[0].trimmed();
                        if (regexBinStr.match(val).hasMatch()) {
                            QByteArray binStr(val.toUtf8());
                            _enumValStr[binStr.toInt(&ok, 2)] = em[1];
                        } else {
                            _enumValStr[em[0].toInt()] = em[1];
                        }
                    }
                }
            }
        }
    } else {
        double val = value.toDouble();
        _rangeSet.append(val);
    }

    auto key = _enumValStr.begin();
    for (; key != _enumValStr.end(); ++key) {
        _listStrData.push_back(key.value());
    }
}
