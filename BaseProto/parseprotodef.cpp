#include "parseprotodef.h"
#include "factmetadata.h"
#include "stringtranslate.h"

#include "log_category.h"
#include "xlsxcellrange.h"
#include "xlsxchart.h"
#include "xlsxdocument.h"
#include "xlsxworkbook.h"

#include <QRegularExpression>

#define ProtoExcelSeq transToUnicode("序号")
#define ProtoExcelVar transToUnicode("变量名")
#define ProtoExcelName transToUnicode("名称")
#define ProtoExcelData transToUnicode("数据")
#define ProtoExcelView transToUnicode("故障")
#define ProtoExcelType transToUnicode("类型")
#define ProtoExcelUnit transToUnicode("单位")
#define ProtoExcelRange transToUnicode("范围")
#define ProtoExcelResol transToUnicode("分辨率")
#define ProtoExcelOffset transToUnicode("偏移")
#define ProtoExcelColor transToUnicode("颜色")
#define ProtoExcelRefer transToUnicode("设备ID")
#define ProtoFramePopup transToUnicode("弹窗信号")
#define ProtoFrameType transToUnicode("帧类型")

ParseProtoFile::ParseProtoFile() {

    datatypeMap.insert(QStringLiteral("Enum"), enumType);
    datatypeMap.insert(QStringLiteral("Signed"), signedType);
    datatypeMap.insert(QStringLiteral("Unsigned"), unsignedType);
    datatypeMap.insert(QStringLiteral("Struct"), structType);
    datatypeMap.insert(QStringLiteral("String"), stringType);
    datatypeMap.insert(QStringLiteral("Undefine"), undefineType);

    excelTableHeader.append(ColumnName{ProtoExcelSeq, true});
    excelTableHeader.append(ColumnName{ProtoExcelVar, true});
    excelTableHeader.append(ColumnName{ProtoExcelName, true});
    excelTableHeader.append(ColumnName{ProtoExcelData, true});
    excelTableHeader.append(ColumnName{ProtoExcelView, true});
    excelTableHeader.append(ColumnName{ProtoExcelType, true});
    excelTableHeader.append(ColumnName{ProtoExcelUnit, true});
    excelTableHeader.append(ColumnName{ProtoExcelRange, true});
    excelTableHeader.append(ColumnName{ProtoExcelResol, true});
    excelTableHeader.append(ColumnName{ProtoExcelOffset, true});
    excelTableHeader.append(ColumnName{ProtoExcelColor, true});
    excelTableHeader.append(ColumnName{ProtoExcelRefer, true});
}

