#include "adjustControl.h"
#include "factmetadata.h"
#include "log_category.h"
#include "stringtranslate.h"
#include "xlsxcellrange.h"
#include "xlsxchart.h"
#include "xlsxdocument.h"
#include "xlsxworkbook.h"

#define AdjustVerifyOutFreq 10

#define COLUMN_COMMAND_TYPE transToUnicode("指令类别")
#define COLUMN_COMMAND_SEQ transToUnicode("指令序号")
#define COLUMN_COMMAND_NAME transToUnicode("指令名称")
#define COLUMN_COMMAND_GROUP transToUnicode("指令组号")
#define COLUMN_COMMAND_REFA transToUnicode("遥调引用")
#define COLUMN_COMMAND_POPUP transToUnicode("弹窗信号")
#define COLUMN_COMMAND_CODE transToUnicode("指令编码(HEX)")

#define COLUMN_ADJUST_NO transToUnicode("序号")
#define COLUMN_ADJUST_NAME transToUnicode("指令名称")
#define COLUMN_ADJUST_TYPE transToUnicode("类型")
#define COLUMN_ADJUST_BYTE transToUnicode("字节数")
#define COLUMN_ADJUST_BITS transToUnicode("数据")
#define COLUMN_ADJUST_RES transToUnicode("分辨率")
#define COLUMN_ADJUST_RANGE transToUnicode("范围")
#define COLUMN_ADJUST_GROUP transToUnicode("组名")
#define COLUMN_ADJUST_ITEM transToUnicode("分组")
#define COLUMN_ADJUST_UNIT transToUnicode("单位")
#define COLUMN_ADJUST_FEED transToUnicode("回报")
#define COLUMN_ADJUST_DEFT transToUnicode("默认")
#define COLUMN_ADJUST_DTAT transToUnicode("数据类型")
#define COLUMN_ADJUST_REGP transToUnicode("正则表达")

AdjustControl::AdjustControl() : _last_sys_group(0), _last_ctrl_group(0) {}

AdjustControl::~AdjustControl() {}

FactListModel *AdjustControl::commandTypes() { return _command_types.cmds(); }

RCAdjustItem *AdjustControl::selectAdjustItem(const QString &type, int code) const {
    auto get = groupAdjust_set.find(type);
    if (get != groupAdjust_set.begin()) {
        RCAdjustGroup *group = get.value().get();
        if (group != nullptr) {
            return group->findSimpleItem(code);
        } else {
            COMMON_WARN("not found adjust item {}: {}", type.toStdString().c_str(), code);
        }
    } else {
        COMMON_WARN("not found adjust item {}: {}", type.toStdString().c_str(), code);
    }
    return nullptr;
}

RCAdjustGroup *AdjustControl::atcDeviceSource() {
    return selectAdjustGroup(transToUnicode("应答机"));
}

RCAdjustGroup *AdjustControl::vhfWorkFreqSource() {
    return selectAdjustGroup(transToUnicode("电台控制"));
}

RCAdjustGroup *AdjustControl::dataLinkSource() {
    return selectAdjustGroup(transToUnicode("链路功率"));
}

RCAdjustGroup *AdjustControl::dataLinkDevSource() { return selectAdjustGroup(transToUnicode("天线控制"));}

RCAdjustGroup *AdjustControl::batteryOnOff() { return selectAdjustGroup(transToUnicode("蓄电池")); }
RCAdjustGroup *AdjustControl::devicePowerOnOff() {return selectAdjustGroup(transToUnicode("供电管理"));}
RCAdjustGroup *AdjustControl::slideingAdjust() { return selectAdjustGroup(transToUnicode("滑行")); }
RCAdjustGroup *AdjustControl::landingAdjust() { return selectAdjustGroup(transToUnicode("起降")); }
RCAdjustGroup *AdjustControl::curiseAdjust() { return selectAdjustGroup(transToUnicode("空中")); }
RCAdjustGroup *AdjustControl::adjustTypes() { return selectAdjustGroup(transToUnicode("遥调")); }

RCAdjustGroup *AdjustControl::selectAdjustGroup(const QString &type) {
    auto mx = groupAdjust_set.find(type);
    RCAdjustGroup *adjustGroup = nullptr;
    if (mx != groupAdjust_set.end()) {
        adjustGroup = mx.value().get();
    }
    return adjustGroup;
}

RCCommandGroup *AdjustControl::generatorControl() {
    return _ext_opt_cmds.commandGroup(transToUnicode("发电机"));
}

