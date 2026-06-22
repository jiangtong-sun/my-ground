#pragma once

#include <QDateTime>

#include "facttablemodel.h"
#include "xlsxworkbook.h"

class BASEPROTO_EXPORT ParseProtoFile {

  public:
    ParseProtoFile();

    ///
    /// \brief protoParse
    ///     解析协议文件，创建协议TABLE
    /// \param proto    协议TABLE
    /// \param file     协议文件
    /// \param tm       协议解码文件时间
    /// \return
    ///
    bool protoParse(QVector<std::shared_ptr<WarnConfig>> &warns,
                    QMap<int, std::shared_ptr<FactTableModel>> &proto,
                    const QString &protofile, QDateTime filetime);

    static bool warnParse(QMap<PWarnConfGrpK, PWarnConfGrpV>& warnGroup,
                          FactTableModelCallabck proto,
                          const QString &protofile);
    static void warnDefine(WarnConfigExt *metaDataMap, const QVariant &);
    static void warnCondition(PWarnConfGrpK &metaDataMap, const QVariant &);

  private:
    ///
    /// \brief loadSubPart
    ///     如果协议某项是一个结构体，需要从Sheet文件中获取结构体详细信息
    /// \param metaDataMap
    /// \param sheet
    /// \param frame
    ///
    void loadSubPart(std::shared_ptr<TransMetaData> metaDataMap, QXlsx::Worksheet *sheet,
                     const QString &frame);

    ///
    /// \brief handleMetaDataType
    ///     判断协议项目数据类型
    /// \param dataElem
    /// \param value
    /// \return
    ///
    bool handleMetaDataType(std::shared_ptr<TransMetaData> dataElem, QVariant value);

    ///
    /// \brief handleResoultion
    ///     处理协议项分辨率设置
    /// \param dataElem
    /// \param value
    /// \return
    ///
    bool handleResoultion(std::shared_ptr<TransMetaData> dataElem, QVariant value);

  protected:
    struct ColumnName {
        QString name;
        int column;
    };

    QVector<ColumnName> excelTableHeader;
    QMap<QString, QString> sheetHasProcess;
    QMap<QString, MetaDataType> datatypeMap;
};