void ParseProtoFile::loadSubPart(std::shared_ptr<TransMetaData> metaDataMap,
                                 QXlsx::Worksheet *sheet, const QString &frame) {

    QXlsx::CellRange range = sheet->dimension();
    int fcol = range.firstColumn();
    int lcol = range.lastColumn();
    int frow = range.firstRow();
    int lrow = range.lastRow();

    QVector<ColumnName> columnStatus;
    bool getValid = false;
    while ((frow <= lrow) && (!getValid)) {
        for (int col = fcol; (col <= lcol) && (!getValid); col++) {
            QVariant value = sheet->read(frow, col);
            getValid = value.isValid();
        }
        if (getValid) { // table header
            for (int col = fcol; col <= lcol; col++) {
                QVariant value = sheet->read(frow, col);
                if (!value.isNull()) {
                    for (auto itr = excelTableHeader.begin(); itr != excelTableHeader.end();
                         itr++) {
                        if (itr->name.contains(value.toString().trimmed())) {
                            columnStatus.append(ColumnName{value.toString(), col});
                            break;
                        }
                    }
                } else {
                    COMMON_WARN("failed to read element from proto file");
                }
            }
        }
        ++frow;
    }

    for (int row = frow; row <= lrow; row++) {
        std::shared_ptr<TransMetaData> dataElem(new TransMetaData());
        for (auto itr = columnStatus.begin(); itr != columnStatus.end(); itr++) {
            QVariant value = sheet->read(row, itr->column);
            if (itr->name.compare(ProtoExcelSeq) == 0) {
                QString num = value.toString().trimmed();
                dataElem->setItemNum(num);
            } else if (itr->name.compare(ProtoExcelName) == 0) {
                dataElem->setName(value.toByteArray());
            } else if (itr->name.compare(ProtoExcelData) == 0) {
                dataElem->setBitSet(value.toString().trimmed());
            } else if (itr->name.compare(ProtoExcelView) == 0) {
                dataElem->setErrString(value, frame, dataElem->itemNum());
            } else if (itr->name.compare(ProtoExcelType) == 0) {
                /// 子EXCEL页面类型处只定义简单数据类型，避免多层次查询
                ///
                if (!value.isNull()) {
                    QString dtypeName = value.toString().trimmed();
                    auto mx = datatypeMap.find(dtypeName);
                    if (mx != datatypeMap.end()) {
                        dataElem->setMetaDataType(mx.value());
                    } else {
                        dataElem->setMetaDataType(MetaDataType::undefineType);
                    }
                } else {
                    dataElem->setMetaDataType(MetaDataType::undefineType);
                }
            } else if (itr->name.compare(ProtoExcelResol) == 0) {
                handleResoultion(dataElem, value);
            } else if (itr->name.compare(ProtoExcelUnit) == 0) {
                dataElem->setUnits(value.toString());
            } else if (itr->name.compare(ProtoExcelRange) == 0) {
                dataElem->parseRange(value.toByteArray());
            } else if (itr->name.compare(ProtoExcelOffset) == 0) {
                dataElem->setOffset(value.toString().toInt());
            }
        }

        metaDataMap->insertFactNode(dataElem->itemNum(), dataElem);
    }
}

bool ParseProtoFile::handleMetaDataType(std::shared_ptr<TransMetaData> dataElem, QVariant value) {
    if (!value.isNull()) {
        dataElem->setMetaDataType(MetaDataType::undefineType);
        static QRegularExpression mSplitExpress("[-;；]");
        QString dtypeName = value.toString().trimmed();
        QStringList sets = dtypeName.split(mSplitExpress, Qt::SkipEmptyParts);
        for (auto x = datatypeMap.begin(); x != datatypeMap.end(); x++) {
            if (!sets.isEmpty() && sets[0].compare(x.key()) == 0) {
                dataElem->setMetaDataType(x.value());
                if (dataElem->metaDataType() == MetaDataType::structType) {
                    dataElem->setDestDataType(translatorStruct);
                } else if (dataElem->metaDataType() == MetaDataType::enumType) {
                    dataElem->setDestDataType(translatorEnums);
                } else if (dataElem->metaDataType() == MetaDataType::signedType ||
                           dataElem->metaDataType() == MetaDataType::unsignedType) {
                    dataElem->setDestDataType(translatorInt32);
                } else {
                    dataElem->setDestDataType(translatorString);
                }
            }
        }
    } else {
        dataElem->setMetaDataType(MetaDataType::undefineType);
    }

    return true;
}

bool ParseProtoFile::handleResoultion(std::shared_ptr<TransMetaData> dataElem, QVariant value) {
    if (!value.isNull()) {
        QString vm = value.toString().trimmed();
        if (vm.compare(QStringLiteral("-")) != 0) {
            if (vm.indexOf("/") >= 0) {
                //分辨率采用分数表示方法
                QStringList em = vm.split("/", Qt::SkipEmptyParts);
                if (em.size() >= 2) {
                    double numerator = em[0].toDouble();
                    double denominator = em[1].toDouble();
                    if (denominator == 0) { // todo
                        denominator = 1;
                    }
                    dataElem->setResolution(numerator / denominator);
                    dataElem->setDestDataType(translatorDouble);
                }
            } else if ((vm.indexOf("0.") >= 0) || (vm.contains("E-", Qt::CaseInsensitive))) {
                //分辨率采用小数表示
                dataElem->setResolution(value.toDouble());
                dataElem->setDestDataType(translatorDouble);
            } else {
                //分辨率采用整数（十进制数）
                dataElem->setResolution(value.toInt());
                if (dataElem->metaDataType() == MetaDataType::structType) {
                    dataElem->setDestDataType(translatorStruct);
                } else if (dataElem->metaDataType() == MetaDataType::enumType) {
                    dataElem->setDestDataType(translatorEnums);
                } else if (dataElem->metaDataType() == MetaDataType::signedType ||
                           dataElem->metaDataType() == MetaDataType::unsignedType) {
                    dataElem->setDestDataType(translatorInt32);
                } else {
                    dataElem->setDestDataType(translatorString);
                }
            }
        }
    }
    return true;
}

