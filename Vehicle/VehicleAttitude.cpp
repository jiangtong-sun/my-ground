#include "VehicleAttitude.h"
#include "log_category.h"
#include "xlsxworksheet.h"

bool DisplayElemNode::attributeMetaData(const FactMetaData *data) {
    connect(data, &FactMetaData::displayChanged, this, &DisplayElemNode::doUpdateQuick);
    attributeData = data;
    return true;
}

bool DisplayElemNode::referenceMetaData(const FactMetaData *data) {
    connect(data, &FactMetaData::rawDisplayChanged, this, &DisplayElemNode::doReferenceSource);
    referenceData = const_cast<FactMetaData *>(data);
    referenceData->notifyRawDisplay();
    return true;
}

void DisplayElemNode::doReferenceSource(const QString & name, int color, int raw) {
    if (referenceData != nullptr) {
        if (raw == 0) { // ?HUANGCHUN
            cacheDatas[1] = cacheDatas[0];
        } else {
            cacheDatas[2] = cacheDatas[0];
        }
    }
}

void DisplayElemNode::doUpdateQuick(QString name, int color) {
    if (attributeData != nullptr) {
        cacheDatas[0] = name.toFloat();
    }
}

void DisplayElemNode::setGroupName(const QString &name) { groupName = name; }

QVariant DisplayElemNode::columnValue(int column) {
    if (column == 0) {
        return groupName;
    } else if (column < 3) {
        return QString::number(cacheDatas[column], 'd', 2);
    }
    return QVariant();
}

std::shared_ptr<CommElemNode> AttributeTable::find(int row) {
    if (orginDatas.size() <= row || row < 0) {
        COMMON_WARN("index out of range {}", row);
        return nullptr;
    }

    return orginDatas.at(row);
}

QVariant AttributeTable::elementData(int row, int col) const {
    if (orginDatas.size() < row || row < 0) {
        return QVariant();
    }

    if (curr_nav_mode == col) {
        QVariant cur = _dataSource.at(row)->columnValue(col);
        return QString("@").append(cur.toString());
    }

    return _dataSource.at(row)->columnValue(col);
}

void AttributeTable::initColumnHeader() {
    QStringList column;
    column.append("");
    column.append(QStringLiteral("主"));
    column.append(QStringLiteral("备"));
    columnSource(column);
}

void AttributeTable::doUpdateTable(const QString & name, int color, int chl) {
    QModelIndex topLeft = createIndex(0, 1);
    QModelIndex bottomRight = createIndex(orginDatas.size(), 4);
    emit dataChanged(topLeft, bottomRight);
}

void AttributeTable::setNavigationMode(int val) { curr_nav_mode = val; }

void AttributeTable::ReadModule(const QXlsx::Worksheet *currSheet, QMap<QString, int> &columnStatus,
                                FactDecoderMgr *decoder, int frow, int lrow) {
    if (currSheet == nullptr) {
        COMMON_WARN("invalid sheet param");
        return;
    }

    QString indexBlock("");
    std::shared_ptr<DisplayElemNode> dataElem = nullptr;
    const FactMetaData *firstMetaData = nullptr;

    for (int row = frow; (row <= lrow); row++) {
        QVariant submodule;
        auto xmx = columnStatus.find(DisplaySubModule);
        if ((xmx != columnStatus.end())) {
            int col = xmx.value();
            submodule = currSheet->read(row, col);
        }

        QVariant frame;
        xmx = columnStatus.find(DisplayRefFrame);
        if (xmx != columnStatus.end()) {
            int col = xmx.value();
            frame = currSheet->read(row, col);
        }

        QVariant frameType;
        xmx = columnStatus.find(DisplayRefFType);
        if (xmx != columnStatus.end()) {
            int col = xmx.value();
            frameType = currSheet->read(row, col);
        }

        QVariant refData;
        xmx = columnStatus.find(DisplayRefData);
        if (xmx != columnStatus.end()) {
            int col = xmx.value();
            refData = currSheet->read(row, col);
        }

        QVariant refIndex;
        xmx = columnStatus.find(DisplayDataIndex);
        if (xmx != columnStatus.end()) {
            int col = xmx.value();
            refIndex = currSheet->read(row, col);
        }

        const FactTableModel *curTable = nullptr;
        if ((decoder != nullptr) && frame.isValid()) {
            curTable = decoder->protoframe(frame.toString());
        }
        if (curTable == nullptr && frameType.isValid()) {
            curTable = decoder->protoframe(frameType.toInt());
        }

        if (submodule.isValid()) {
            if (indexBlock.isEmpty() || indexBlock.compare(submodule.toString()) != 0) {
                dataElem = std::make_shared<DisplayElemNode>();
                indexBlock = submodule.toString();
                dataElem->setGroupName(indexBlock);
                appendRow(dataElem);
            }
        }

        if ((curTable != nullptr) && (curTable->code() != 0)) {
            QString tmp = refData.toString();
            if (refIndex.isValid()) {
                firstMetaData = curTable->loadFact(tmp.trimmed().toInt());
                dataElem->referenceMetaData(firstMetaData);
            } else {
                firstMetaData = curTable->loadFact(tmp.trimmed().toInt());
                dataElem->attributeMetaData(curTable->loadFact(tmp.trimmed().toInt()));
            }
        } else {
            COMMON_INFO("not found fact by {}: {}", frame.toString().toStdString().c_str(),
                        refData.toString().toStdString().c_str());
        }
    }

    connect(firstMetaData, &FactMetaData::rawDisplayChanged, this, &AttributeTable::doUpdateTable);
}
