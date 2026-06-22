#include "facttablemodel.h"
#include "factmetadata.h"
#include "log_category.h"
#include "prototelemetry.h"
#include "stringtranslate.h"
#include <QDateTime>
#include <filesystem>

FactTableModel::FactTableModel(QDateTime datetime, QObject *parent)
    : QAbstractTableModel(), _file_time(datetime) {
    _headData.append(transToUnicode("code"));
    _headData.append(transToUnicode("name"));
    _headData.append(transToUnicode("value"));
    _headData.append(transToUnicode("origin"));
    _checked = false;
    _code = 0;
}

FactTableModel::~FactTableModel() {}

std::shared_ptr<WarnElemNode> FactTableModel::warnData(WarnConfig *warnConf, FactTableModel *table,
                                                       QString &timestamp, WarningLevel warn,
                                                       int priority, const QString &name) {
    std::shared_ptr<WarnElemNode> warnNode(new (WarnElemNode));
    QString warnStr = warnConf->_warnString[warn]; ///
    warnNode->group(warnStr.isEmpty() ? name : warnStr);
    warnNode->timestamp(timestamp);
    warnNode->frame(table->name());
    warnNode->priority(priority);
    warnNode->name(name);
    warnNode->level(warn);
    return warnNode;
}

void FactTableModel::decodeWarnData(WarnConfig *warnConf, FactTableModel *table, QString &timestamp,
                                    WarningLevel warn, CheckWarnCallback first, int chl) {

    int priority = warnConf->_priorityValue[warn];
    int checkVal = warnConf->_checkValue[warn];
    int checkCnt = warnConf->_checkCount[warn];
    int refRow = warnConf->_referRow[warn];
    int refOpt = warnConf->_referOpt[warn];

    FactMetaData *frameData = table->loadFact(warnConf->_frameItem);
    assert(frameData != nullptr);
    if (frameData == nullptr) {
        COMMON_WARN("unknow frame {}", warnConf->_frameItem);
        return;
    }

    if (warnConf->_referFrame) {
        COMMON_WARN("unknow refer frame {}", warnConf->_referFrame);
        return;
    }

    if (refOpt & 0xf) {
        FactMetaData *frameData2 = table->loadFact(warnConf->_frameItem - (refRow & 0xff));
        assert(frameData2 != nullptr);

        if (frameData2 == nullptr || ((refRow & 0xff) == 0)) {
            COMMON_WARN("unknow frame row {} => {}", warnConf->_frameItem, refRow);
            return;
        }

        int val1 = frameData->rawValue(chl);
        int val2 = frameData2->rawValue(chl);

        int curval = 0;
        if (refOpt == AndOperate) {
            curval = val1 & val2;
        } else if (refOpt == NotOperate) {
            curval = val1 ^ val2;
        } else if (refOpt == AddOperate) {
            if (warnConf->_moreOrLess == EqualValue) {
                if (val1 == (checkVal & 0xff) || val2 == (checkVal & 0xff)) {
                    first(warnData(warnConf, table, timestamp, warn, priority, frameData->name()));
                }
            } else if (warnConf->_moreOrLess == LessEqual) {
                if ((val1 <= (checkVal & 0xff)) || val2 <= (checkVal & 0xff)) {
                    first(warnData(warnConf, table, timestamp, warn, priority, frameData->name()));
                }
            } else if (warnConf->_moreOrLess == GreateEqual) {
                if ((val1 >= (checkVal & 0xff)) || val2 >= (checkVal & 0xff)) {
                    first(warnData(warnConf, table, timestamp, warn, priority, frameData->name()));
                }
            }
            return;
        } else if (refOpt == EachOperate) {
            if (warnConf->_moreOrLess == EqualValue) {
                if (val1 == (checkVal & 0xff) || val2 == (checkVal & 0xff)) {
                    first(warnData(warnConf, table, timestamp, warn, priority, frameData->name()));
                }
            } else if (warnConf->_moreOrLess == LessEqual) {
                if ((val1 <= (checkVal & 0xff)) || val2 <= (checkVal & 0xff)) {
                    first(warnData(warnConf, table, timestamp, warn, priority, frameData->name()));
                }
            } else if (warnConf->_moreOrLess == GreateEqual) {
                if ((val1 >= (checkVal & 0xff)) || val2 >= (checkVal & 0xff)) {
                    first(warnData(warnConf, table, timestamp, warn, priority, frameData->name()));
                }
            }
            return;
        }
        if (warnConf->_moreOrLess == EqualValue) {
            if (curval == (checkVal & 0xff)) {
                first(warnData(warnConf, table, timestamp, warn, priority, frameData->name()));
            }
        } else if (warnConf->_moreOrLess == LessEqual) {
            if ((curval <= (checkVal & 0xff))) {
                first(warnData(warnConf, table, timestamp, warn, priority, frameData->name()));
            }
        } else if (warnConf->_moreOrLess == GreateEqual) {
            if ((curval >= (checkVal & 0xff))) {
                first(warnData(warnConf, table, timestamp, warn, priority, frameData->name()));
            }
        }
    } else if (refOpt == 0) {
        int val = frameData->rawValue(chl);
        if (warnConf->_moreOrLess == EqualValue) {
            for (int mx = 0; mx < checkCnt; mx++) {
                int valm = (checkVal >> (8 * mx)) & 0xff;
                if (val == valm && first) {
                    first(warnData(warnConf, table, timestamp, warn, priority, frameData->name()));
                }
            }
        } else if (warnConf->_moreOrLess == LessEqual) {
            for (int mx = 0; mx < checkCnt; mx++) {
                int valm = (checkVal >> (8 * mx)) & 0xff;
                if (val <= valm && first) {
                    first(warnData(warnConf, table, timestamp, warn, priority, frameData->name()));
                }
            }
        } else if (warnConf->_moreOrLess == GreateEqual) {
            for (int mx = 0; mx < checkCnt; mx++) {
                int valm = (checkVal >> (8 * mx)) & 0xff;
                if (val >= valm && first) {
                    first(warnData(warnConf, table, timestamp, warn, priority, frameData->name()));
                }
            }
        } else if (warnConf->_moreOrLess == ColorValue) {
            if ((frameData->acolor() == warn) && first) {
                first(warnData(warnConf, table, timestamp, warn, priority, frameData->name()));
            }
        }
    }
}

