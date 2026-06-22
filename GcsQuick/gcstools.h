#pragma once

#include "gcsstdafx.h"

#include <QColor>
#include <QIcon>
#include <QObject>
#include <QUrl>
#include <QtQml/qqml.h>

class CONTROL_EXPORT GcsTools : public QObject {
    Q_OBJECT
    QML_NAMED_ELEMENT(GcsTools)
    QML_SINGLETON
  public:
    explicit GcsTools(QObject *parent = nullptr);

    static GcsTools *instance();
    static GcsTools *create(QQmlEngine *, QJSEngine *) { return instance(); }

    Q_INVOKABLE int qtMajor();
    Q_INVOKABLE int qtMinor();

    Q_INVOKABLE bool isMacos();
    Q_INVOKABLE bool isLinux();
    Q_INVOKABLE bool isWin();

    Q_INVOKABLE void clipText(const QString &text);

    Q_INVOKABLE QString uuid();

    Q_INVOKABLE QString readFile(const QString &fileName);

    Q_INVOKABLE void setQuitOnLastWindowClosed(bool val);

    Q_INVOKABLE void setOverrideCursor(Qt::CursorShape shape);

    Q_INVOKABLE void restoreOverrideCursor();

    Q_INVOKABLE void deleteLater(QObject *p);

    Q_INVOKABLE QRect getVirtualGeometry();

    Q_INVOKABLE QString getApplicationDirPath();

    Q_INVOKABLE QUrl getUrlByFilePath(const QString &path);

    Q_INVOKABLE QColor withOpacity(const QColor &, qreal alpha);

    Q_INVOKABLE QString md5(const QString &text);
    Q_INVOKABLE QString sha256(const QString &text);
    Q_INVOKABLE QString toBase64(const QString &text);
    Q_INVOKABLE QString fromBase64(const QString &text);

    Q_INVOKABLE bool removeDir(const QString &dirPath);
    Q_INVOKABLE bool removeFile(const QString &filePath);

    Q_INVOKABLE qint64 currentTimestamp();
    Q_INVOKABLE QPoint cursorPos();
    Q_INVOKABLE QIcon windowIcon();

    Q_INVOKABLE bool isSoftware();
    Q_INVOKABLE int cursorScreenIndex();
};