RCCommandGroup *AdjustControl::infoSrcControl() {
    return _ext_opt_cmds.commandGroup(transToUnicode("信息源"));
}

RCCommandGroup *AdjustControl::controlRateControl() {
    return _ext_opt_cmds.commandGroup(transToUnicode("控制律"));
}

void AdjustControl::updateSysCmdGroup(QString name) {
    sys_cmds_grp = name;
    emit sysCmdGroupChanged();
    emit cmdAdjustsChanged();
}

RCAdjustGroup *AdjustControl::cmdAdjusts() { return selectAdjustGroup(sys_cmds_grp); }

RCCommandGroup *AdjustControl::sysCmdGroup() { return _sys_opt_cmds.commandGroup(sys_cmds_grp); }

void AdjustControl::uptExtCmdGroup(QString type) {
    ext_cmds_grp = type;
    emit extCmdGroupChanged();
}

RCCommandGroup *AdjustControl::taskInitialize() {
    COMMON_INFO("try get task initialize operation");
    return _sys_opt_cmds.commandGroup(transToUnicode("任务初始化"));
}

RCCommandGroup *AdjustControl::extentCmdGroup() { return _ext_opt_cmds.commandGroup(ext_cmds_grp); }

RCAdjustGroup *AdjustControl::groupAdjustCmd(QString type) { return selectAdjustGroup(type); }

RCCommandGroup *AdjustControl::OptCommandGroup::commandGroup(const QString &val) {
    for (auto ix = command_group.begin(); !val.isEmpty() && ix != command_group.end(); ix++) {
        if ((*ix).get()->groupName().compare(val) == 0) {
            //(*ix).get()->resetChecking();
            return (*ix).get();
        }
    }

    return nullptr;
}

RCCommandGroup *AdjustControl::OptCommandGroup::commandGroup(int val) {
    std::shared_ptr<RCCommandGroup> curr = nullptr;
    if ((curr = command_group.at(val)) != nullptr) {
        return curr.get();
    }

    return nullptr;
}

RCCommandGroup *AdjustControl::commandGroup() {
    return _command_types.commandGroup(_last_ctrl_group);
}

void AdjustControl::setSelCmdGroup(int val) {
    _last_ctrl_group = val;
    emit selCmdGroupChanged();
    emit commandGroupChanged();
}

bool AdjustControl::OptCommandGroup::resetCommandState(int code) const {
    auto stx = command_group.begin();
    while (stx != command_group.end()) {
        if (stx->get()->resetWorkState(code)) {
            return true;
        }

        ++stx;
    }

    return false;
}

RCCommandItem *AdjustControl::getControlByFeedback(int code) const {
    auto bmx = ext_command_map.find(code);
    if (bmx != ext_command_map.end()) {
        bmx->get()->setWorking(false);
    }
    auto mx = all_command_map.find(code);
    if (mx != all_command_map.end()) {
        return mx->get();
    }
    return nullptr;
}

RCAdjustItem *AdjustControl::getAdjustByfeedback(int code) const {
    RCAdjustItem *item = nullptr;
    auto mx = groupAdjust_set.begin();
    while (mx != groupAdjust_set.end()) {
        item = mx.value()->findAdjustByFeed(code);
        if (item != nullptr) {
            break;
        }
        ++mx;
    }
    return item;
}

RCAdjustItem *AdjustControl::getAdjustByfeedback(int feedback, int cmdtype, int subtype) const {
    RCAdjustItem *item = nullptr;
    auto mx = groupAdjust_set.begin();
    while (mx != groupAdjust_set.end()) {
        item = mx.value()->findItem(feedback, cmdtype, subtype);
        if (item != nullptr) {
            break;
        }

        ++mx;
    }
    return item;
}