QString FactTableModel::currTimestamp(int chl) const {
    auto mx = _fileHandlers.find(chl);
    if (mx != _fileHandlers.end()) {
        return mx.value()->curTimestamp;
    }

    return QString();
}

void FactTableModel::signalModel(int chl) {
    for (int x = 0; x < _datas.size(); x++) {
        _datas[x]->notifyDisplay(chl);
    }
}

void FactTableModel::flushModelData(CheckWarnCallback first, int chl) {
    QString timestamp = currTimestamp(chl);
    for (int x = 0; x < _datas.size(); x++) {
        if (_datas[x]->isWarnUpdated(chl)) {
            WarnConfig *config = _datas[x]->warnConfig();
            if ((config != nullptr) && (config->_errorRow > 0)) {
                if (config && (config->_errorRow & (1 << WarningLevel::LevelOne))) {
                    decodeWarnData(config, this, timestamp, WarningLevel::LevelOne, first, chl);
                }
                if (config && (config->_errorRow & (1 << WarningLevel::LevelTwo))) {
                    decodeWarnData(config, this, timestamp, WarningLevel::LevelTwo, first, chl);
                }
                if (config && (config->_errorRow & (1 << WarningLevel::LevelThree))) {
                    decodeWarnData(config, this, timestamp, WarningLevel::LevelThree, first, chl);
                }
                if (config && (config->_errorRow & (1 << WarningLevel::LevelFour))) {
                    decodeWarnData(config, this, timestamp, WarningLevel::LevelFour, first, chl);
                }
            }
        }
    }
}

int FactTableModel::rowCount(const QModelIndex &parent) const {
    if (parent.isValid()) {
        return 0;
    } else {
        return _datas.size();
    }
}