void ParseProtoFile::warnDefine(WarnConfigExt *metaDataMap, const QVariant &datas) {
    static QRegularExpression splitExpressA("[=]");
    static QRegularExpression splitExpressB("[\n]");
    static QRegularExpression regexBinStr(R"(^[01]+$)");
    QStringList sets = datas.toString().split(splitExpressB);
    if (sets.size() > 1) {
        foreach (QString elem, sets) {
            QString eachline = elem;
            std::shared_ptr<WarnConfNode> node(new WarnConfNode());
            QStringList eachSet = eachline.split(splitExpressA);
            if (eachSet.length() > 0) {
                bool ok = false;
                if (eachSet[0].contains("0x") || eachSet[0].contains("0X")) {
                    node->value = eachSet[0].trimmed().toInt(&ok, 16);
                } else {
                    QString val = eachSet[0].trimmed();
                    if (regexBinStr.match(val).hasMatch()) {
                        QByteArray binStr(val.toUtf8());
                        node->value = binStr.toInt(&ok, 2);
                    } else {
                        node->value = eachSet[0].toInt();
                    }
                }
            }
            if (eachSet.length() > 1) {
                node->warnStr = eachSet[1];
            }
            if (eachSet.length() > 2) {
                QString val = eachSet[2];
                if (val.contains(transToUnicode("1级")) || val.contains(transToUnicode("警告级"))) {
                    node->level = LevelOne;
                    node->color = 1; // Red
                } else if (val.contains(transToUnicode("2级")) ||
                           val.contains(transToUnicode("注意级"))) {
                    node->level = LevelTwo;
                    node->color = 2; // Orange
                } else if (val.contains(transToUnicode("3级")) ||
                           val.contains(transToUnicode("提示级"))) {
                    node->level = LevelThree;
                    node->color = 3; // Yellow
                }
            }
            metaDataMap->warns.insert(node->value, node);
        }
    }
}

void ParseProtoFile::warnCondition(PWarnConfGrpK &metaDataMap, const QVariant &datas) {
    static QRegularExpression splitExpressC("[/]");
    static QRegularExpression regexBinStr(R"(^[01]+$)");
    if (datas.toString().length() < 3) {
        return;
    }

    QStringList sets = datas.toString().split(splitExpressC);
    for (int ix = 0; ix < sets.length(); ix++) {
        QString valueelem = sets[ix];
        int last = valueelem.lastIndexOf("]");
        if (last < 0) {
            last = valueelem.lastIndexOf(")");
        }
        int frist = valueelem.lastIndexOf("[");
        if (frist < 0) {
            frist = valueelem.lastIndexOf("(");
        }
        if (last > 0 && frist >= 0) {
            bool ok = false;
            QString tmValue = valueelem.mid(frist + 1, last - frist - 1);
            metaDataMap->_referItem[ix] = tmValue.toInt();
            QString refItm = valueelem.mid(0, frist).trimmed();
            if (refItm.contains("0x") || refItm.contains("0X")) {
                int code = refItm.toInt(&ok, 16);
                metaDataMap->_referFrame[ix] = code;
            } else {
                if (regexBinStr.match(refItm).hasMatch()) {
                    QByteArray binStr(refItm.toUtf8());
                    int code = binStr.toInt(&ok, 2);
                    metaDataMap->_referFrame[ix] = code;
                } else {
                    metaDataMap->_referFrame[ix] = refItm.toInt();
                }
            }
        } else {
            COMMON_WARN("failed to parse {}", valueelem.toStdString());
        }
    }
}

