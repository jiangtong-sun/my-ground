#pragma once

#include "networkdata.h"
#include <QColor>
#include <QObject>
#include <QString>

#define WriteConf(TYPE, M, conf)                                                                                       \
    void M(const TYPE &in_##M) { conf._##M = in_##M; }

#define ReadConf(TYPE, M, conf)                                                                                        \
    TYPE M() { return _##M; }

#define EMEMBER_READWRITE(TYPE, M)                                                                                     \
  public:                                                                                                              \
    void M(const TYPE &in_##M) { _##M = in_##M; }                                                                      \
    TYPE M() { return _##M; }                                                                                          \
                                                                                                                       \
  private:                                                                                                             \
    TYPE _##M;

#define PROPERTY_READWRITE(TYPE, M)                                                                                    \
    Q_PROPERTY(TYPE M MEMBER _##M NOTIFY M##Changed)                                                                   \
  public:                                                                                                              \
    Q_SIGNAL void M##Changed();                                                                                        \
    void M(const TYPE &in_##M) {                                                                                       \
        _##M = in_##M;                                                                                                 \
        Q_EMIT M##Changed();                                                                                           \
    }                                                                                                                  \
    TYPE M() { return _##M; }                                                                                          \
                                                                                                                       \
  protected:                                                                                                           \
    TYPE _##M;

class SoftwareCommConfig : public QObject {
    Q_OBJECT
    Q_DISABLE_COPY(SoftwareCommConfig)
    PROPERTY_READWRITE(QString, deftChannel)
    PROPERTY_READWRITE(QString, controlChannel)
    PROPERTY_READWRITE(QString, taskChannel)

  public:
    SoftwareCommConfig() {
        _deftChannel = QString("udp://239.0.0.1:19191");
        _controlChannel = QString("udp://239.0.0.1:19191");
        _taskChannel = QString("udp://239.0.0.1:29292");
    }

    QStringList channelList() {
        QStringList rtn;
        rtn.append(_controlChannel);
        rtn.append(_taskChannel);
        return rtn;
    }

    ~SoftwareCommConfig() = default;
};

class NetworkNodeConfig : public QObject {
    Q_OBJECT
    Q_DISABLE_COPY(NetworkNodeConfig)
    PROPERTY_READWRITE(int, quality)
    PROPERTY_READWRITE(bool, localLoop)
    PROPERTY_READWRITE(bool, biglittle)
    PROPERTY_READWRITE(bool, mockData)
    PROPERTY_READWRITE(bool, enable)
    PROPERTY_READWRITE(bool, tcpMode)
    PROPERTY_READWRITE(bool, copyMode)

  public:
    bool multiMode() { return net_config.multi; }
    void multiMode(bool v) { net_config.multi = v; }

    int multiRxPort() { return net_config.multiRxPort; }
    void multiRxPort(int v) { net_config.multiRxPort = v; }

    int multiTxPort() { return net_config.multiTxPort; }
    void multiTxPort(int v) { net_config.multiTxPort = v; }

    int dataTransFreq() { return net_config.dataTransFreq; }
    void dataTransFreq(int v) { net_config.dataTransFreq = v; }

    int netPort() { return net_config.netPort; }
    void netPort(int v) { net_config.netPort = v; }

    QString netIp() { return QString::fromStdString(net_config.address); }
    void netIp(QString v) { net_config.address = v.toStdString(); }

    QString multiRxIp() { return QString::fromStdString(net_config.multiRxAddr); }
    void multiRxIp(QString v) { net_config.multiRxAddr = v.toStdString(); }

    QString multiTxIp() { return QString::fromStdString(net_config.multiTxAddr); }
    void multiTxIp(QString v) { net_config.multiTxAddr = v.toStdString(); }

  public:
    NetworkNodeConfig() = default;
    ~NetworkNodeConfig() = default;

    ChannelConfig &config() { return net_config; }

  protected:
    ChannelConfig net_config;
};

class FlightLandCurve: public QObject {
    Q_OBJECT
    PROPERTY_READWRITE(QColor, background)
    PROPERTY_READWRITE(QColor, axisColor)
    PROPERTY_READWRITE(QColor, graph1Color)
    PROPERTY_READWRITE(QColor, graph2Color)
    PROPERTY_READWRITE(QColor, graph3Color)
    PROPERTY_READWRITE(QColor, graph4Color)
    PROPERTY_READWRITE(QColor, gridColor)
    PROPERTY_READWRITE(int, graph1LineWidth)
    PROPERTY_READWRITE(int, graph2LineWidth)
    PROPERTY_READWRITE(int, graph3LineWidth)
    PROPERTY_READWRITE(int, graph4LineWidth)

public:
    FlightLandCurve() {
        _background = QColor(155, 155, 155);
        _axisColor = QColor(250, 250, 250);
        _graph1Color = QColor(255, 10, 10);
        _graph2Color = QColor(10, 210, 20);
        _graph3Color = QColor(10, 10, 250);
        _graph4Color = QColor(205, 205, 205);
        _gridColor =  QColor(205, 205, 205);
        _graph1LineWidth = 2;
        _graph2LineWidth = 2;
        _graph3LineWidth = 2;
        _graph4LineWidth = 2;
    }
    ~FlightLandCurve() = default;
};

class FlightLanding : public QObject {
    Q_OBJECT

    PROPERTY_READWRITE(double, dstOffset)
    PROPERTY_READWRITE(double, gamaSteep)
    PROPERTY_READWRITE(double, gamaSteepMix)
    PROPERTY_READWRITE(double, gamaShallow)
    PROPERTY_READWRITE(double, pullStart)
    PROPERTY_READWRITE(double, pullNext)
    PROPERTY_READWRITE(double, landEnter)
    PROPERTY_READWRITE(QString, landName)
  public:
    FlightLanding() {
        _dstOffset = 2;
        _gamaSteep = -3;
        _gamaSteepMix = 0;
        _gamaShallow = -1.5;
        _landEnter = 300;
        _pullStart = 30;
        _pullNext = 20;
    }
    ~FlightLanding() = default;
};

class AppLocateConfig : public QObject {
    Q_OBJECT
    PROPERTY_READWRITE(int, screenId)
    PROPERTY_READWRITE(int, width)
    PROPERTY_READWRITE(int, height)
    PROPERTY_READWRITE(int, xpos)
    PROPERTY_READWRITE(int, ypos)
  public:
    AppLocateConfig() {
        _screenId = 1;
        _width = 1920;
        _height = 1080;
        _xpos = 0;
        _ypos = 0;
    }

    ~AppLocateConfig() = default;
};

class SerialNodeConfig : public QObject {
    Q_OBJECT
    Q_DISABLE_COPY(SerialNodeConfig)
    PROPERTY_READWRITE(bool, state)
    PROPERTY_READWRITE(bool, enable)
    PROPERTY_READWRITE(int, bautrate)
    PROPERTY_READWRITE(int, stopbit)
    PROPERTY_READWRITE(int, databit)
    PROPERTY_READWRITE(int, parity)
    PROPERTY_READWRITE(int, timeout)
    PROPERTY_READWRITE(int, quality)
    PROPERTY_READWRITE(QString, commName)
  public:
    SerialNodeConfig() {
        _state = false;
        _enable = false;
        _stopbit = 0;
        _databit = 8;
        _parity = 0;
        _timeout = 10;
        _bautrate = 115200;
        _commName = QStringLiteral("COM1");
    }

    ~SerialNodeConfig() = default;
};
