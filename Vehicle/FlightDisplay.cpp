#include "FlightDisplay.h"
#include "log_category.h"
#include <QRegularExpression>
#include <QTextCodec>

static QRegularExpression gValSplitExpressAnd("[&]");
static QRegularExpression gValSplitExpressOr("[|]");
static QRegularExpression mLineSplitExpress("[\\n;]");
static QRegularExpression gValSplitExpress2("[\\{\\}]");
static QRegularExpression gValSplitExpress3("[:,\\[\\]\\<\\>\\-\\=]");
static QRegularExpression gValSplitExpress4("[\\=]");

static int colorMapName(const QString &name) {
    if (name.contains("Red")) {
        return RedColor;
    } else if (name.contains("Orange")) {
        return OrangeColor;
    } else if (name.contains("Yellow")) {
        return YellowColor;
    } else if (name.contains("Green")) {
        return GreenColor;
    } else if (name.contains("White")) {
        return WhiteColor;
    } else {
        return GreenColor;
    }
}

static int String2Integer(const QString &va) {
    if (va.contains("0x") || va.contains("0X")) {
        return va.toInt(nullptr, 16);
    }

    return va.toInt();
}

FlightDisplay::FlightDisplay() {
    _showColor = 0;
    _frameCode = 0;
    _showIndex = 0;
    _rateValue = 0;
    _valueStr = "NA";
}

bool FlightDisplay::stateValue(int chl) const {
    if (factdata != nullptr) {
        return factdata->rawValue(chl);
    }

    return true;
}

QString FlightDisplay::range() const {
    if (factdata != nullptr) {
        return factdata->place();
    }

    return QString();
}

QStringList FlightDisplay::enumValStr() const {
    if (factdata != nullptr) {
        return factdata->enumValStr();
    }
    return QStringList();
}

QString FlightDisplay::unit() const {
    if (factdata != nullptr) {
        return factdata->units();
    }
    return QString();
}

QString FlightDisplay::name() const {
    if (factdata != nullptr) {
        return factdata->name();
    }
    return QString();
}

void FlightDisplay::notifyRawDisplay() {
    if (factdata != nullptr) {
        factdata->notifyRawDisplay();
    }
}

void FlightDisplay::setShowIndex(const QString &va) {
    static QRegularExpression splitExpress(QStringLiteral("[-+/:]"));
    QStringList em = va.split(splitExpress);
    if (em.length() > 1) {
        if (va.contains("+")) {
            _operate = AddOperate;
        }
        if (va.contains("-")) {
            _operate = SubOperate;
        }
        int left = String2Integer(em[0]) << 8;
        left |= String2Integer(em[1]);
        left |= 0x80000000; //
        setShowIndex(left);
    } else {
        setShowIndex(String2Integer(va));
    }
}

void FlightDisplay::setFrameCode(const QString &va) {
    static QRegularExpression splitExpress(QStringLiteral("[-+/:]"));
    QStringList em = va.split(splitExpress);
    if (em.length() > 1) {
        if (va.contains("+")) {
            _operate = AddOperate;
        }
        if (va.contains("-")) {
            _operate = SubOperate;
        }
        int left = String2Integer(em[0]) << 16;
        left |= String2Integer(em[1]);
        setFrameCode(left);
    } else {
        setFrameCode(String2Integer(va));
    }
}

void FlightDisplay::insertFactElemExt(FactMetaData *left, FactMetaData *right) {
    assert(left != nullptr);
    assert(right != nullptr);

    factdata = left;
    factdata2 = right;
    factdata->enableMultiDisplay();
    factdata2->enableMultiDisplay();
    connect(factdata, &FactMetaData::multiDisplayChanged, this, &FlightDisplay::doDisplayUpdateExt);
    connect(factdata2, &FactMetaData::multiDisplayChanged, this,
            &FlightDisplay::doDisplayUpdateExt);
}