int FactTableModel::columnCount(const QModelIndex &parent) const {
    if (parent.isValid()) {
        return 0;
    } else {
        return _headData.size();
    }
}

FactTableModel::FileCache::FileCache() { memset(_cache, 0, sizeof(_cache)); }

// FileCache 是 FactTableModel 的嵌套类
void FactTableModel::FileCache::closeFile() {
    //ofile：FileCache 类的成员变量，C++ 标准库的 std::ofstream（文件输出流）
    if (ofile.is_open()) {
        ofile.close();
    }
}

bool FactTableModel::FileCache::createFile(const QString &filename) {
    QByteArray groupArr = filename.toLocal8Bit();
    std::wstring datafile = filename.toStdWString();

    ofile.open(datafile.c_str());
    if (!ofile.is_open()) {
        COMMON_WARN("Could not to create file {}", groupArr.toStdString());
        return false;
    }

    ofile.rdbuf()->pubsetbuf(_cache, sizeof(_cache));
    COMMON_INFO("create data file {}", groupArr.toStdString());
    return true;
}

void FactTableModel::resetFilename(int channel) {
    QDate cur = _file_time.date();
    QTime ctm = _file_time.time();
    QString datetm = QString("Datas/%1-%2-%3").arg(cur.year()).arg(cur.month()).arg(cur.day());
    QString timestr = QString("%1-%2-%3").arg(ctm.hour()).arg(ctm.minute()).arg(ctm.second());
    QString newfile = QString("[%1]-%2.txt").arg(channel).arg(name());
    std::filesystem::path path = datetm.toStdString();
    if (!std::filesystem::exists(path)) {
        std::filesystem::create_directories(path);
        COMMON_WARN("create folder {}", datetm.toStdString());
    }
    QString subfilder = datetm + "/" + timestr;
    path = subfilder.toStdString();
    if (!std::filesystem::exists(path)) {
        std::filesystem::create_directories(path);
        COMMON_WARN("create folder {}", subfilder.toStdString());
    }

    std::shared_ptr<FileCache> fileHandle(new FileCache());
    QString filename = subfilder + "/" + newfile;
    if (fileHandle->createFile(filename)) {
        _fileHandlers.insert(channel, fileHandle);
        fileHandle->ofile << "UTC\t";
        for (int i = 0; i < _datas.count(); i++) {
            fileHandle->ofile << _datas[i].get()->name().toStdString();
            fileHandle->ofile << "\t";
        }
        fileHandle->ofile << "channel\t";
        fileHandle->ofile << "systime\t";
        fileHandle->ofile << "\n";
    }
}

void FactTableModel::closeFileSave() {
    //FactTableModel 的成员变量,作用：标记 “停止文件保存”，通知后台写入线程（若有）停止向文件写数据
    stop_flag = true;
    Sleep(30); // delay
    //_fileHandlers.begin() 返回指向第一个文件处理器的迭代器
    auto mx = _fileHandlers.begin();
    while (mx != _fileHandlers.end()) {
        mx.value()->closeFile();
        ++mx;
    }
    _fileHandlers.clear();  //清空 _fileHandlers 容器
}

void FactTableModel::dataFileSave(float utc, int channel, int select, int timestamp) {
    if (stop_flag) { ///停止标记
        return;
    }

    QDateTime dtm = QDateTime::currentDateTime();
    QString tmStr = dtm.toString("yyyy-MM-dd-hh-mm-ss");

    auto mx = _fileHandlers.find(channel);
    if (mx == _fileHandlers.end() || mx.value() == nullptr) {
        return;
    }

    if (utc <= 0) {
        /// 外部已经计算好时间
        if (_timestamp > 0) {
            /// 通过名称来检索时间信息的位置
            mx.value()->curTimestamp =
                _datas[_timestamp]->formatTimestamp(channel, select, _biglittle);
        } else {
            /// 通过ID来检索时间信息的位置
            if (_datas.count() > timestamp) {
                mx.value()->curTimestamp =
                    _datas[timestamp]->formatTimestamp(channel, select, _biglittle);
            }
        }
    }

    if (utc > 0) {
        // c21项目的UTC时间从外部输入
        char buffer[32] = {0};
        std::snprintf(buffer, 32, "%.3f", utc);
        mx.value()->ofile << buffer << "\t";
    } else {
        mx.value()->ofile << mx.value()->counts() << "\t";
    }

    for (int i = 0; i < _datas.count(); i++) {
        mx.value()->ofile << _datas[i].get()->display(channel).toStdString();
        mx.value()->ofile << "\t";
    }

    mx.value()->ofile << channel << "\t" << tmStr.toStdString() << "\t";
    mx.value()->ofile << "\n";
}

