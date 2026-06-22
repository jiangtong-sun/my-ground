#pragma once

#include "ConfigController.h"
#include "GcsMissionDef.h"
#include "concurrentqueue.h"
#include "gcstreemodel.h"

#include <QObject>
#include <QUdpSocket>

struct DataBlockHeader {
    uint16_t channel = 0;
    uint16_t length = 0;
    uint32_t offset = 0;
    uint32_t address = 0;
    uint16_t port = 0;
    uint16_t space = 0;
};

struct DataBlockBody {
    DataBlockHeader header;
    std::shared_ptr<uint8_t> buffer;

    DataBlockBody();
    DataBlockBody(const uint8_t *data, int datalen);
};

class DataRecordReadWrite {
  public:
    ~DataRecordReadWrite();

    std::shared_ptr<DataBlockBody> popBlock();

    void writeData(std::shared_ptr<DataBlockBody>);

    int readDataFromFile(uint8_t counts);

    void jumpFileData(uint32_t pos);

    int writeDataToFile();

    int countQueue();

    void releaseResource();

    void createFile(const QString &path, const QString &file);

    void openFile(const QString &path, const QString &);

    void closeFile();

  private:
    QString datafile = "";
    FILE *_filehandle = nullptr;
    FILE *_readhandle = nullptr;
    moodycamel::ConcurrentQueue<std::shared_ptr<DataBlockBody>> dataQueue;
};

class DataRecordItem : public QObject {
    Q_OBJECT

    QPROPERTY_AUTO(bool, telemetry);
    QPROPERTY_AUTO(bool, media);
    QPROPERTY_AUTO(bool, weather);
    QPROPERTY_AUTO(bool, contrlAdjust);
    QPROPERTY_AUTO(int, index);
    QPROPERTY_AUTO(int, during);
    QPROPERTY_AUTO(int64_t, startTm);
    QPROPERTY_AUTO(QString, recordTime);

  public:
    DataRecordItem();
    Q_INVOKABLE QVariant columnValue(int column);
    static int64_t currentTimeMs();
};

class GcsDataManager : public QObject {
    Q_OBJECT
    QPROPERTY_AUTO(bool, hasTelemetry);
    QPROPERTY_AUTO(bool, hasMedia);
    QPROPERTY_AUTO(bool, hasWeather);
    QPROPERTY_AUTO(bool, hasCtlAdjust);

    QPROPERTY_AUTO(qint64, startTime);
    Q_PROPERTY(int rwState READ rwState NOTIFY rwStateChanged);
    Q_PROPERTY(double playSpeed READ playSpeed WRITE setPlaySpeed NOTIFY playSpeedChanged);
    Q_PROPERTY(int slideValue READ slideValue WRITE setSlideValue NOTIFY slideValueChanged);
    Q_PROPERTY(GcsListmodel *visualDataList READ visualDataList NOTIFY visualDataListChanged)

  signals:
    void dataThreadExited();
    void rwStateChanged();
    void playSpeedChanged();
    void slideValueChanged(int);
    void visualDataListChanged();

  public:
    enum GcsExportType { MediaType = 1, TelemetryType, AdjustControlType, WeatherType };

    GcsDataManager(QObject *parent = nullptr);
    ~GcsDataManager();

    void setPlaySpeed(double val);
    void setSlideValue(int val);

    GcsListmodel *visualDataList() { return &visual_datalist; }
    double playSpeed() { return play_speed; }
    int slideValue() { return slide_value; }
    int rwState() { return rw_data_flag.load(); }

    Q_INVOKABLE void loadDataFile();
    Q_INVOKABLE void stopReplay();
    Q_INVOKABLE void pauseReplay();
    Q_INVOKABLE void stopRecord();
    Q_INVOKABLE void startRecord();
    Q_INVOKABLE bool startReplay(QString file, int selectRow);

    void writeData(ConfigController *, int chl, const void *sock, const uint8_t *data, int datalen);
    void writeToJson(DataRecordItem *);
    void readJson();

  private:
    bool replayData(std::shared_ptr<DataBlockBody>, int64_t last, QUdpSocket &sock);

  private:
    enum DataRWState { RW_Invalid = 0, DataReplay = 1, DataWrite = 2 };
    std::atomic_bool play_flag;
    std::atomic<int> rw_data_flag = 0;
    std::thread data_thread;

    QList<std::shared_ptr<DataRecordItem>> cache_record_data;
    std::shared_ptr<DataRecordItem> data_record_item;
    DataRecordReadWrite telemetry_rw_queue;
    DataRecordReadWrite ctrladjust_rw_queue;
    DataRecordReadWrite weather_queue;
    DataRecordReadWrite media_queue;
    std::atomic<int> slide_value = 0;
    int curr_play_loc = 0;
    double play_speed = 0;
    int cur_select_row = -1;
    GcsListmodel visual_datalist;
    GcsTreeModel data_tree_model;
};