int AdjustControl::readSheetHeader(QMap<QString, int> &columnStatus, QXlsx::Worksheet *currSheet) {
    QXlsx::CellRange range = currSheet->dimension();
    int fcol = range.firstColumn();
    int lcol = range.lastColumn();
    int frow = range.firstRow();
    int lrow = range.lastRow();

    QVector<QString> excelTableHeader;
    excelTableHeader.append(COLUMN_ADJUST_NO);
    excelTableHeader.append(COLUMN_ADJUST_NAME);
    excelTableHeader.append(COLUMN_ADJUST_TYPE);
    excelTableHeader.append(COLUMN_ADJUST_BYTE);
    excelTableHeader.append(COLUMN_ADJUST_RES);
    excelTableHeader.append(COLUMN_ADJUST_RANGE);
    excelTableHeader.append(COLUMN_ADJUST_GROUP);
    excelTableHeader.append(COLUMN_ADJUST_ITEM);
    excelTableHeader.append(COLUMN_ADJUST_UNIT);
    excelTableHeader.append(COLUMN_ADJUST_FEED);
    excelTableHeader.append(COLUMN_ADJUST_DTAT);
    excelTableHeader.append(COLUMN_ADJUST_BITS);
    excelTableHeader.append(COLUMN_ADJUST_REGP);
    excelTableHeader.append(COLUMN_ADJUST_DEFT);

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
                        if (itr->contains(value.toString().trimmed())) {
                            columnStatus.insert(*itr, col);
                        }
                    }
                } else {
                    COMMON_WARN("failed to read excel content");
                }
            }
        }

        ++frow;
    }

    return frow;
}

void AdjustControl::reloadStructSheet(TransMetaData *item, QXlsx::Worksheet *currSheet,
                                      QMap<QString, MetaValueType> &datatypeMap) {
    if (currSheet == nullptr || item == nullptr) {
        COMMON_INFO("try to parse sheet, invalid input param");
        return;
    }

    QXlsx::CellRange range = currSheet->dimension();
    int frow = range.firstRow();
    int lrow = range.lastRow();

    QMap<QString, int> columnStatus;
    frow = readSheetHeader(columnStatus, currSheet);
    if (columnStatus.size() <= 0) {
        COMMON_INFO("read excel failed");
        return;
    }

    int countElem = 0;
    for (int row = frow; row <= lrow; row++) {
        QVariant name = currSheet->read(row, columnStatus.find(COLUMN_ADJUST_NAME).value());
        if (!name.isValid()) {
            continue;
        }

        QVariant byteLen = currSheet->read(row, columnStatus.find(COLUMN_ADJUST_BYTE).value());
        QVariant dbits = currSheet->read(row, columnStatus.find(COLUMN_ADJUST_BITS).value());
        QVariant dtype = currSheet->read(row, columnStatus.find(COLUMN_ADJUST_DTAT).value());
        QVariant range = currSheet->read(row, columnStatus.find(COLUMN_ADJUST_RANGE).value());
        std::shared_ptr<TransMetaData> metaData(new TransMetaData());
        assert(metaData != nullptr);
        metaData->setName(name.toByteArray());
        auto m = datatypeMap.find(dtype.isValid() ? dtype.toString().trimmed() : "NA");
        metaData->setMetaValueType(m != datatypeMap.end() ? m.value() : valueTypeUndef);
        metaData->setBitSet(dbits.isValid() ? dbits.toString().trimmed() : "NA");
        metaData->setByteCount(byteLen.isValid() ? byteLen.toInt() * 8 : 0);
        metaData->parseRange(range.isValid() ? range.toString() : "");
        item->insertFactNode(countElem++, metaData);
    }
}

void AdjustControl::reloadStructSheet(RCAdjustItem *item, QXlsx::Worksheet *currSheet,
                                      QMap<QString, MetaValueType> &datatypeMap) {
    if (currSheet == nullptr || item == nullptr) {
        COMMON_INFO("try to parse sheet, invalid input param");
        return;
    }

    QXlsx::CellRange range = currSheet->dimension();
    int frow = range.firstRow();
    int lrow = range.lastRow();

    QMap<QString, int> columnStatus;
    frow = readSheetHeader(columnStatus, currSheet);
    if (columnStatus.size() <= 0) {
        COMMON_INFO("read excel failed");
        return;
    }

    for (int row = frow; row <= lrow; row++) {
        QVariant byteLen = currSheet->read(row, columnStatus.find(COLUMN_ADJUST_BYTE).value());
        QVariant dbits = currSheet->read(row, columnStatus.find(COLUMN_ADJUST_BITS).value());
        QVariant dtype = currSheet->read(row, columnStatus.find(COLUMN_ADJUST_DTAT).value());
        QVariant range = currSheet->read(row, columnStatus.find(COLUMN_ADJUST_RANGE).value());
        if (byteLen.isValid() && dbits.isValid() && dtype.isValid() && range.isValid()) {
            FactMetaData *metaData = new FactMetaData();
            assert(metaData != nullptr);
            metaData->parseRange(range.toString());
            auto m = datatypeMap.find(dtype.toString().trimmed());
            metaData->setMetaValueType(m != datatypeMap.end() ? m.value() : valueTypeUndef);
            metaData->setBitSet(dbits.toString().trimmed());
            metaData->setByteCount(byteLen.toInt() * 8);
            item->addMetaData(metaData);
        }
    }
}

