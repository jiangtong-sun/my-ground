#pragma once

#include "CommandItem.h"
#include "factlistmodel.h"
#include "protometadef.h"
#include <QObject>
#include <memory>

class BASEPROTO_EXPORT RCCommandGroup : public QObject {
    Q_OBJECT
    Q_DISABLE_COPY(RCCommandGroup)

  public:
    Q_PROPERTY(QString groupName READ groupName WRITE setGroupName NOTIFY nameChanged)
    Q_PROPERTY(bool checked READ checked WRITE setChecked NOTIFY checkedChanged)
    Q_PROPERTY(FactListModel *commands READ commands NOTIFY commandChanged)
    Q_PROPERTY(QString vertName READ vertName CONSTANT)
    Q_PROPERTY(int columnSize READ columnSize CONSTANT)

    RCCommandGroup();
    ~RCCommandGroup();

    QString vertName() { return _vertName; }
    QString groupName() { return _name; }
    void setGroupName(const QString &val);
    bool checked() { return _checked; }

    void resetChecking();

    void setChecked(bool val) {
        _checked = val;
        emit checkedChanged();
    }

    /// adjust ui layout
    int columnSize() { return _subcmds.count() > 4 ? 4 : _subcmds.count(); }

    bool resetWorkState(int code);

    virtual void insert(std::shared_ptr<RCCommandItem>);

    FactListModel *commands() { return &_subcmds; }

  signals:
    void checkedChanged();
    void commandChanged();
    void nameChanged();

  protected:
    QString _name = "";
    QString _vertName = "";
    bool _checked = false;
    FactListModel _subcmds;
    QList<std::shared_ptr<RCCommandItem>> cmd_cache;
};

class BASEPROTO_EXPORT RCCmdSubGroup : public QObject {
    Q_OBJECT
    Q_DISABLE_COPY(RCCmdSubGroup)

  public:
    Q_PROPERTY(QString groupName READ groupName CONSTANT)
    Q_PROPERTY(RCCommandItem *firstNode READ firstNode CONSTANT)
    Q_PROPERTY(RCCommandItem *secondNode READ secondNode CONSTANT)
    Q_PROPERTY(RCCommandItem *thirdNode READ thirdNode CONSTANT)
    Q_PROPERTY(FactListModel *subcommands READ subcommands CONSTANT)
    Q_PROPERTY(QString vertName READ vertName CONSTANT)
    Q_PROPERTY(int counts READ counts CONSTANT)

    RCCmdSubGroup();

    QString vertName() { return _vertName; }
    QString groupName() { return _name; }
    void setGroupName(const QString &val);
    void insert(std::shared_ptr<RCCommandItem>);
    int counts() const { return cmd_cache.size(); }
    FactListModel *subcommands();
    RCCommandItem *firstNode();
    RCCommandItem *secondNode();
    RCCommandItem *thirdNode();

  signals:
    void nameChanged();

  private:
    QString _name = "";
    QString _vertName = "";
    FactListModel _sub_cmds;
    QVector<std::shared_ptr<RCCommandItem>> cmd_cache;
};

class BASEPROTO_EXPORT RCCommandGroupExt : public RCCommandGroup {
    Q_OBJECT
    Q_DISABLE_COPY(RCCommandGroupExt)
    Q_PROPERTY(int rowElem READ rowElem CONSTANT)
  public:
    RCCommandGroupExt() : RCCommandGroup() {}
    void insert(std::shared_ptr<RCCommandItem>);
    int rowElem() { return _row_elem; }

  protected:
    int _row_elem = 0;
    QMap<QString, std::shared_ptr<RCCmdSubGroup>> command_group;
};

class BASEPROTO_EXPORT RCAdjustGroup : public QObject {
    Q_OBJECT
    Q_DISABLE_COPY(RCAdjustGroup)
    Q_PROPERTY(int count READ count CONSTANT)
    Q_PROPERTY(int groupType READ groupType CONSTANT)
    Q_PROPERTY(QString vertName READ vertName CONSTANT)
    Q_PROPERTY(QString groupName READ groupName CONSTANT)
    Q_PROPERTY(QStringList groupList READ groupList CONSTANT)
    Q_PROPERTY(RCAdjustItem *firstItem READ firstItem CONSTANT)
    Q_PROPERTY(RCAdjustItem *secondItem READ secondItem CONSTANT)
    Q_PROPERTY(bool checked READ checked WRITE setChecked NOTIFY checkedChanged)
    Q_PROPERTY(FactListModel *adjustCmds READ adjustCmds NOTIFY adjustCmdsChanged)
  public:
    RCAdjustGroup() = default;
    ~RCAdjustGroup() = default;

    QStringList groupList();

    RCAdjustItem *firstItem() const;
    RCAdjustItem *secondItem() const;
    RCAdjustItem *findItem(int feedback, int cmdtype, int subtype) const;
    RCAdjustItem *findSimpleItem(int itemNum) const;
    RCAdjustItem *findAdjustByFeed(int itemNum) const;

    Q_INVOKABLE RCAdjustItem *findItemByCmdType(int);

    QString vertName() const { return _vertName; }
    QString groupName() const { return _groupName; }
    void setGroupName(const QString &val);

    FactListModel *adjustCmds() { return &_adjustCmds; }
    void add(std::shared_ptr<RCAdjustItem> elem);
    void setGroupType(int val) { _groupType = val; }
    bool checked() const { return _checked; }
    int groupType() const { return _groupType; }
    int count() const { return _adjustcmd_set.count(); }
    void setChecked(bool val) {
        _checked = val;
        emit checkedChanged();
    }
  signals:
    void firstItemChanged();
    void adjustCmdsChanged();
    void checkedChanged();

  private:
    QList<std::shared_ptr<RCAdjustItem>> _adjustcmd_set;
    FactListModel _adjustCmds;
    bool _checked = false;
    int _groupType = 0;
    QString _vertName = "";
    QString _groupName = "";
};