bool ParseProtoFile::warnParse(QMap<PWarnConfGrpK, PWarnConfGrpV> &warnGroup,
                               FactTableModelCallabck protoDefine, const QString &dataFile) {
    QXlsx::Document xlsxRead(dataFile);
    if (!xlsxRead.load()) {
        return false;
    }

    QVector<ColumnName> excelTableHeader;
    excelTableHeader.append(ColumnName{ProtoExcelSeq, true});
    excelTableHeader.append(ColumnName{ProtoExcelName, true});
    excelTableHeader.append(ColumnName{ProtoExcelData, true});
    excelTableHeader.append(ColumnName{ProtoExcelView, true});
    excelTableHeader.append(ColumnName{ProtoExcelVar, true});
    excelTableHeader.append(ColumnName{ProtoFrameType, true});
    excelTableHeader.append(ColumnName{ProtoExcelOffset, true});
    excelTableHeader.append(ColumnName{ProtoFramePopup, true});

    QStringList list = xlsxRead.sheetNames();
    for (int i = 0; i < list.length(); i++) {
        QString protoFrame = list[i].trimmed();
        if (!xlsxRead.selectSheet(protoFrame)) {
            COMMON_INFO("sheet {} cannot read", protoFrame.toStdString().c_str());
            continue;
        }
        QXlsx::Worksheet *currSheet = xlsxRead.currentWorksheet();
        QXlsx::CellRange range = currSheet->dimension();
        int fcol = range.firstColumn();
        int lcol = range.lastColumn();
        int frow = range.firstRow();
        int lrow = range.lastRow();
        QVector<ColumnName> columnStatus;
        lrow = lrow > 256 ? 256 : lrow;
        lcol = lcol > 32 ? 32 : lcol;
        fcol = fcol < 0 ? 0 : fcol;
        frow = frow < 0 ? 0 : frow;
        /// 获取有效第一行
        bool getValid = false;
        while ((frow <= lrow) && (!getValid)) {
            for (int col = fcol; (col <= lcol) && (!getValid); col++) {
                QVariant value = currSheet->read(frow, col);
                getValid = value.isValid();
            }
            if (getValid) { // table header
                for (int col = fcol; col <= lcol; col++) {
                    QVariant value = currSheet->read(frow, col);
                    if (!value.isNull()) {
                        for (auto itr = excelTableHeader.begin(); itr != excelTableHeader.end();
                             itr++) {
                            if (itr->name.contains(value.toString().trimmed())) {
                                columnStatus.append(ColumnName{value.toString(), col});
                                break;
                            }
                        }
                    } else {
                        COMMON_WARN("failed to read element from proto file");
                    }
                }
            }
            ++frow;
        }
        /// 按Group数据
        int currIndex = 1;
        PWarnConfGrpK groupk = nullptr;
        PWarnConfGrpV groupV = nullptr;
        for (int row = frow; row <= lrow; row++) {
            for (auto itr2 = columnStatus.begin(); itr2 != columnStatus.end(); itr2++) {
                if (itr2->name.compare(ProtoExcelView) == 0) {
                    QVariant value = currSheet->read(row, itr2->column);
                    if (value.isValid()) {
                        if ((groupk == nullptr) || !groupk->groupName.contains(value.toString())) {
                            groupk = std::make_shared<WarnConfigGroupKey>();
                            groupV = std::make_shared<WarnConfigGroupVal>();
                            assert(groupk != nullptr);
                            assert(groupV != nullptr);
                            groupk->groupName = value.toString();
                            warnCondition(groupk, value);
                            warnGroup.insert(groupk, groupV);
                        }
                    }
                }
            }

            FactTableModel *factTable = nullptr;
            std::shared_ptr<WarnConfigExt> protoGroup(new WarnConfigExt());
            assert(protoGroup != nullptr);

            WarnConfigExt *metaDataMap = protoGroup.get();
            for (auto itr = columnStatus.begin(); itr != columnStatus.end(); itr++) {
                QVariant value = currSheet->read(row, itr->column);
                if (!value.isValid()) {
                    continue;
                }

                if (itr->name.compare(ProtoExcelSeq) == 0) {
                    QString num = value.toString().trimmed();
                    metaDataMap->_sequence = num.toInt();
                } else if (itr->name.compare(ProtoExcelName) == 0) {
                    metaDataMap->_warnItemName = (value.toByteArray());
                } else if (itr->name.compare(ProtoFramePopup) == 0) {
                    metaDataMap->_popupWin = value.toInt();
                } else if (itr->name.compare(ProtoExcelVar) == 0) {
                    metaDataMap->_frameName = value.toString();
                } else if (itr->name.compare(ProtoFrameType) == 0) {
                    bool ok = false;
                    if (value.toString().contains("0x") || value.toString().contains("0X")) {
                        metaDataMap->_frameCode = value.toString().trimmed().toInt(&ok, 16);
                    } else {
                        QString val = value.toString().trimmed();
                        metaDataMap->_frameCode = val.toInt(&ok, 2);
                    }
                    if (protoDefine(metaDataMap->_frameCode) != nullptr) {
                        factTable = protoDefine(metaDataMap->_frameCode);
                    }
                } else if (itr->name.compare(ProtoExcelOffset) == 0) {
                    metaDataMap->_frameOffset = value.toInt();
                } else if (itr->name.compare(ProtoExcelData) == 0) {
                    warnDefine(metaDataMap, value);
                }
            }
            if (factTable != nullptr) {
                FactMetaData *em = factTable->loadFact(metaDataMap->_frameOffset);
                protoGroup->_display = em; //
            }

            if (groupV.get() != nullptr) {
                groupV.get()->addNode(protoGroup);
            }

            ++currIndex;
        }
    }
    return true;
}