void AdjustControl::reloadEnumSheet(QXlsx::Worksheet *currSheet, QMap<int, QString> &confs) {
    QXlsx::CellRange range = currSheet->dimension();
    int frow = range.firstRow();
    int lrow = range.lastRow();

    QMap<QString, int> columnStatus;
    frow = readSheetHeader(columnStatus, currSheet);
    if (columnStatus.size() <= 0) {
        COMMON_INFO("read excel failed");
        return;
    }

    for (int row = frow; row <= lrow; row++) {
        QVariant number = currSheet->read(row, columnStatus.find(COLUMN_ADJUST_NO).value());
        QVariant name = currSheet->read(row, columnStatus.find(COLUMN_ADJUST_NAME).value());
        if (number.isValid() && name.isValid()) {
            confs.insert(number.toInt(), name.toString());
        }
    }
}

int AdjustControl::reloadStruct(int frow, int lrow, QMap<QString, int> &columnStatus,
                                QMap<QString, MetaValueType> &datatypeMap, const QString &groupName,
                                const QXlsx::Worksheet *currSheet, QXlsx::Document *xlsxRead,
                                std::shared_ptr<RCAdjustItem> subMap) {

    int group = 0;
    bool startgroup = false;
    for (int row = frow; row <= lrow; row++) {
        QVariant defVal = currSheet->read(row, columnStatus.find(COLUMN_ADJUST_GROUP).value());
        QVariant back = currSheet->read(row, columnStatus.find(COLUMN_ADJUST_FEED).value());
        QVariant ingrp = currSheet->read(row, columnStatus.find(COLUMN_ADJUST_ITEM).value());

        if (back.isValid()) {
            bool ok = false;
            int decimal = back.toString().trimmed().toInt(&ok, 16);
            if (defVal.isValid() && defVal.toString().compare(groupName) != 0) {
                return row - frow - 1;
            }
            if (subMap->feedback() != decimal) {
                return row - frow - 1;
            }
            if (ingrp.isValid()) {
                if (!startgroup) {
                    startgroup = true;
                } else {
                    return row - frow - 1;
                }
            }
        }

        TransMetaData *dataElem = new TransMetaData();
        assert(dataElem != nullptr);

        dataElem->setGroupName(groupName);

        QVariant number = currSheet->read(row, columnStatus.find(COLUMN_ADJUST_NO).value());
        if (number.isValid()) {
            dataElem->setItemNum(number.toString());
        }

        QVariant name = currSheet->read(row, columnStatus.find(COLUMN_ADJUST_NAME).value());
        if (name.isValid()) {
            dataElem->setName(name.toByteArray());
        }

        QVariant bsize = currSheet->read(row, columnStatus.find(COLUMN_ADJUST_BYTE).value());
        if (bsize.isValid()) {
            dataElem->setByteCount(bsize.toInt() * 8);
        }

        QVariant bits = currSheet->read(row, columnStatus.find(COLUMN_ADJUST_BITS).value());
        if (bits.isValid()) {
            dataElem->setBitSet(bits.toString().trimmed());
        }

        QVariant reso = currSheet->read(row, columnStatus.find(COLUMN_ADJUST_RES).value());
        if (reso.isValid()) {
            QString vm = reso.toString().trimmed();
            if (vm.compare(transToUnicode("—")) != 0) {
                dataElem->setResolution(vm.toDouble());
            }
        }

        QVariant type = currSheet->read(row, columnStatus.find(COLUMN_ADJUST_DTAT).value());
        if (type.isValid()) {
            auto m = datatypeMap.find(type.toByteArray());
            dataElem->setMetaValueType(m != datatypeMap.end() ? m.value() : valueTypeUndef);
        }

        QVariant regp = currSheet->read(row, columnStatus.find(COLUMN_ADJUST_REGP).value());
        if (regp.isValid()) {
            QRegExp rx(regp.toString());
            dataElem->setRegExpress(rx);
        } else {
            QRegExp rx("[0-9.]");
            dataElem->setRegExpress(rx);
        }

        bool newSheet = false;
        QVariant units = currSheet->read(row, columnStatus.find(COLUMN_ADJUST_UNIT).value());
        if (units.isValid()) {
            QString unitname = units.toString().trimmed();
            dataElem->setUnits(unitname);
            if (xlsxRead->selectSheet(unitname)) {
                newSheet = true;
            }
        }

        QVariant defts = currSheet->read(row, columnStatus.find(COLUMN_ADJUST_DEFT).value());
        if (defts.isValid()) {
            dataElem->itemVisible(defts.toUInt());
        }

        QVariant range = currSheet->read(row, columnStatus.find(COLUMN_ADJUST_RANGE).value());
        if (range.isValid()) {
            QString vm = range.toString().trimmed();
            dataElem->parseRange(vm);
        }

        subMap->addMetaData(dataElem);

        if (newSheet && dataElem->metaValueType() == valueTypeStruct) {
            QXlsx::Worksheet *subSheet = xlsxRead->currentWorksheet();
            reloadStructSheet(dataElem, subSheet, datatypeMap);

        } else if (newSheet) {
            QMap<int, QString> rangeMaps;
            QXlsx::Worksheet *subSheet = xlsxRead->currentWorksheet();
            reloadEnumSheet(subSheet, rangeMaps);
            QString vm = range.toString().trimmed();
            dataElem->parseRangeExt(vm, rangeMaps);
        }
    }

    return lrow - frow;
}

