#include "GcsDataManager.h"
#include "SimpleSerialListener.h"
#include "log_category.h"
#include <QApplication>
#include <QDateTime>
#include <QFile>
#include <QJsonDocument>
#include <QJsonObject>
#include <QUdpSocket>
#include <chrono>
#include <filesystem>
#include <uv.h>

DataRecordReadWrite::~DataRecordReadWrite() { closeFile(); }

int64_t DataRecordItem::currentTimeMs() {
    typedef std::chrono::milliseconds ms_precision;
    typedef std::chrono::time_point<std::chrono::system_clock, ms_precision> time_precision;
    time_precision tp = std::chrono::time_point_cast<ms_precision>(std::chrono::system_clock::now());
    return tp.time_since_epoch().count();
}

DataBlockBody::DataBlockBody() : buffer(nullptr) { memset(&header, 0, sizeof(DataBlockHeader)); }

DataBlockBody::DataBlockBody(const uint8_t *data, int datalen) {
    buffer = std::shared_ptr<uint8_t>(new uint8_t[datalen]);
    memcpy(buffer.get(), data, datalen);
    header.length = datalen;
}

void DataRecordReadWrite::writeData(std::shared_ptr<DataBlockBody> indata) { dataQueue.enqueue(indata); }

int DataRecordReadWrite::countQueue() { return dataQueue.size_approx(); }

void DataRecordReadWrite::releaseResource() {
    std::shared_ptr<DataBlockBody> msg = nullptr;
    while (dataQueue.try_dequeue(msg) && msg != nullptr) {
        msg = nullptr;
    }
}

std::shared_ptr<DataBlockBody> DataRecordReadWrite::popBlock() {
    std::shared_ptr<DataBlockBody> data = nullptr;
    dataQueue.try_dequeue(data);
    return data;
}

void DataRecordReadWrite::jumpFileData(uint32_t pos) {}

int DataRecordReadWrite::readDataFromFile(uint8_t counts) {
    if (_readhandle == nullptr) {
        return -1;
    }

    int rtn = 0;
    for (uint8_t i = 0; i < counts; i++) {
        std::shared_ptr<DataBlockBody> data(new DataBlockBody());
        char *add = (char *) &data->header;
        int sz = sizeof(DataBlockHeader);
        int gx = std::fread(add, 1, sz, _readhandle);
        if (gx == sz) {
            sz = data->header.length;
            if (sz > 0) {
                data->buffer = std::shared_ptr<uint8_t>(new uint8_t[sz]);
                std::fread(data->buffer.get(), 1, sz, _readhandle);
                dataQueue.enqueue(data);
                rtn++;
            }
        }
    }

    return rtn;
}

void DataRecordReadWrite::openFile(const QString &path, const QString &file) {
    std::filesystem::path path2("RawData/" + path.toStdString());
    datafile = QString("%1/%2.dat").arg(QString::fromStdString(path2.string())).arg(file);
    _wfopen_s(&_readhandle, datafile.toStdWString().c_str(), L"rb+");
    if (_readhandle == nullptr) {
        COMMON_WARN("file cannot open {}", datafile.toStdString());
        return;
    }
}

int DataRecordReadWrite::writeDataToFile() {
    int rtn = 0;

    std::shared_ptr<DataBlockBody> msg = nullptr;
    if ((+dataQueue.size_approx() > 0) && dataQueue.try_dequeue(msg) && (msg != nullptr)) {
        if (_filehandle != nullptr) {
            uint8_t *datas = msg.get()->buffer.get();
            fwrite(&msg.get()->header, sizeof(DataBlockHeader), 1, _filehandle);
            fwrite(datas, msg.get()->header.length, 1, _filehandle);
        }

        rtn += msg.get()->header.length;
    }

    return rtn;
}

void DataRecordReadWrite::createFile(const QString &path, const QString &file) {
    std::filesystem::path path1("RawData");
    if (!std::filesystem::exists(path1)) {
        std::filesystem::create_directories(path1);
    }

    std::filesystem::path path2("RawData/" + path.toStdString());
    if (!std::filesystem::exists(path2)) {
        std::filesystem::create_directories(path2);
    }

    datafile = QString("%1/%2.dat").arg(QString::fromStdString(path2.string())).arg(file);
    _wfopen_s(&_filehandle, datafile.toStdWString().c_str(), L"wb+");
    if (_filehandle == nullptr) {
        COMMON_WARN("Could not to create file {}", datafile.toStdString());
        return;
    }
}