bool ParseProtoFile::protoParse(QVector<std::shared_ptr<WarnConfig>> &warns,
                                QMap<int, std::shared_ptr<FactTableModel>> &veh_proto_set,
                                const QString &dataFile, QDateTime dateTime) {

    // verify the input file is can been opened by xlsx lib
    QXlsx::Document xlsxRead(dataFile);
    if (!xlsxRead.load()) {
        return false;
    }

    QStringList list = xlsxRead.sheetNames();
    for (int i = 0; i < list.length() && (i < 50); i++) {
        QString protoFrame = list[i].trimmed();
        if (sheetHasProcess.find(protoFrame) != sheetHasProcess.end()) {
            COMMON_INFO("sheet {} is subnode", protoFrame.toStdString().c_str());
            continue;
        }

        if (!xlsxRead.selectSheet(protoFrame)) {
            continue;
        }

        /// 创建协议TABLE
        std::shared_ptr<FactTableModel> protoTable(new FactTableModel(dateTime));
        assert(protoTable != nullptr);

        FactTableModel *metaDataMap = protoTable.get();

        metaDataMap->setName(protoFrame);

        QXlsx::Worksheet *currSheet = xlsxRead.currentWorksheet();
        QXlsx::CellRange range = currSheet->dimension();
        int fcol = range.firstColumn();
        int lcol = range.lastColumn();
        int frow = range.firstRow();
        int lrow = range.lastRow();
        QVector<ColumnName> columnStatus;

        lrow = lrow > 256 ? 256 : lrow;
        lcol = lcol > 32 ? 32 : lcol;
        fcol = fcol < 0 ? 0 : fcol;
        frow = frow < 0 ? 0 : frow;

        /// 获取有效第一行
        bool getValid = false;
        while ((frow <= lrow) && (!getValid)) {
            for (int col = fcol; (col <= lcol) && (!getValid); col++) {
                QVariant value = currSheet->read(frow, col);
                getValid = value.isValid();
            }

            if (getValid) { // table header
                for (int col = fcol; col <= lcol; col++) {
                    QVariant value = currSheet->read(frow, col);
                    if (!value.isNull()) {
                        for (auto itr = excelTableHeader.begin(); itr != excelTableHeader.end();
                             itr++) {
                            if (itr->name.contains(value.toString().trimmed())) {
                                columnStatus.append(ColumnName{value.toString(), col});
                                break;
                            }
                        }
                    } else {
                        COMMON_WARN("failed to read element from proto file");
                    }
                }
            }

            ++frow;
        }

        int currIndex = 1;
        for (int row = frow; row <= lrow; row++) {
            std::shared_ptr<TransMetaData> dataElem(new TransMetaData());
            dataElem->setFrameName(protoFrame);

            for (auto itr = columnStatus.begin(); itr != columnStatus.end(); itr++) {
                QVariant value = currSheet->read(row, itr->column);
                if (itr->name.compare(ProtoExcelSeq) == 0) {
                    QString num = value.toString().trimmed();
                    if (!num.isEmpty()) {
                        dataElem->setItemNum(num);
                    } else {
                        dataElem->setItemNum(QString::number(currIndex));
                    }
                } else if (itr->name.compare(ProtoExcelVar) == 0) {
                    QString varName = value.toString().trimmed();
                    dataElem->setGroupName(varName);
                } else if (itr->name.compare(ProtoExcelName) == 0) {
                    dataElem->setName(value.toByteArray());
                } else if (itr->name.compare(ProtoExcelView) == 0) {
                    if (value.isValid()) {
                        std::shared_ptr<WarnConfig> onair = nullptr;
                        onair = dataElem->setErrString(value, protoFrame, dataElem->itemNum());
                        if (onair != nullptr) {
                            onair->_frameCode = protoFrame;
                            onair->_frameItem = dataElem->itemNum();
                            warns.append(onair);
                        }
                    }
                } else if (itr->name.compare(ProtoExcelData) == 0) {
                    dataElem->setBitSet(value.toString().trimmed());
                } else if (itr->name.compare(ProtoExcelRefer) == 0) {
                    dataElem->parseRefer(value.toString());
                } else if (itr->name.compare(ProtoExcelType) == 0) {
                    handleMetaDataType(dataElem, value);
                } else if (itr->name.compare(ProtoExcelResol) == 0) {
                    handleResoultion(dataElem, value);
                } else if (itr->name.compare(ProtoExcelUnit) == 0) {
                    dataElem->setUnits(value.toString());
                } else if (itr->name.compare(ProtoExcelRange) == 0) {
                    if (dataElem->metaDataType() == MetaDataType::structType) {
                        dataElem->setCopyData(value.toString());
                    } else {
                        dataElem->parseRange(value.toByteArray());
                    }
                } else if (itr->name.compare(ProtoExcelOffset) == 0) {
                    dataElem->setOffset(value.toString().toInt());
                } else if (itr->name.compare(ProtoExcelColor) == 0) {
                    dataElem->parseColor(value.toByteArray());
                }
            }

            if (dataElem->metaDataType() == MetaDataType::structType) {
                if (xlsxRead.selectSheet(dataElem->copyData())) {
                    QXlsx::Worksheet *nextsheet = xlsxRead.currentWorksheet();
                    loadSubPart(dataElem, nextsheet, protoFrame); // next sub node
                    sheetHasProcess.insert(dataElem->copyData(), dataElem->copyData());
                }
            }

            if (metaDataMap != nullptr) {
                metaDataMap->addFactMeta(dataElem->itemNum(), dataElem);
            }

            ++currIndex;
        }

        QString type = "";
        FactMetaData *frametype = metaDataMap->find(ProtoFrameType);
        //assert(frametype != nullptr);
        if (frametype != nullptr) {
            type = frametype->units();
        } else {
            COMMON_WARN("unable parse proto file");
        }

        bool ok = false;
        if (!type.isEmpty()) {
            int decimal = 0;

            // 协议帧类型如果是16进制字符串或十进制字符串
            if (type.contains("0x")) {
                decimal = type.trimmed().toInt(&ok, 16);
            } else {
                decimal = type.trimmed().toInt(&ok);
            }

            protoTable->setCode(decimal);
            veh_proto_set.insert(decimal, protoTable);
        } else {
            COMMON_WARN("unable to parse proto type");
        }
    }

    return true;
}
