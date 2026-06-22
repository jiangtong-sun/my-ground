#pragma once

#include <QTcpSocket>
#include <QTimer>

using SimpleRecvCallback = std::function<void(const QByteArray &)>;

class SimpleTcpClient : public QObject
{
    Q_OBJECT
public:
    explicit SimpleTcpClient(QObject *parent = nullptr);
    void initSocket(const QString &host, quint16 port, SimpleRecvCallback recvCb);
    void sendData(const QByteArray &data);
    void scheduleReconnect();
    void connectToHost();
    void stopClient();

private slots:
    void onConnected();
    void onDisconnected();
    void onError(QAbstractSocket::SocketError socketError);
    void onStateChanged(QAbstractSocket::SocketState state);

private:
    int count_unrecvdata = 0;
    std::atomic_int count_recvs = 0;
    std::atomic_int m_stopFlag = 0;
    std::atomic_bool m_connectState = 0;
    SimpleRecvCallback recv_callback = nullptr;
    QTcpSocket *m_socket = nullptr;
    QTimer reconn_timer;
    QString m_host = "";
    quint16 m_port = 0;
};