bool AdjustControl::reloadMoreAdjust(const QStringList &files) {
    for (int i = 0; i < files.size(); i++) {
        QString value = files.at(i);
        reloadAdjusts(value);
    }

    return true;
}

bool AdjustControl::reloadAdjusts(const QString &file) {
    QXlsx::Document xlsxRead(file);
    if (!xlsxRead.load()) {
        COMMON_WARN("failed to read excel {}", file.toStdString().c_str());
        return false;
    }

    COMMON_INFO("read excel {}", file.toStdString().c_str());
    _adjustFile.append(file);

    QMap<QString, MetaValueType> datatypeMap;
    datatypeMap.insert(transToUnicode("Uint8"), valueTypeUint8);
    datatypeMap.insert(transToUnicode("Int8"), valueTypeInt8);
    datatypeMap.insert(transToUnicode("Uint16"), valueTypeUint16);
    datatypeMap.insert(transToUnicode("Int16"), valueTypeInt16);
    datatypeMap.insert(transToUnicode("Uint32"), valueTypeUint32);
    datatypeMap.insert(transToUnicode("Int32"), valueTypeInt32);
    datatypeMap.insert(transToUnicode("U8"), valueTypeUint8);
    datatypeMap.insert(transToUnicode("S8"), valueTypeInt8);
    datatypeMap.insert(transToUnicode("U16"), valueTypeUint16);
    datatypeMap.insert(transToUnicode("S16"), valueTypeInt16);
    datatypeMap.insert(transToUnicode("U32"), valueTypeUint32);
    datatypeMap.insert(transToUnicode("S32"), valueTypeInt32);
    datatypeMap.insert(transToUnicode("Enum"), valueTypeEnum);
    datatypeMap.insert(transToUnicode("Struct"), valueTypeStruct);

    QStringList list = xlsxRead.sheetNames();
    for (int i = 0; i < list.length(); i++) {
        QString protoFrame = list[i].trimmed();
        if (!(protoFrame.compare("AdjustGroup") == 0 || protoFrame.compare("AdjustExt") == 0)) {
            continue;
        }

        if (!xlsxRead.selectSheet(protoFrame)) {
            continue;
        }

        QXlsx::Worksheet *currSheet = xlsxRead.currentWorksheet();
        QXlsx::CellRange range = currSheet->dimension();
        int frow = range.firstRow();
        int lrow = range.lastRow();

        QMap<QString, int> columnStatus;
        frow = readSheetHeader(columnStatus, currSheet);
        if (columnStatus.size() <= 0) {
            COMMON_INFO("read excel failed");
            continue;
        }

        QString lastType = "";
        std::shared_ptr<RCAdjustGroup> groupCommand = nullptr;

        for (int row = frow; row <= lrow; row++) {

            int decimal = 0;
            std::shared_ptr<RCAdjustItem> dataElem(new RCAdjustItem());
            if (dataElem == nullptr) {
                COMMON_ERROR("allocate memory failed");
                continue;
            }

            QVariant number = currSheet->read(row, columnStatus.find(COLUMN_ADJUST_NO).value());
            if (number.isValid()) {
                dataElem->metaData()->setItemNum(number.toString());
            }

            QVariant name = currSheet->read(row, columnStatus.find(COLUMN_ADJUST_NAME).value());
            if (name.isValid()) {
                dataElem->metaData()->setName(name.toByteArray());
            }

            QVariant bsize = currSheet->read(row, columnStatus.find(COLUMN_ADJUST_BYTE).value());
            if (bsize.isValid()) {
                dataElem->metaData()->setByteCount(bsize.toInt() * 8);
            }

            QVariant ctype = currSheet->read(row, columnStatus.find(COLUMN_ADJUST_TYPE).value());
            if (ctype.isValid()) {
                dataElem->setCommandType(ctype.toString());
            }

            QVariant reso = currSheet->read(row, columnStatus.find(COLUMN_ADJUST_RES).value());
            if (reso.isValid()) {
                QString vm = reso.toString().trimmed();
                if (vm.compare(transToUnicode("—")) != 0) {
                    dataElem->metaData()->setResolution(vm.toDouble());
                }
            }

            QVariant defVal = currSheet->read(row, columnStatus.find(COLUMN_ADJUST_GROUP).value());
            if (lastType.isEmpty() ||
                (defVal.isValid() && lastType.compare(defVal.toString()) != 0)) {
                if (groupCommand != nullptr) {
                    auto mx = groupAdjust_set.find(groupCommand->groupName());
                    if (mx == groupAdjust_set.end()) {
                        QByteArray groupArr = groupCommand->groupName().toLocal8Bit();
                        COMMON_INFO("read excel group {}", groupArr.toStdString().c_str());
                        groupAdjust_set.insert(groupCommand->groupName(), groupCommand);
                    }
                }
                groupCommand = std::shared_ptr<RCAdjustGroup>(new RCAdjustGroup());
                groupCommand->setGroupName(defVal.toString());
                lastType = defVal.toString();
            }

            QVariant back = currSheet->read(row, columnStatus.find(COLUMN_ADJUST_FEED).value());
            if (back.isValid()) {
                bool ok = false;
                decimal = back.toString().trimmed().toInt(&ok, 16);
                dataElem->setFeedback(decimal);
            }

            bool newSheet = false;
            QVariant units = currSheet->read(row, columnStatus.find(COLUMN_ADJUST_UNIT).value());
            if (units.isValid()) {
                QString unitName = units.toString().trimmed();
                if (xlsxRead.selectSheet(unitName)) {
                    newSheet = true;
                }

                dataElem->metaData()->setUnits(units.toString().trimmed());
            }

            QVariant regp = currSheet->read(row, columnStatus.find(COLUMN_ADJUST_REGP).value());
            if (regp.isValid()) {
                QRegExp rx(regp.toString());
                dataElem->metaData()->setRegExpress(rx);
            }

            QVariant type = currSheet->read(row, columnStatus.find(COLUMN_ADJUST_DTAT).value());
            if (type.isValid()) {
                auto m = datatypeMap.find(type.toByteArray());
                dataElem->metaData()->setMetaValueType(m != datatypeMap.end() ? m.value()
                                                                              : valueTypeUndef);
            }

            QVariant range = currSheet->read(row, columnStatus.find(COLUMN_ADJUST_RANGE).value());
            if (newSheet) {
                QXlsx::Worksheet *subSheet = xlsxRead.currentWorksheet();
                if (dataElem->metaData()->metaValueType() == valueTypeStruct) {
                    reloadStructSheet(dataElem.get(), subSheet, datatypeMap);
                } else {
                    QMap<int, QString> rangeMaps;
                    reloadEnumSheet(subSheet, rangeMaps);
                    QString vm = range.toString().trimmed();
                    dataElem->metaData()->parseRangeExt(vm, rangeMaps);
                }
            } else {
                if (range.isValid()) {
                    QString vm = range.toString().trimmed();
                    dataElem->metaData()->parseRange(vm);
                }
            }

            row += reloadStruct(row, lrow, columnStatus, datatypeMap, lastType, currSheet,
                                &xlsxRead, dataElem);
            dataElem->setGroupName(lastType);
            groupCommand->add(dataElem);
        }

        if (groupCommand != nullptr) {
            auto mx = groupAdjust_set.find(groupCommand->groupName());
            if (mx == groupAdjust_set.end()) {
                COMMON_INFO("read excel group {}", groupCommand->groupName().toStdString().c_str());
                groupAdjust_set.insert(groupCommand->groupName(), groupCommand);
            }
        }
    }

    return true;
}