int FlightDisplay::checkDisplayColor(int chl) {
    if (displaySets.count() <= 0) {
        return _settingColor;
    }

    factdata = displaySets[0];
    _valueStr = displaySets[0]->display(chl);
    double last = displaySets[0]->rateValue(_valueStr.toDouble());
    last = qBound(0.0, last, 1.0);
    if (last != _rateValue) {
        _rateValue = last;
    }

    if (!airthmeticOpts.empty()) {
        int index = -1;
        int ivalue = -1;
        float value = -1;
        FactMetaData *item = nullptr;
        for (auto ix = airthmeticOpts.begin(); ix != airthmeticOpts.end(); ix++) {
            bool rtn = false;
            // 每一行配置数据分析
            QList<std::shared_ptr<AirthmeticPart>> &parts = (*ix).airthOpts;
            for (int mx = 0; (!rtn) && (mx < parts.count()); mx++) {
                bool first = false;
                bool subflag = parts[mx]->subNode.count() > 0 ? false : true;
                if (parts[mx]->index != index) {
                    index = parts[mx]->index;
                    if ((index < displaySets.count()) && (index >= 0)) {
                        item = displaySets[index];
                        if (parts[mx]->dtype == 1) {
                            value = item->display(chl).toFloat();
                        } else if (parts[mx]->dtype == 2) {
                            ivalue = item->rawValue(chl);
                        }
                    }
                }
                if (parts[mx]->dtype == 1) {
                    /// 范围判断 输入量是否在某区域内，返回对应的颜色信息
                    if ((value >= parts[mx]->apoint) && (value <= parts[mx]->bpoint)) {
                        first = true;
                    }
                } else if (parts[mx]->dtype == 2) {
                    if (ivalue == parts[mx]->abpoint) {
                        first = true;
                    }
                }
                /// 之前的元素为真的时候，处理子元素信息
                for (int kb = 0; (kb < parts[mx]->subNode.count()); kb++) {
                    std::shared_ptr<AirthmeticSub> npart = parts[mx]->subNode[kb];
                    if (npart->index != index) {
                        index = npart->index;
                        if ((index < displaySets.count()) && (index >= 0)) {
                            item = displaySets[index];
                            _valueStr = item->display(chl);
                            if (npart->dtype == 1) {
                                value = _valueStr.toFloat();
                            } else if (npart->dtype == 2) {
                                ivalue = _valueStr.toInt();
                            }
                        }
                    }
                    if (npart->dtype == 1) {
                        /// 范围判断 输入量是否在某区域内，返回对应的颜色信息
                        if ((value >= npart->apoint) && (value <= npart->bpoint)) {
                            subflag = true;
                        }
                    } else if (npart->dtype == 2) {
                        /// 判断是否相等
                        if (ivalue == npart->abpoint) {
                            subflag = true;
                        }
                    }
                }
                // 第一个与后面的都为真
                if (first && subflag) {
                    rtn = true;
                }
            }
            if (rtn) { // 满足条件
                return (*ix).color;
            }
        }
    }

    if (!logicalOpts.empty()) {

        for (auto bx = logicalOpts.begin(); bx != logicalOpts.end(); bx++) {
            bool rtnnode1 = true;
            bool rtnnode2 = false;
            QList<std::shared_ptr<LogicalOptPart>> &lopt = (*bx)->part1;
            if ((*bx)->airthpart.optype != LogicalInvalid) {
                rtnnode1 = (*bx)->airthpart.checkDisplayColor(displaySets, chl);
            }
            if ((*bx)->optype == LogicalOr) {
                rtnnode2 = false;
                for (auto dx = lopt.begin(); !rtnnode2 && (dx != lopt.end()); dx++) {
                    rtnnode2 |= (*dx)->checkDisplayColor(displaySets, chl);
                }
            } else {
                rtnnode2 = true;
                for (auto dx = lopt.begin(); rtnnode2 && (dx != lopt.end()); dx++) {
                    rtnnode2 &= (*dx)->checkDisplayColor(displaySets, chl);
                }
            }
            if (rtnnode1 && rtnnode2) { // 满足条件的情况
                return (*bx)->color;
            }
        }
    }

    return _settingColor;
}

bool FlightDisplay::AirthmeticOptPart::checkDisplayColor(QList<FactMetaData *> &metaSets, int chl) {
    if (optype == LogicalAnd) {
        bool rtn = true;
        for (auto bx = airthOpts.begin(); rtn && (bx != airthOpts.end()); bx++) {
            AirthmeticPart *index = (*bx).get();
            if (index->index < metaSets.count()) {
                FactMetaData *item = metaSets[index->index];
                if (index->dtype == 1) {
                    float value = item->display(chl).toFloat();
                    bool rt = (value >= index->apoint) && (value <= index->bpoint);
                    rtn = rtn & rt;
                } else if (index->dtype == 2) {
                    uint32_t value = item->rawValue(chl);
                    bool rt = (value == index->abpoint) ? true : false;
                    rtn = rtn & rt;
                } else {
                    rtn = false;
                }
            }
        }
        return rtn;
    }

    for (auto bx = airthOpts.begin(); (bx != airthOpts.end()); bx++) {
        AirthmeticPart *index = (*bx).get();
        if (index->index < metaSets.count()) {
            FactMetaData *item = metaSets[index->index];
            float value = item->display(chl).toFloat();
            if ((value >= index->apoint) && (value <= index->bpoint)) {
                return true;
            }
        }
    }
    return false;
}