void DataRecordReadWrite::closeFile() {
    if (_filehandle != nullptr) {
        fclose(_filehandle);
        _filehandle = nullptr;
    }

    if (_readhandle != nullptr) {
        fclose(_readhandle);
        _readhandle = nullptr;
    }
}

GcsDataManager::GcsDataManager(QObject *parent) {
    data_record_item = std::make_shared<DataRecordItem>();
    rw_data_flag = 0;
    slide_value = 0;
    play_speed = 1.0;
}

void GcsDataManager::startRecord() {
    QDateTime file_time = QDateTime::currentDateTime();
    QDate cur = file_time.date();
    QTime ctm = file_time.time();
    QString datetm = QString("%1-%2-%3").arg(cur.year()).arg(cur.month()).arg(cur.day());
    QString timestr = QString("%1-%2-%3").arg(ctm.hour()).arg(ctm.minute()).arg(ctm.second());
    _startTime = DataRecordItem::currentTimeMs();

    rw_data_flag = DataWrite;

    data_record_item->recordTime(datetm + "-" + timestr);
    data_record_item->startTm(_startTime);
    data_record_item->index(cache_record_data.count());

    data_thread = std::thread([=]() {
        telemetry_rw_queue.createFile("telemetry", datetm + "-" + timestr);
        ctrladjust_rw_queue.createFile("ctrladjust", datetm + "-" + timestr);
        weather_queue.createFile("media", datetm + "-" + timestr);
        media_queue.createFile("weather", datetm + "-" + timestr);
        emit rwStateChanged();

        while (rw_data_flag == DataWrite) {
            std::this_thread::sleep_for(std::chrono::milliseconds(10));
            int valu = telemetry_rw_queue.writeDataToFile();
            valu += ctrladjust_rw_queue.writeDataToFile();
            valu += weather_queue.writeDataToFile();
            valu += media_queue.writeDataToFile();
        }

        telemetry_rw_queue.closeFile();
        ctrladjust_rw_queue.closeFile();

        weather_queue.closeFile();
        media_queue.closeFile();

        telemetry_rw_queue.releaseResource();
        ctrladjust_rw_queue.releaseResource();
        weather_queue.releaseResource();
        media_queue.releaseResource();

        COMMON_INFO("close raw data thread");
    });
}

void GcsDataManager::writeData(ConfigController *config, int chl, const void *sock, const uint8_t *data, int datalen) {
    if (rw_data_flag != DataWrite) {
        return;
    }

    if (config == nullptr || sock == nullptr) {
        COMMON_WARN("unknown input param");
        return;
    }

    std::shared_ptr<DataBlockBody> msg = std::make_shared<DataBlockBody>(data, datalen);
    assert(msg != nullptr);
    if (msg == nullptr) {
        COMMON_WARN("unknown allocate data");
        return;
    }

    msg->header.channel = chl;
    msg->header.offset = DataRecordItem::currentTimeMs() - _startTime;
    sockaddr_in *sockin = (sockaddr_in *) (sock);

    if (chl == ChannelSelection::SlaveNetwork) {
        NetworkNodeConfig *netslave = config->slaveNetNode();
        ChannelConfig &configslave = netslave->config();

        int port = ntohs(sockin->sin_port);
        msg->header.port = sockin->sin_port;
        msg->header.address = sockin->sin_addr.S_un.S_addr;

        if (port == configslave.multiRxPort) {
            telemetry_rw_queue.writeData(msg);
            data_record_item->telemetry(true);

        } else if (port == configslave.multiTxPort) {
            ctrladjust_rw_queue.writeData(msg);
            data_record_item->contrlAdjust(true);
        }

    } else if (chl == ChannelSelection::MainNetwork) {
        NetworkNodeConfig *netmain = config->mainNetNode();
        ChannelConfig &configmain = netmain->config();

        int port = ntohs(sockin->sin_port);
        msg->header.port = sockin->sin_port;
        msg->header.address = sockin->sin_addr.S_un.S_addr;

        if (port == configmain.multiRxPort) {
            telemetry_rw_queue.writeData(msg);
            data_record_item->telemetry(true);

        } else if (port == configmain.multiTxPort) {
            ctrladjust_rw_queue.writeData(msg);
            data_record_item->contrlAdjust(true);
        }

    } else if (chl == ChannelSelection::SerialNetwork) {
        telemetry_rw_queue.writeData(msg);
        data_record_item->telemetry(true);

    } else {
        qDebug() << "data " << datalen << chl;
    }
}

