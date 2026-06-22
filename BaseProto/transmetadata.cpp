#include "transmetadata.h"
#include "log_category.h"
#include <QRegularExpression>

static QRegularExpression splitExpress("[|]");
static QRegularExpression splitExpress2("[&^|+]");
static QRegularExpression mLineSplitExpress(("[;\\n\\r]"));
static QRegularExpression gValSplitExpress(QStringLiteral("[=:]"));

int64_t currentSysTimeMs() {
    typedef std::chrono::milliseconds ms_precision;
    typedef std::chrono::time_point<std::chrono::system_clock, ms_precision> timeprecision;
    timeprecision tp = std::chrono::time_point_cast<ms_precision>(std::chrono::system_clock::now());
    return tp.time_since_epoch().count();
}

static int valueHexOrDex(const QString &value) {
    if (value.contains("0x") || value.contains("0X")) {
        bool ok = false;
        return value.toInt(&ok, 16);
    } else {
        return value.toInt();
    }
}

void ReferenceValueCmp::SimpleRefer(const QString &val, int ix) {
    QStringList valueList = val.split(gValSplitExpress, Qt::SkipEmptyParts);
    if (valueList.length() >= 2) {
        QString refFrame = valueList.at(0);
        int st = refFrame.indexOf("{");
        int et = refFrame.indexOf("}");
        if (st >= 0 && et >= st) {
            QString frame = refFrame.mid(st + 1, et - st - 1);
            QString mx = refFrame.mid(0, st);
            _ref[ix].item = valueHexOrDex(frame);
            _ref[ix].frame = valueHexOrDex(mx);
        }
        int lb = refFrame.indexOf("[");
        int rb = refFrame.indexOf("]");
        if (lb >= 0 && rb >= lb) {
            QString frame = refFrame.mid(lb + 1, rb - lb - 1);
            QString mx = refFrame.mid(0, lb);
            _ref[ix].item = valueHexOrDex(frame);
            _ref[ix].frame = valueHexOrDex(mx);
        }
        _ref[ix].expect = valueHexOrDex(valueList.at(1));
    }
}

void ReferenceValueCmp::OrRefer(const QString &val) {
    int ix = 0;
    int left = val.indexOf("|");

    QStringList lineSet;
    lineSet.append(val.mid(0, left));
    lineSet.append(val.mid(left + 1));

    foreach (QString var1, lineSet) {
        SimpleRefer(var1, ix);
        ++ix;
    }

    _operate = OrOperate;
}

void ReferenceValueCmp::AndRefer(const QString &val) {
    int ix = 0;
    int left = val.indexOf("&");

    QStringList lineSet;
    lineSet.append(val.mid(0, left));
    lineSet.append(val.mid(left + 1));

    foreach (QString var1, lineSet) {
        SimpleRefer(var1, ix);
        ++ix;
    }

    _operate = AndOperate;
}

void ReferenceValueCmp::parseRefer(const QString &refValue) {
    int ix = 0;
    QStringList sets = refValue.split(mLineSplitExpress, Qt::SkipEmptyParts);
    foreach (QString var, sets) {
        if (var.indexOf("&") > 0) {
            AndRefer(var);
        } else if (var.indexOf("|") > 0) {
            AndRefer(var);
        } else {
            SimpleRefer(var, 0); //
            _operate = InvalidOperate;
        }
    }
}

void FactMetaData::parseRefer(const QString &refValue) {
    if (!refValue.isEmpty()) {
        _refer.parseRefer(refValue);
        _refer.enableRefer();
    }
}

void WarnConfigGroupVal::resetWarnColor(int timeout) {
    foreach (auto var, groupElems) {
        // reset warn node color
        var->resetWarnColor(timeout);
    }
}

WarnConfigGroupKey::WarnConfigGroupKey() {
    memset(&_referItem, 0, sizeof(_referItem));
    memset(&_referValue, 0, sizeof(_referValue));
    memset(&_referFrame, 0, sizeof(_referFrame));
}

void WarnConfigGroupVal::addNode(std::shared_ptr<WarnConfigExt> node) {
    groupElems.append(node);
    allDataModel.append(node.get());
}