bool AdjustControl::reloadCommands(const QString &file) {
    QXlsx::Document xlsxRead(file);
    // check th input file is able to open or not
    if (!xlsxRead.load()) {
        return false;
    }

    _controlFile = file;

    int groupCtrlType = 0;
    bool enableCmdExt = false;
    OptCommandGroup *listModel = nullptr;

    // read all sheet names of the input excel file
    QStringList list = xlsxRead.sheetNames();
    for (int i = 0; i < list.length(); i++) {

        // reading sheet 'i' page, active that sheet page
        QString protoFrame = list[i].trimmed();
        if (!xlsxRead.selectSheet(protoFrame)) {
            continue;
        }

        if (protoFrame.compare("syscontrol") == 0) {
            enableCmdExt = false;
            listModel = &_sys_opt_cmds;
        } else if (protoFrame.compare("ackcontrol") == 0) {
            enableCmdExt = true;
            listModel = &_ext_opt_cmds;
        } else {
            enableCmdExt = false;
            listModel = &_command_types;
        }

        QXlsx::Worksheet *currSheet = xlsxRead.currentWorksheet();
        QXlsx::CellRange range = currSheet->dimension();
        int fcol = range.firstColumn();
        int lcol = range.lastColumn();
        int frow = range.firstRow();
        int lrow = range.lastRow();

        QVector<QString> excelTableHeader;

        excelTableHeader.append(COLUMN_COMMAND_TYPE);
        excelTableHeader.append(COLUMN_COMMAND_SEQ);
        excelTableHeader.append(COLUMN_COMMAND_NAME);
        excelTableHeader.append(COLUMN_COMMAND_CODE);
        excelTableHeader.append(COLUMN_COMMAND_GROUP);
        excelTableHeader.append(COLUMN_COMMAND_POPUP);
        excelTableHeader.append(COLUMN_COMMAND_REFA);

        QMap<QString, int> columnStatus;

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
                            if (itr->contains(value.toString().trimmed())) {
                                columnStatus.insert(*itr, col);
                            }
                        }
                    } else {
                        COMMON_WARN("failed to read excel content");
                    }
                }
            }
            ++frow;
        }

        if (columnStatus.size() <= 0) {
            COMMON_WARN("read excel failed");
            return false;
        }

        int currIndex = 1;
        QString lastType = "";
        QByteArray lastGroup = "";

        std::shared_ptr<RCCommandGroup> groupCommand = nullptr;

        for (int row = frow; row <= lrow; row++) {
            std::shared_ptr<RCCommandItem> dataElem(new RCCommandItem());
            QVariant type = currSheet->read(row, columnStatus.find(COLUMN_COMMAND_TYPE).value());
            QVariant index = currSheet->read(row, columnStatus.find(COLUMN_COMMAND_SEQ).value());
            if (index.isValid()) {
                dataElem->setIndex(index.toInt());
            }

            QVariant name = currSheet->read(row, columnStatus.find(COLUMN_COMMAND_NAME).value());
            if (name.isValid()) {
                dataElem->setName(name.toByteArray());
            }

            QVariant popup = currSheet->read(row, columnStatus.find(COLUMN_COMMAND_POPUP).value());
            if (name.isValid()) {
                dataElem->setPopup(popup.toInt());
            }

            QVariant grup = currSheet->read(row, columnStatus.find(COLUMN_COMMAND_GROUP).value());
            if (grup.isValid()) {
                lastGroup = grup.toByteArray();
                dataElem->setGroup(lastGroup);
            } else { // using last group index
                dataElem->setGroup(lastGroup);
            }

            QVariant refA = currSheet->read(row, columnStatus.find(COLUMN_COMMAND_REFA).value());
            if (name.isValid()) {
                dataElem->setRefAdjust(refA.toString());
            }

            QVariant hex = currSheet->read(row, columnStatus.find(COLUMN_COMMAND_CODE).value());
            if (hex.isValid()) {
                bool ok;
                int decimal = hex.toString().trimmed().toInt(&ok, 16);
                dataElem->setFeed(decimal);
            }

            if (lastType.isEmpty() || (type.isValid() && !lastType.contains(type.toString()))) {
                if (listModel != nullptr && groupCommand != nullptr) {
                    listModel->command_group.append(groupCommand);
                    listModel->operate_cmds.append(groupCommand.get());
                    QByteArray frameArr = protoFrame.toLocal8Bit();
                    QByteArray groupArr = groupCommand->groupName().toLocal8Bit();
                    COMMON_WARN("control {}, group {}", frameArr.toStdString(),
                                groupArr.toStdString());
                }

                if (enableCmdExt) {
                    groupCommand = std::shared_ptr<RCCommandGroupExt>(new RCCommandGroupExt());
                    groupCommand->setGroupName(type.toString());
                    lastType = type.toString();
                } else {
                    groupCommand = std::shared_ptr<RCCommandGroup>(new RCCommandGroup());
                    groupCommand->setGroupName(type.toString());
                    lastType = type.toString();
                }
            }

            if (all_command_map.find(dataElem->feed()) == all_command_map.end()) {
                all_command_map.insert(dataElem->feed(), dataElem);
            } else {
                ext_command_map.insert(dataElem->feed(), dataElem);
                COMMON_WARN("control command same id {} ", dataElem->feed());
            }

            groupCommand->insert(dataElem);
        }

        if (listModel != nullptr && groupCommand != nullptr) {
            listModel->command_group.append(groupCommand);
            listModel->operate_cmds.append(groupCommand.get());
        }
    }

    return true;
}