bool FlightDisplay::LogicalOptPart::checkDisplayColor(QList<FactMetaData *> &metaSets, int chl) {
    if (optype == LogicalAnd) {
        bool rtn = true;
        for (auto bx = logical.begin(); (rtn) && (bx != logical.end()); bx++) {
            int itemValue = ((*bx) >> 16) & 0xffff;
            if (itemValue < metaSets.count()) {
                FactMetaData *item = metaSets[itemValue];
                uint32_t val = item->rawValue(chl);
                rtn = (val == ((*bx) & 0xffff)) ? true : false;
            }
        }

        return rtn;
    }

    bool rtn = false;
    for (auto bx = logical.begin(); (!rtn) && (bx != logical.end()); bx++) {
        int itemValue = ((*bx) >> 16) & 0xffff;
        if (itemValue < metaSets.count()) {
            FactMetaData *item = metaSets[itemValue];
            uint32_t val = item->rawValue(chl);
            if (val == ((*bx) & 0xffff)) {
                rtn = true;
            }
        }
    }
    return rtn;
}

void FlightDisplay::insertFactElem(FactMetaData *node) {
    if (node == nullptr) {
        COMMON_INFO("insert fact elem for vehicle failed");
        return;
    }

    factdata = node;
    connect(node, &FactMetaData::displayChanged, this, &FlightDisplay::doDisplayUpdate);
    connect(node, &FactMetaData::rawDisplayChanged, this, &FlightDisplay::doRawDisplayChanged);
}

void FlightDisplay::logicalDispayColor(const QString &sets, int idx) {
    QStringList kv = sets.split(gValSplitExpress2, Qt::SkipEmptyParts);
    if (kv.length() >= 2) {
        std::shared_ptr<LogicalOperation> node(new LogicalOperation());
        QStringList condi;
        node->color = colorMapName(kv[0]);
        for (int ix = 1; ix <= kv.length(); ix += 2) {
            if (kv[ix].contains("<") && kv[ix].contains(">")) {
                if (kv[ix].contains("&")) {
                    condi = kv[ix].split(gValSplitExpressAnd, Qt::SkipEmptyParts);
                    node->airthpart.optype = LogicalAnd;
                } else if (kv[ix].contains("|")) {
                    condi = kv[ix].split(gValSplitExpressOr, Qt::SkipEmptyParts);
                    node->airthpart.optype = LogicalOr;
                } else {
                    condi.clear();
                    condi.append(kv[ix]);
                    node->airthpart.optype = LogicalEqual;
                }
                for (int bx = 0; bx < condi.length(); bx++) {
                    QStringList detail = condi[bx].split(gValSplitExpress3, Qt::SkipEmptyParts);
                    if (detail.length() >= 3) {
                        std::shared_ptr<AirthmeticPart> anode(new AirthmeticPart());
                        anode->dtype = 1;
                        anode->index = detail[0].toInt();
                        anode->apoint = detail[1].toFloat();
                        anode->bpoint = detail[2].toFloat();
                        node->airthpart.airthOpts.append(anode);
                    } else if (detail.length() >= 2) {
                        std::shared_ptr<AirthmeticPart> anode(new AirthmeticPart());
                        anode->dtype = 2;
                        anode->index = detail[0].toInt();
                        anode->abpoint = detail[1].toInt();
                        node->airthpart.airthOpts.append(anode);
                    }
                }
            } else {
                std::shared_ptr<LogicalOptPart> part1(new LogicalOptPart());
                if (kv[ix].contains("&&")) {
                    condi = kv[ix].split(gValSplitExpressAnd, Qt::SkipEmptyParts);
                    part1->optype = LogicalAnd;
                } else if (kv[ix].contains("||")) {
                    condi = kv[ix].split(gValSplitExpressOr, Qt::SkipEmptyParts);
                    part1->optype = LogicalOr;
                } else {
                    condi.clear();
                    condi.append(kv[ix]);
                    part1->optype = LogicalEqual;
                }
                for (int bx = 0; bx < condi.length(); bx++) {
                    QStringList detail = condi[bx].split(gValSplitExpress3, Qt::SkipEmptyParts);
                    if (detail.length() >= 2) {
                        int valindex = detail[0].toInt();
                        valindex <<= 16;
                        if (detail[1].contains("0x") || detail[1].contains("0X")) {
                            valindex = valindex + detail[1].toInt(nullptr, 16);
                        } else {
                            valindex = valindex + detail[1].toInt();
                        }
                        part1->logical.append(valindex);
                    }
                }
                node->part1.append(part1);
            }
            if (ix + 1 < kv.length()) {
                if (kv[ix + 1].contains("&")) {
                    condi = kv[ix + 1].split(gValSplitExpressAnd, Qt::SkipEmptyParts);
                    node->optype = LogicalAnd;
                } else if (kv[ix + 1].contains("|")) {
                    condi = kv[ix + 1].split(gValSplitExpressOr, Qt::SkipEmptyParts);
                    node->optype = LogicalOr;
                } else {
                    node->optype = LogicalInvalid;
                    COMMON_INFO("invalid configure");
                }
            }
        }
        logicalOpts.append(node);
    } else if (kv.length() >= 1) {
        _settingColor = colorMapName(kv[0]);
    } else {
        _settingColor = GreenColor;
    }
}

