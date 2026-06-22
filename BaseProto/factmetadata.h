#pragma once

#include <QtQml/qqml.h>

#include "factlistmodel.h"
#include "protometadef.h"
#include "transmetadata.h"

class BASEPROTO_EXPORT TransMetaData : public FactMetaData {
    Q_OBJECT
  public:
    ~TransMetaData();
    TransMetaData(QObject *parent = nullptr);
    TransMetaData(const TransMetaData &other);
    TransMetaData &operator=(const TransMetaData &other);

    Q_PROPERTY(int refItem READ refItem CONSTANT)
    Q_PROPERTY(FactListModel *subListModel READ subListModel CONSTANT)

    int refItem() const { return _refItem; }
    QString copyData() const { return _copyData; }

    void setRefItem(int val) { _refItem = val; }
    void setCopyData(const QString &val) { _copyData = val; }

    int children() const { return _subfactDatas.count(); }

    template <class Type>
    void protoTrans(Type val, int chl, ReferDeviceCallback refer);

    ///
    /// \brief setErrString
    /// \param val
    /// \param frame
    /// \param id
    /// \return
    ///
    std::shared_ptr<WarnConfig> setErrString(const QVariant &val, const QString &frame, uint8_t id);

    WarnConfig *warnConfig() { return &_warnConfig; }

    void insertFactNode(int set, std::shared_ptr<TransMetaData>);

    std::shared_ptr<TransMetaData> createFactNode();

    FactListModel *subListModel() { return &_subfactDatas; }

    ///
    /// \brief setProtoValue
    /// \param data
    /// \param select
    /// \param chl
    /// \param refer
    /// \return
    ///
    int setProtoValue(const uint8_t *data, int select, int chl, ReferDeviceCallback refer);

    /// EXCEL文件定义COPY行为
    ///
    void doFactCopy();

    typedef int (*DataTranslator)(TransMetaData *ptr, const uint8_t *len, int chls,
                                  ReferDeviceCallback refer);

    static QMap<QString, DataTranslator> &loadDataTranslator();

    /// 协议数据到显示数据, 当协议判断字节数错误的时候
    /// 返回值为数据解析后偏移地址
    ///
    int protoData(const uint8_t *data, int chl, ReferDeviceCallback refer);

  protected:
    int _refItem = 0;
    QString _copyData = "";
    WarnConfig _warnConfig;
    FactListModel _subfactDatas;
    QMap<int, std::shared_ptr<TransMetaData>> _cacheNode;
};