QVariant FactTableModel::data(const QModelIndex &index, int role) const {
    if (!index.isValid()) {
        return QVariant();
    }
    if (role == Qt::DisplayRole) {
        std::shared_ptr<FactMetaData> dataElem = nullptr;
        if ((dataElem = _datas[index.row()]) == nullptr) {
            return QVariant();
        }

        switch (index.column()) {
        case 0: {
            return dataElem->itemNum();
        }
        case 1: {
            QString unit = dataElem->units();
            if (unit.isEmpty() || unit.compare("-") == 0) {
                return dataElem->name();
            } else {
                return QString("%1 [%2]").arg(dataElem->name(), dataElem->units());
            }
        }
        case 2: {
            return dataElem->display(_defChl);
        }
        case 3: {
            return dataElem->hexValue(_defChl);
        }
        }
    } else if (role == Qt::TextAlignmentRole) {
        return Qt::AlignCenter;
    }

    return QVariant();
}

QVariant FactTableModel::headerData(int section, Qt::Orientation orientation, int role) const {
    if (orientation == Qt::Horizontal) {
        if (role == Qt::DisplayRole) {
            return _headData.at(section);
        }
    }

    return QAbstractTableModel::headerData(section, orientation, role);
}

FactMetaData *FactTableModel::loadFact(int i) const {
    if (i < 0 || i >= _datas.count()) {
        return const_cast<FactMetaData *>(&_nullData);
    }

    return static_cast<FactMetaData *>(_datas[i].get());
}

FactMetaData *FactTableModel::at(int i) const { return loadFact(i); }

FactMetaData *FactTableModel::find(const QString &name) const {
    for (int i = 0; i < _datas.count(); i++) {
        if (_datas[i].get()->name().contains(name)) {
            return _datas[i].get();
        }
    }
    return nullptr;
}

void FactTableModel::addFactMeta(int id, std::shared_ptr<TransMetaData> &elem) {
    COMMON_TRACE("add node {} {}", id, elem->groupName().toStdString().c_str());
    ProtoDecoderInfo &protoinfo = GetProtoDecoderInfo();
    QString gname = elem->groupName();
    QString cname = elem->name();

    if (!protoinfo.ProtoTimestampName.isEmpty() && (gname.contains(protoinfo.ProtoTimestampName) ||
                                                    cname.contains(protoinfo.ProtoTimestampName))) {
        _timestamp = _listModel.count();
    }

    _biglittle = protoinfo.DataBigLittleEndian;
    _listModel.append(elem.get());
    _datas.append(elem);
}

QHash<int, QByteArray> FactTableModel::roleNames() const { return {{Qt::DisplayRole, "display"}}; }

int FactTableModel::decoder(const uint8_t *buffer, int size, int chl, ReferDeviceCallback refer) {
    int offset = 0;
    TransMetaData *curr = nullptr;
    for (int i = 0; i < _datas.size(); i++) {
        curr = _datas[i].get();
        if (curr == nullptr) {
            COMMON_INFO("not supported, please to check the definition of proto file");
            continue;
        }
        if (curr->metaDataType() == MetaDataType::stringType) {
            offset += curr->getBitSet().toInt() / 8;
        } else {
            offset += curr->setProtoValue(buffer + offset, 0, chl, refer);
        }
    }
    return offset;
}