std::shared_ptr<WarnConfigExt> WarnConfigGroupVal::findBySeq(uint32_t seq) {
    foreach (auto var, groupElems) {
        if (var->_sequence == seq) {
            return var;
        }
    }
    return nullptr;
}

void WarnConfigExt::resetWarnColor(int timeout) {
    if (_warnTimeout > 0 && currentSysTimeMs() > _warnTimeout + timeout) {
        emit warnColorChanged(0);
        _warnColor = 0;
    }
}

std::shared_ptr<WarnConfNode> WarnConfigExt::findByValue(int value) {
    std::shared_ptr<WarnConfNode> rtn = nullptr;
    if (warns.find(value) != warns.end()) {
        if ((rtn = warns.find(value).value()) != nullptr) {
            _warnTimeout = currentSysTimeMs();
            if (_warnColor != rtn->color) {
                _warnColor = rtn->color;
                emit warnColorChanged(_warnColor);
            }
        }
    }
    return rtn;
}

FactMetaData::FactMetaData(const FactMetaData &other) {
    _frameName = other._frameName;
    _groupName = other._groupName;
    _selected = other._selected;
    datas[0] = other.datas[0];
    datas[1] = other.datas[1];
    datas[2] = other.datas[2];
    _metaData = other._metaData;
}

FactMetaData &FactMetaData::operator=(const FactMetaData &other) {
    if (&other == this) {
        return *this;
    }

    _selected = other._selected;
    _frameName = other._frameName;
    _groupName = other._groupName;
    datas[0] = other.datas[0];
    datas[1] = other.datas[1];
    datas[2] = other.datas[2];
    _metaData = other._metaData;

    return *this;
}

QString FactMetaData::vert3String(const QString &value) {
    QString vertName = value;
    int off = value.length() % 3 + 1;
    for (int ia = vertName.length() - off; ia > 0; ia -= 3) {
        if (vertName.at(ia) > 256) {
            vertName.insert(ia, "\n");
        }
    }
    return vertName;
}

QString FactMetaData::vert2String(const QString &value) {
    QString vertName = value;
    for (int ia = vertName.length() - 2; ia > 0; ia -= 2) {
        if (vertName.at(ia) > 256) {
            vertName.insert(ia, "\n");
        }
    }
    return vertName;
}

QString FactMetaData::vertString(const QString &value) {
    QString vertName = value;

    int ia = 0;
    int split = -1;

    for (ia = 1; ia < vertName.length(); ia++) {
        if ((vertName.at(ia - 1) > 256 && vertName.at(ia) < 256) ||
            (vertName.at(ia - 1) < 256 && vertName.at(ia) > 256)) {
            vertName.insert(ia, "\n");
            split = ia;
            break;
        }
    }

    for (ia = vertName.length() - 1; ia > split + 1; ia--) {
        if (vertName.at(ia) > 256) {
            vertName.insert(ia, "\n");
        }
    }

    for (ia = split - 1; ia > 0; ia--) {
        if (vertName.at(ia) > 256) {
            vertName.insert(ia, "\n");
        }
    }

    return vertName;
}

bool FactMetaData::isUpdated(int chl) {
    if (datas[chl]._oldValue != datas[chl]._rawValue) {
        datas[chl]._oldValue = datas[chl]._rawValue;
        return true;
    }
    return false;
}

bool FactMetaData::isWarnUpdated(int chl) {
    if (isUpdated(chl)) {
        return true;
    }

    uint64_t timems = currentSysTimeMs();
    if (timems > updateTm) { // checking
        updateTm = timems + 2000;
        return true;
    }

    return false;
}

void FactMetaData::notifyDisplay(int chl) {
    emit displayChanged(QString(datas[chl]._display), _metaData.acolor());
}

QString FactMetaData::formatTimestamp(int chl, int select, bool bigmode) {
    int value = datas[chl]._rawValue;
    if (bigmode) {
        int a = (value >> 16) & 0xff;
        int b = (value >> 8) & 0xff;
        int c = (value >> 0) & 0xff;
        value = (c << 16) | (b << 8) | a;
    }

    int hor = (value >> 19) & 0x1f;
    int minu = (value >> 13) & 0x3f;
    int sec = (value) &0x1fff;

    sprintf(datas[chl]._display, "%02d:%02d:%02d:%02d", hor, minu, sec / 100, (sec % 100) * 10);

    if (chl == select) {
        emit displayChanged(QString(datas[chl]._display), 0);
    }

    return QString(datas[chl]._display);
}