void GcsDataManager::writeToJson(DataRecordItem *item) {

    assert(item != nullptr);
    if (item == nullptr) {
        return;
    }

    std::filesystem::path path1("RawData");
    if (!std::filesystem::exists(path1)) {
        std::filesystem::create_directories(path1);
    }

    std::string mx = path1.string() + "//records.json";
    QFile file(QString::fromStdString(mx.c_str()));
    if (!file.open(QIODevice::Append | QIODevice::Text)) {
        file.open(QIODevice::WriteOnly | QIODevice::Text);
    }

    QJsonObject jsonObject;
    jsonObject["telemetry"] = item->telemetry();
    jsonObject["media"] = item->media();
    jsonObject["weather"] = item->weather();
    jsonObject["contrlAdjust"] = item->contrlAdjust();
    jsonObject["index"] = item->index();
    jsonObject["during"] = item->during();
    jsonObject["startTm"] = item->startTm();
    jsonObject["recordTime"] = item->recordTime();
    jsonObject["telemetry"] = item->telemetry();

    QJsonDocument doc(jsonObject);
    QByteArray add = doc.toJson(QJsonDocument::Compact).replace("\n", "");
    file.write(add);
    file.write("\n");
    file.close();
}

void GcsDataManager::setPlaySpeed(double val) { play_speed = val; }

void GcsDataManager::setSlideValue(int val) {
    slide_value = val;
    emit slideValueChanged(val);
}

void GcsDataManager::stopReplay() {
    if (rw_data_flag != 0) {
        rw_data_flag = 0;
        if (data_thread.joinable()) {
            data_thread.join();
        }
        emit rwStateChanged();
    }
}

bool GcsDataManager::replayData(std::shared_ptr<DataBlockBody> data, int64_t last, QUdpSocket &sock) {
    if (data == nullptr) {
        return true;
    }

    int64_t curr = DataRecordItem::currentTimeMs();
    if ((curr - last) * play_speed > data->header.offset) {
        struct sockaddr_in sockaddr;
        sockaddr.sin_addr.S_un.S_addr = data->header.address;
        char *ip = inet_ntoa(sockaddr.sin_addr);
        QHostAddress group(QString::fromStdString(ip));
        int hport = ntohs(data->header.port);
        sock.writeDatagram((char *) data->buffer.get(), data->header.length, group, hport);
        return true;
    }

    return false;
}

bool GcsDataManager::startReplay(QString file, int selectRow) {
    if (rw_data_flag != 0) {
        COMMON_WARN("replaying...");
        return false;
    }
    rw_data_flag = DataReplay;
    cur_select_row = selectRow;
    data_thread = std::thread([=]() {
        if (_hasTelemetry) {
            telemetry_rw_queue.openFile("telemetry", file);
        }
        if (_hasCtlAdjust) {
            ctrladjust_rw_queue.openFile("ctrladjust", file);
        }
        if (_hasMedia) {
            media_queue.openFile("media", file);
        }
        if (_hasWeather) {
            weather_queue.openFile("weather", file);
        }

        curr_play_loc = 0;
        std::shared_ptr<DataBlockBody> telemetry = nullptr;
        std::shared_ptr<DataBlockBody> ctrladjust = nullptr;
        std::shared_ptr<DataBlockBody> weather = nullptr;
        std::shared_ptr<DataBlockBody> media = nullptr;
        int telemetryend = false;
        int ctrladjustend = false;
        int weatherend = false;
        int mediaend = false;

        emit rwStateChanged();

        int countReplay = 0;
        QUdpSocket udpSocket;
        std::shared_ptr<DataRecordItem> rowItem = cache_record_data.at(cur_select_row);
        int64_t lasttime = DataRecordItem::currentTimeMs();

        while (rw_data_flag == DataReplay) {
            int64_t moredata = DataRecordItem::currentTimeMs();
            if (_hasTelemetry && telemetry_rw_queue.countQueue() <= 1) {
                telemetryend = telemetry_rw_queue.readDataFromFile(50);
            }
            if (_hasCtlAdjust && ctrladjust_rw_queue.countQueue() <= 1) {
                ctrladjustend = ctrladjust_rw_queue.readDataFromFile(50);
            }
            if (_hasWeather && weather_queue.countQueue() <= 1) {
                weatherend = weather_queue.readDataFromFile(50);
            }
            if (_hasMedia && media_queue.countQueue() <= 1) {
                mediaend = media_queue.readDataFromFile(50);
            }

            if (replayData(telemetry, lasttime, udpSocket)) {
                telemetry = telemetry_rw_queue.popBlock();
                if (countReplay++ % 10 == 0) {
                    COMMON_INFO("replay count {}", countReplay);
                }
            }

            if (replayData(ctrladjust, lasttime, udpSocket)) {
                ctrladjust = ctrladjust_rw_queue.popBlock();
            }

            if (replayData(weather, lasttime, udpSocket)) {
                weather = weather_queue.popBlock();
            }

            if (replayData(media, lasttime, udpSocket)) {
                media = media_queue.popBlock();
            }

            int64_t curr = DataRecordItem::currentTimeMs();
            if (curr - lasttime > 1000) { // each 1 second
                int step = (curr - lasttime) * play_speed;
                setSlideValue(std::ceil(step / rowItem->during()));
            } else if (curr - moredata < 10) { // each 10 milliseconds
                std::this_thread::sleep_for(std::chrono::milliseconds(10));
            }

            if (telemetry_rw_queue.countQueue() <= 0 && ctrladjust_rw_queue.countQueue() <= 0 &&
                weather_queue.countQueue() <= 0 && media_queue.countQueue() <= 0) {
                break;
            }
        }

        telemetry_rw_queue.closeFile();
        ctrladjust_rw_queue.closeFile();
        weather_queue.closeFile();
        media_queue.closeFile();

        emit dataThreadExited();
    });

    return true;
}

