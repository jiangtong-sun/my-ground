#pragma once

#include "factlistmodel.h"
#include "factmetadata.h"

#include <QAbstractTableModel>
#include <QDateTime>
#include <QFile>
#include <fstream>
#include <sstream>

#define DATA_CACHE_SPACE 1024 * 32

class BASEPROTO_EXPORT FactTableModel : public QAbstractTableModel {
    Q_OBJECT
    Q_DISABLE_COPY(FactTableModel)

  public:
    Q_PROPERTY(int code READ code WRITE setCode NOTIFY codeChanged)
    Q_PROPERTY(bool checked READ checked WRITE setChecked NOTIFY checkedChanged)
    Q_PROPERTY(QString name READ name WRITE setName NOTIFY nameChanged)
    Q_PROPERTY(FactListModel *listModel READ listModel CONSTANT)
    ///
    /// \brief FactTableModel
    /// \param parent
    ///
    FactTableModel(QDateTime, QObject *parent = nullptr);

    ~FactTableModel();

    ///
    /// \brief decodeWarnData
    ///     解析告警规则，判断是否需要告警
    /// \param warnConf
    /// \param table
    /// \param warn
    /// \param first
    /// \param second
    /// \param chl
    ///
    static void decodeWarnData(WarnConfig *warnConf, FactTableModel *table, QString &timestamp,
                               WarningLevel warn, CheckWarnCallback first, int chl);

    static std::shared_ptr<WarnElemNode> warnData(WarnConfig *warnConf, FactTableModel *table,
                                                  QString &timestamp, WarningLevel warn,
                                                  int priority, const QString &name);
    ///
    /// \brief flushModelData
    ///     判断协议个字段，是配置告警逻辑
    /// \param first
    /// \param second
    /// \param chl
    ///
    void flushModelData(CheckWarnCallback first, int chl);

    void signalModel(int chl);

    void setCode(int val) {
        _code = val;
        emit codeChanged();
    }

    void setChecked(bool val) {
        _checked = val;
        emit checkedChanged();
    }

    void setName(QString val) {
        _name = val;
        emit nameChanged();
    }

    int code() const { return _code; }
    bool checked() const { return _checked; }
    void setDefChl(int val) { _defChl = val; }
    QString name() const { return _name; }

    QString currTimestamp(int chl) const;

    FactMetaData *at(int i) const;
    FactMetaData *loadFact(int i) const;
    FactMetaData *find(const QString &name) const;

    ///
    /// \brief resetFilename
    /// \param channel
    ///
    void resetFilename(int channel);

    ///
    /// \brief closeFileSave
    ///
    void closeFileSave();

    ///
    /// \brief dataFileSave
    /// \param utc
    /// \param channel
    ///
    void dataFileSave(float utc, int channel, int select, int timestamp);

    int rowCount() const { return _listModel.count(); }

    FactListModel *listModel() { return &_listModel; }

    ///
    /// \brief decoder
    /// \param len
    /// \param chl
    /// \param refer
    /// \return
    ///
    virtual int decoder(const uint8_t *, int len, int chl, ReferDeviceCallback refer);

    ///
    /// \brief addFactMeta
    ///
    void addFactMeta(int, std::shared_ptr<TransMetaData> &);

    int rowCount(const QModelIndex &parent) const override;
    int columnCount(const QModelIndex &parent) const override;
    QVariant data(const QModelIndex &index, int role) const override;
    QVariant headerData(int sect, Qt::Orientation orient, int role) const override;
    QHash<int, QByteArray> roleNames() const override;

  signals:
    void nameChanged();
    void codeChanged();
    void checkedChanged();

  protected:
    int _code = 0;
    int _timestamp = 0;
    bool _biglittle = false;
    bool _checked = false;
    QString _name = "";
    QDateTime _file_time;
    FactMetaData _nullData;
    QStringList _headData;
    FactListModel _listModel;
    QList<std::shared_ptr<TransMetaData>> _datas;
    std::atomic_bool stop_flag{false};
    std::atomic_bool open_flag{false};
    std::atomic_int _defChl{0};

    struct FileCache {
        std::ofstream ofile;
        uint32_t _counts = 0;
        QString curTimestamp = "";
        char _cache[DATA_CACHE_SPACE];

        FileCache();
        uint32_t counts() { return ++_counts; }
        bool createFile(const QString &file);
        void closeFile();
    };

    QMap<int, std::shared_ptr<FileCache>> _fileHandlers;
};

using FactTableModelCallabck = std::function<FactTableModel *(int)>;