QString FactMetaData::hexValue(int chl) const {
    uint value = datas[chl]._rawValue;
    return (QString("0x%1").arg(value, 8, 16, QLatin1Char('0'))).toUpper();
}

uint32_t WarnConfig::decoder(const QString &valString) {
    _errString = valString;

    QStringList sets = _errString.split(mLineSplitExpress, Qt::SkipEmptyParts);
    foreach (QString var, sets) {
        QStringList lineSet = var.split(gValSplitExpress, Qt::SkipEmptyParts);
        if (lineSet.size() < 2) {
            continue;
        }

        int errlines = 0;
        QString warnStr = lineSet[0];
        if (warnStr.contains(QStringLiteral("1级")) || warnStr.contains(QStringLiteral("警告"))) {
            _errorRow |= (1 << LevelOne);
            errlines = LevelOne;
        } else if (warnStr.contains(QStringLiteral("2级")) ||
                   warnStr.contains(QStringLiteral("注意"))) {
            _errorRow |= (1 << LevelTwo);
            errlines = LevelTwo;
        } else if (warnStr.contains(QStringLiteral("3级")) ||
                   warnStr.contains(QStringLiteral("提示"))) {
            _errorRow |= (1 << LevelThree);
            errlines = LevelThree;
        } else if (warnStr.contains(QStringLiteral("4级")) ||
                   warnStr.contains(QStringLiteral("维护"))) {
            _errorRow |= (1 << LevelFour);
            errlines = LevelFour;
        }

        // 1级故障=$*=具体告警字符
        QString valueStr = lineSet[1];
        if (valueStr.contains(QStringLiteral("$"))) {
            int right = valueStr.lastIndexOf(QStringLiteral("$"));
            if (right >= 0) {
                _checkCount[errlines] += 1;
                _moreOrLess = ColorValue;
                _priorityValue[errlines] = valueStr.midRef(right + 1).toInt();
                if (lineSet.size() > 2) {
                    _warnString[errlines] = lineSet[2];
                }
            }
            continue;
        }

        // 2级故障=0x00
        // 1级故障=0x0|0x1
        // 1级故障=<0x2
        // 1级故障=>0x1
        if (valueStr.contains("<")) {
            _moreOrLess = LessEqual;
            valueStr = valueStr.mid(1);
        } else if (valueStr.contains(">")) {
            _moreOrLess = GreateEqual;
            valueStr = valueStr.mid(1);
        } else {
            _moreOrLess = EqualValue;
        }
        bool afterWarnStr = false;
        QStringList valueList = valueStr.split(splitExpress, Qt::SkipEmptyParts);
        foreach (QString var, valueList) {
            if (var.contains("0x") || var.contains("0X")) {
                bool ok = false;
                int value = var.trimmed().toInt(&ok, 16);
                _checkValue[errlines] <<= 8;
                _checkValue[errlines] |= value;
            } else {
                int value = var.trimmed().toInt();
                _checkValue[errlines] <<= 8;
                _checkValue[errlines] |= value;
            }
            _checkCount[errlines] += 1;
        }

        // 1级故障=>0x1=0+24
        // 1级故障=0x1=0&24
        // 2级故障=0x1=0^24
        // 1级故障=0x0={0x30[18]0x1}

        if (lineSet.size() > 2) {
            QString valueStr2 = lineSet[2];
            QStringList valueList = valueStr2.split(splitExpress2, Qt::SkipEmptyParts);
            if (valueStr2.contains("{")) {
                int st = valueStr2.indexOf("{");
                int et = valueStr2.indexOf("}");
                valueStr2 = valueStr2.mid(st + 1, et - st - 1);
                int lb = valueStr2.indexOf("[");
                int rb = valueStr2.indexOf("]");

                bool ok = false;
                QString mx = valueStr2.mid(0, lb);
                int value = mx.trimmed().toInt(&ok, 16);
                _referFrame = value << 16;
                mx = valueStr2.mid(lb + 1, rb - lb - 1);
                _referFrame |= (mx.toInt() << 8);
                mx = valueStr2.mid(rb + 1);
                value = mx.trimmed().toInt(&ok, 16);
                _referFrame |= value;
            } else {
                foreach (QString var, valueList) {
                    if (var.contains("0x") || var.contains("0X")) {
                        bool ok = false;
                        int value = var.trimmed().toInt(&ok, 16);
                        _referRow[errlines] <<= 8;
                        _referRow[errlines] |= value;
                    } else {
                        int value = var.trimmed().toInt();
                        _referRow[errlines] <<= 8;
                        _referRow[errlines] |= value;
                    }
                }
                if (valueStr2.contains("&")) {
                    _referOpt[errlines] = AndOperate;
                } else if (valueStr2.contains("^")) {
                    _referOpt[errlines] = NotOperate;
                } else if (valueStr2.contains("+")) {
                    _referOpt[errlines] = AddOperate;
                } else if (valueStr2.contains("|")) {
                    _referOpt[errlines] = EachOperate;
                }
            }
        }
    }

    return _referFrame;
}