void GcsDataManager::pauseReplay() {}

void GcsDataManager::loadDataFile() {
    std::filesystem::path path1("RawData");
    if (!std::filesystem::exists(path1)) {
        std::filesystem::create_directories(path1);
    }

    std::string mx = path1.string() + "//records.json";

    QFile file(QString::fromStdString(mx.c_str()));
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        return;
    }

    QByteArray data;
    data = file.readAll();
    data.replace(" ", "");
    file.close();

    QStringList jsonList = QString(data).split("\n");
    for (const QString &jsonStr : jsonList) {
        std::shared_ptr<DataRecordItem> item = std::make_shared<DataRecordItem>();
        QJsonDocument doc = QJsonDocument::fromJson(jsonStr.toUtf8());
        QJsonObject obj = doc.object();
        item->telemetry(obj["telemetry"].toBool());
        item->media(obj["media"].toBool());
        item->weather(obj["weather"].toBool());
        item->contrlAdjust(obj["contrlAdjust"].toBool());
        item->index(obj["index"].toInt());
        item->during(obj["during"].toInt());
        item->startTm(obj["startTm"].toInt());
        item->recordTime(obj["recordTime"].toString());
        cache_record_data.append(item);
        visual_datalist.append(item.get());
    }
}

void GcsDataManager::stopRecord() {
    if (rw_data_flag != 0) {
        rw_data_flag = 0;

        int64_t last = DataRecordItem::currentTimeMs();
        data_record_item->during(last - data_record_item->startTm());
        cache_record_data.append(data_record_item);
        visual_datalist.append(data_record_item.get());
        writeToJson(data_record_item.get());

        emit rwStateChanged();

        if (data_thread.joinable()) {
            data_thread.join();
        }
    }
}

GcsDataManager::~GcsDataManager() { stopRecord(); }

QVariant DataRecordItem::columnValue(int column) {
    switch (column) {
    case 0:
        return index();
    case 1:
        return recordTime();
    case 2: {
        int value = during() / 1000;
        int hour = value / 3600;
        int mill = (value - hour * 3600) / 60;
        int second = (value - hour * 3600 - mill * 60);
        char buffer[16] = {0};
        sprintf(buffer, "%02d:%02d:%02d", hour, mill, second);
        return QString::fromStdString(buffer);
    }
    case 3:
        return telemetry();
    case 4:
        return contrlAdjust();
    case 5:
        return media();
    case 6:
        return weather();
    case 7:
        return QString("操作");
    }
    return QVariant();
}

DataRecordItem::DataRecordItem() {
    _during = 0;
    _index = 0;
    _media = false;
    _weather = false;
    _contrlAdjust = false;
    _telemetry = false;
    _startTm = currentTimeMs();
    QDateTime dateTime = QDateTime::currentDateTime();      //获取系统当前的时间
    _recordTime = dateTime.toString("yyyy-MM-dd hh:mm:ss"); //格式化时间
}