void FlightDisplay::calculateColor(const QString &data, AirthmeticOperation &airthOpts) {
    std::shared_ptr<AirthmeticPart> node(new AirthmeticPart());
    QStringList airth = data.split(gValSplitExpress3, Qt::SkipEmptyParts);
    if (airth.length() >= 3) {
        node->dtype = 1;
        node->index = airth[0].toInt();
        node->apoint = airth[1].toFloat();
        node->bpoint = airth[2].toFloat();
        airthOpts.airthOpts.append(node);
    } else if (airth.length() >= 2) {
        node->index = airth[0].toInt();
        node->dtype = 2;
        if (airth[1].contains("0x") || airth[1].contains("0X")) {
            node->abpoint = airth[1].toInt(nullptr, 16);
        } else {
            node->abpoint = airth[1].toInt();
        }
        airthOpts.airthOpts.append(node);
    }
}

void FlightDisplay::calculateColorExt(const QString &data, AirthmeticOperation &airthOpts) {
    std::shared_ptr<AirthmeticPart> node(new AirthmeticPart());
    QStringList ors = data.split(gValSplitExpressOr);
    for (int mx = 0; mx < ors.length(); mx++) {
        QStringList airth = data.split(gValSplitExpress3, Qt::SkipEmptyParts);
        std::shared_ptr<AirthmeticSub> subNode(new AirthmeticSub());
        if (airth.length() >= 3) {
            subNode->dtype = 1;
            subNode->index = airth[0].toInt();
            subNode->apoint = airth[1].toFloat();
            subNode->bpoint = airth[2].toFloat();
            node->subNode.append(subNode);
        } else if (airth.length() >= 2) {
            subNode->index = airth[0].toInt();
            subNode->dtype = 2;
            if (airth[1].contains("0x") || airth[1].contains("0X")) {
                subNode->abpoint = airth[1].toInt(nullptr, 16);
            } else {
                subNode->abpoint = airth[1].toInt();
            }
            node->subNode.append(subNode);
        }
    }
    airthOpts.airthOpts.append(node);
}

void FlightDisplay::airthmeticDispayColor(const QString &sets, int idx) {
    QStringList kv = sets.split(gValSplitExpress2, Qt::SkipEmptyParts);
    if (kv.length() >= 2) {
        AirthmeticOperation airthnode;
        airthnode.color = colorMapName(kv[0]);
        if (kv.length() >= 3) {
            airthnode.optype = LogicalAnd;
            calculateColor(kv[1], airthnode);
            for (int kx = 3; kx < kv.length(); kx++) {
                calculateColorExt(kv[3], airthnode);
            }
        } else {
            calculateColor(kv[1], airthnode);
        }
        airthmeticOpts.append(airthnode);
    } else if (kv.length() >= 1) {
        _settingColor = colorMapName(kv[0]);
    } else {
        _settingColor = GreenColor;
    }
}

void FlightDisplay::parseDispayColor(const QString &sets) {
    QStringList em = sets.split(mLineSplitExpress);
    for (int ix = 0; ix < em.length(); ix++) {
        QString elem = em[ix];
        if (elem.contains("&&") || elem.contains("||")) {
            logicalDispayColor(elem, ix);
        } else {
            airthmeticDispayColor(elem, ix);
        }
    }
}