void FactMetaData::enumDefValue() {
    QStringList vm = enumValStr();
    QString val = "0";
    if (vm.length() > 0) {
        val = QString("%1").arg(enumValue(vm[vm.length() - 1]));
    }
    setRawValue(val);
}

void FactMetaData::setRawValue(QString value) {
    _rawInValue = value;
    emit rawValueChanged();
}

void FactMetaData::manualUpdateData(int chl) {
    emit displayChanged(QString(datas[chl]._display), acolor());
}

void FactMetaData::notifyRawDisplay() { _rawNofify = true; }

void FactMetaData::setRawDisplayValue(int chl, int value) {
    datas[chl]._rawValue = value;
    _metaData.transferValue(datas[chl]._rawValue, datas[chl]._display, sizeof(datas[chl]._display));
}

void FactMetaData::parseRangeExt(const QString &val, QMap<int, QString> &values){
    _metaData.parseRangeExt(val, values);
}

void FactMetaData::parseRange(const QString &val, QMap<int, QString> &values) {
    _metaData.parseRange(val, values);
}

QString FactMetaData::place(void) const { return _metaData.place(); }

double FactMetaData::rateValue(double value) const { return _metaData.rateValue(value); }

QString FactMetaData::getBitSet() const { return _metaData.getBitSet(); }

int8_t FactMetaData::byteCount() const { return _metaData.byteCount(); }

double FactMetaData::resolution() const { return _metaData.resolution(); }

MetaDataType FactMetaData::metaDataType() const { return _metaData.metaDataType(); }

MetaValueType FactMetaData::metaValueType() const { return _metaData.metaValueType(); }

uint32_t FactMetaData::value() const { return _metaData.value(_rawInValue); }

bool FactMetaData::isString(void) const { return _metaData.isString(); }

bool FactMetaData::isStruct(void) const {
    return metaValueType() == valueTypeStruct ? true : false;
}

bool FactMetaData::isEnum(void) const { return _metaData.isEnum(); }

uint32_t FactMetaData::rawValue(int chl) const { return datas[chl]._rawValue; }

QString FactMetaData::rawInValue(void) const { return _rawInValue; }

int FactMetaData::enumValue(const QString &val) const { return _metaData.enumValueBy(val); }

int FactMetaData::enumValue(int idx) const { return _metaData.enumValue(idx); }

int FactMetaData::enumValueCheck(int idx) const { return _metaData.enumValueCheck(idx); }

QString FactMetaData::enumStrById(int idx) const { return _metaData.enumStrById(idx); }

QString FactMetaData::enumStrByKey(int idx) const { return _metaData.enumStrByKey(idx); }

QStringList FactMetaData::enumValStr(void) const { return _metaData.enumStr(); }

int16_t FactMetaData::itemNum() const { return _metaData.itemNum(); }

QString FactMetaData::name(void) const { return _metaData.name(); }

QString FactMetaData::display(int chl) const { return QString(datas[chl]._display); }

QString FactMetaData::units(void) const { return _metaData.units(); }