void ControlAdjustVerify::setExpect(const QString &name, uint8_t seq, uint8_t veh, int row,
                                    int expect) {
    QDateTime utc_time = QDateTime::currentDateTimeUtc();
    last_upt = utc_time.currentMSecsSinceEpoch();
    during = last_upt - last_upt;
    expect_name = name;
    expect_result = expect;
    expect_veh = veh;
    verify_row = row;
    command_seq = seq;
}

//网络最小延迟周期
#if defined(AFC21_PROJECT_CONF)
#define MINI_DATALINKER_DELAY 300
#else
#define MINI_DATALINKER_DELAY 400
#endif

void ControlAdjustVerify::setFeedback(int16_t ctrl, int16_t adjust, uint8_t seq) {
    if (++print_freq == 3) {
        COMMON_INFO("telemetry data, control code {}, adjust code {}, frame no {}, expect frame {}",
                    ctrl, adjust, seq, command_seq);
    }

    feedback_adjust = adjust;
    feedback_control = ctrl;
    feedback_seq = seq;
}

bool ControlAdjustVerify::delayTimeout() {
    QDateTime utc_time = QDateTime::currentDateTimeUtc();
    qint64 cur = utc_time.currentMSecsSinceEpoch();
    if (cur - last_upt > MINI_DATALINKER_DELAY) {
        return true;
    }

    return false;
}

