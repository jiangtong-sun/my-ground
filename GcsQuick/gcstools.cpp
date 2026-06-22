#include "GcsTools.h"

#include <QClipboard>
#include <QCryptographicHash>
#include <QCursor>
#include <QDateTime>
#include <QDir>
#include <QFileInfo>
#include <QGlobalStatic>
#include <QGuiApplication>
#include <QQuickWindow>
#include <QScreen>
#include <QUuid>

#ifdef Q_OS_WIN
#pragma comment(lib, "user32.lib")
#include <windows.h>
#include <windowsx.h>
#endif

Q_GLOBAL_STATIC(GcsTools, gEnvTool)
GcsTools *GcsTools::instance() { return gEnvTool(); }

GcsTools::GcsTools(QObject *parent) : QObject{parent} {}

void GcsTools::clipText(const QString &text) { QGuiApplication::clipboard()->setText(text); }

QString GcsTools::uuid() {
    return QUuid::createUuid().toString().remove('-').remove('{').remove('}');
}

QString GcsTools::readFile(const QString &fileName) {
    QString content;
    QFile file(fileName);
    if (file.open(QIODevice::ReadOnly)) {
        QTextStream stream(&file);
        content = stream.readAll();
    }
    return content;
}

bool GcsTools::isMacos() {
#if defined(Q_OS_MACOS)
    return true;
#else
    return false;
#endif
}

bool GcsTools::isLinux() {
#if defined(Q_OS_LINUX)
    return true;
#else
    return false;
#endif
}

bool GcsTools::isWin() {
#if defined(Q_OS_WIN)
    return true;
#else
    return false;
#endif
}

int GcsTools::qtMajor() {
    const QString qtVersion = QString::fromLatin1(qVersion());
    const QStringList versionParts = qtVersion.split('.');
    return versionParts[0].toInt();
}

int GcsTools::qtMinor() {
    const QString qtVersion = QString::fromLatin1(qVersion());
    const QStringList versionParts = qtVersion.split('.');
    return versionParts[1].toInt();
}

void GcsTools::setQuitOnLastWindowClosed(bool val) {
    QGuiApplication::setQuitOnLastWindowClosed(val);
}

void GcsTools::setOverrideCursor(Qt::CursorShape shape) {
    QGuiApplication::setOverrideCursor(QCursor(shape));
}

void GcsTools::restoreOverrideCursor() { QGuiApplication::restoreOverrideCursor(); }

void GcsTools::deleteLater(QObject *p) {
    if (p) {
        p->deleteLater();
    }
}

QRect GcsTools::getVirtualGeometry() { return QGuiApplication::primaryScreen()->virtualGeometry(); }

QString GcsTools::getApplicationDirPath() { return QGuiApplication::applicationDirPath(); }

QUrl GcsTools::getUrlByFilePath(const QString &path) { return QUrl::fromLocalFile(path); }

QColor GcsTools::withOpacity(const QColor &color, qreal opacity) {
    int alpha = qRound(opacity * 255) & 0xff;
    return QColor::fromRgba((alpha << 24) | (color.rgba() & 0xffffff));
}

QString GcsTools::md5(const QString &text) {
    return QCryptographicHash::hash(text.toUtf8(), QCryptographicHash::Md5).toHex();
}

QString GcsTools::toBase64(const QString &text) { return text.toUtf8().toBase64(); }

QString GcsTools::fromBase64(const QString &text) { return QByteArray::fromBase64(text.toUtf8()); }

bool GcsTools::removeDir(const QString &dirPath) {
    QDir qDir(dirPath);
    return qDir.removeRecursively();
}

bool GcsTools::removeFile(const QString &filePath) {
    QFile file(filePath);
    return file.remove();
}

QString GcsTools::sha256(const QString &text) {
    return QCryptographicHash::hash(text.toUtf8(), QCryptographicHash::Sha256).toHex();
}

QPoint GcsTools::cursorPos() { return QCursor::pos(); }

qint64 GcsTools::currentTimestamp() { return QDateTime::currentMSecsSinceEpoch(); }

QIcon GcsTools::windowIcon() { return QGuiApplication::windowIcon(); }

bool GcsTools::isSoftware() { return QQuickWindow::sceneGraphBackend() == "software"; }

int GcsTools::cursorScreenIndex() {
    int screenIndex = 0;
    int screenCount = QGuiApplication::screens().count();
    if (screenCount > 1) {
        QPoint pos = QCursor::pos();
        for (int i = 0; i <= screenCount - 1; ++i) {
            if (QGuiApplication::screens().at(i)->geometry().contains(pos)) {
                screenIndex = i;
                break;
            }
        }
    }
    return screenIndex;
}