void FlightDisplay::parseDispaySets(FactDecoderMgr *decoder, const QString &sets) {
    QStringList em = sets.split(mLineSplitExpress);
    for (int ix = 0; ix < em.length(); ix++) {
        QStringList kv = em[ix].split(gValSplitExpress3);
        if (kv.length() >= 2) {
            QString key = kv[0];
            QString value = kv[1];
            FactTableModel *curTable = nullptr;
            if (key.contains("0x") || key.contains("0X")) {
                curTable = decoder->protoframe(key.toUInt(nullptr, 16));
            } else {
                curTable = decoder->protoframe(key.toUInt());
            }
            if (curTable != nullptr) {
                FactMetaData *data = curTable->loadFact(value.trimmed().toInt());
                assert(data != nullptr);
                displaySets.push_back(data);
                if (data != nullptr) {
                    data->enableMultiDisplay();
                    connect(data, &FactMetaData::multiDisplayChanged, this,
                            &FlightDisplay::checkRawDataChanged);
                }
            }
        }
    }
}

void FlightDisplay::manualUpdate(int chl) {
    if (factdata != nullptr) {
        factdata->manualUpdateData(chl);
    }
}

void FlightDisplay::setDisplay(const QString &name) { _valueStr = name; }

void FlightDisplay::afterUpdateDisplay() { emit displayValueChanged(); }

void FlightDisplay::unittestDisplay(int value, int chl) {
    assert(factdata != nullptr);
    if (factdata != nullptr) {
        factdata->setRawDisplayValue(chl, value);
    }
}

void FlightDisplay::checkRawDataChanged(int color, int channel) {
    _showColor = checkDisplayColor(channel) | (color & 0x100);
    emit updateColorValue(_valueStr, _showColor);
}

void FlightDisplay::doRawDisplayChanged(const QString &val, int color, int raw) {
    assert(factdata != nullptr);
    if (factdata == nullptr) {
        COMMON_INFO("invalid member factdata");
        return;
    }

    // 判断RANAGE是否越界
    double last = factdata->rateValue(val.toDouble());
    last = qBound(0.0, last, 1.0);
    if (last != _rateValue) {
        _rateValue = last;
    }

    _valueStr = val;
    _showColor = color;
    _rawValue = raw;

    emit updateColorValue(_valueStr, _showColor);
}

void FlightDisplay::doDisplayUpdateExt(int color, int chl) {
    assert(factdata2 != nullptr);
    assert(factdata != nullptr);

    _showColor = color;
    _valueStr = factdata ? factdata->display(chl) : "";
    _valueStr2 = factdata2 ? factdata2->display(chl) : "";
    float a = factdata->display(chl).toFloat();
    float b = factdata2->display(chl).toFloat();
    float value = a + b;

    if (_operate == InvalidOperate) {
        emit updateColorValue(_valueStr + "/" + _valueStr2, _showColor);
        return;
    }

    if (_operate == AddOperate) {
        value = a + b;
    } else if (_operate == SubOperate) {
        value = a - b;
    }

    for (int im = 0; im < _condList.count(); im++) {
        if (_condList[im].airth == MoreThan) {
            if (value > _condList[im].compare) {
                value += _condList[im].offset;
            }
        } else if (_condList[im].airth == LessThan) {
            if (value < _condList[im].compare) {
                value += _condList[im].offset;
            }
        }
    }

    emit updateColorValue(QString::asprintf("%.2f", value), _showColor);
}

void FlightDisplay::doDisplayUpdate(QString val, int color) {
    assert(factdata != nullptr);

    // 判断RANAGE是否越界
    double last = factdata->rateValue(val.toDouble());
    last = qBound(0.0, last, 1.0);
    if (last != _rateValue) {
        _rateValue = last;
    }


    if (!_formatStr.isEmpty() && (_formatStr.contains("%."))) {
        _valueStr = QString::asprintf("%.2f", val.toFloat());
    } else {
        _valueStr = val;
    }

    _showColor = color;
    emit updateColorValue(_valueStr, _showColor);
}

void FlightDisplay::setItemName(const QString &name) {
    _vertName = FactMetaData::vertString(name);
    _vert2Name = FactMetaData::vert2String(name);
    _vert3Name = FactMetaData::vert3String(name);
    _itemName = name;
}

void FlightDisplay::setCondition(const QString &cond) {
    QStringList em = cond.split(mLineSplitExpress);
    for (int ix = 0; ix < em.length(); ix++) {
        QStringList mx = em[ix].split(gValSplitExpress4);
        qDebug() << "setCondition " << mx;
        if (mx.length() >= 3) {
            ConditionNode node;
            if (mx[0].contains(">")) {
                node.airth = MoreThan;
            } else if (mx[0].contains("<")) {
                node.airth = LessThan;
            }
            node.compare = mx[1].toInt();
            node.offset = mx[2].toInt();
            _condList.append(node);
        }
    }
}