bool ControlAdjustVerify::isTimeout(uint32_t timeout) {
    QDateTime utc_time = QDateTime::currentDateTimeUtc();
    qint64 cur = utc_time.currentMSecsSinceEpoch();
    if (cur - last_upt > timeout) {
        during = cur - last_upt;
        if (control != nullptr) {
            control->setWorking(false);
        }
        if (adjusts != nullptr) {
            adjusts->setSelectId(-1);
            adjusts->setWorking(false);
        }
        expect_name = transToUnicode("指令超时");
        return true;
    }

    return false;
}

int ControlAdjustVerify::isExpectValue(AdjustControl *verify, bool seqcheck) {
    if (delayTimeout()) {
        if (!seqcheck) {
            return isExpectValue(verify);
        }
        if (feedback_seq == command_seq) {
            return isExpectValue(verify);
        }
    }

    return 0;
}

int ControlAdjustVerify::isExpectValue(AdjustControl *verify) {
    if ((control != nullptr) && (feedback_control >= 0)) {
        if ((expect_result & 0xff) == (feedback_control & 0xff)) {
            control->setWorking(false);
            return 0x01;
        }

        RCCommandItem *item = nullptr;
        if (verify != nullptr) {
            item = verify->getControlByFeedback(feedback_control);
        }

        if (item != nullptr) {
            expect_name = item->name();
            control->setWorking(false);
            return 0x0f;
        } else {
            expect_name = transToUnicode("无效指令");
            control->setWorking(false);
            return 0x02;
        }
    }

    if ((adjusts != nullptr) && (feedback_adjust >= 0)) {
        if ((expect_result & 0xff) == (feedback_adjust & 0xff)) {
            adjusts->setSelectId(-1);
            adjusts->setWorking(false);
            adjusts->setSelectSub(-1);
            return true;
        }

        RCAdjustItem *item = nullptr;
        if (verify != nullptr) {
            item = verify->getAdjustByfeedback(feedback_adjust);
        }

        if (item != nullptr) {
            expect_name = item->name();
            adjusts->setSelectId(-1);
            adjusts->setWorking(false);
            return 0x0f;
        } else {
            adjusts->setSelectId(-1);
            adjusts->setWorking(false);
            expect_name = transToUnicode("无效指令");
            return 0x02;
        }
    }

    return 0;
}
