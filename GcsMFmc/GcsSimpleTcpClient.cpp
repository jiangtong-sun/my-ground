#include "GcsSimpleTcpClient.h"
#include "log_category.h"

SimpleTcpClient::SimpleTcpClient(QObject *parent)
    : QObject(parent), m_socket(nullptr), m_stopFlag(0)
{
    connect(&reconn_timer, &QTimer::timeout, this, &SimpleTcpClient::scheduleReconnect);
}

void SimpleTcpClient::sendData(const QByteArray &data)
{
    if (m_socket && m_socket->state() == QAbstractSocket::ConnectedState) {
        m_socket->write(data);
    } else {
        COMMON_INFO("socket is not connected state");
    }
}

void SimpleTcpClient::onConnected()
{
    m_connectState = true;
    COMMON_INFO("connected");
}

void SimpleTcpClient::onDisconnected()
{
    COMMON_INFO("disconnected");
    m_connectState = false;
}

void SimpleTcpClient::onError(QAbstractSocket::SocketError socketError)
{
    COMMON_INFO("socket error {} -- {}", (int)socketError,
                m_socket->errorString().toLocal8Bit().toStdString());
    switch (socketError) {
        case QAbstractSocket::RemoteHostClosedError:
        case QAbstractSocket::ConnectionRefusedError:
        case QAbstractSocket::HostNotFoundError:
        case QAbstractSocket::NetworkError:
            m_connectState = false;
            break;
        default:
            break;
    }
}

void SimpleTcpClient::onStateChanged(QAbstractSocket::SocketState state)
{
    COMMON_INFO("socket state changed to {}", (int)state);
}

void SimpleTcpClient::stopClient(){
    COMMON_INFO("tcp client {}, state {}", (int)m_socket, m_stopFlag.load());
    m_stopFlag = 0x1;
    reconn_timer.stop();
}

void SimpleTcpClient::initSocket(const QString &host, quint16 port, SimpleRecvCallback recvCb)
{
    m_host = host;
    m_port = port;
    recv_callback = recvCb;
    reconn_timer.start(3000);
}

void SimpleTcpClient::connectToHost()
{
    m_socket = new QTcpSocket(this);
    COMMON_INFO("connecting to {}:{}, socket address: {}", m_host.toStdString(), m_port, (int)m_socket);
    if (m_socket == nullptr) {
        return;
    }

    m_socket->setSocketOption(QAbstractSocket::LowDelayOption, 1);
    m_socket->setSocketOption(QAbstractSocket::KeepAliveOption, 1);
    connect(m_socket, &QTcpSocket::connected, this, &SimpleTcpClient::onConnected);
    connect(m_socket, &QTcpSocket::disconnected, this, &SimpleTcpClient::onDisconnected);
    connect(m_socket, QOverload<QAbstractSocket::SocketError>::of(&QTcpSocket::errorOccurred),
           this, &SimpleTcpClient::onError);
    connect(m_socket, &QTcpSocket::stateChanged, this, &SimpleTcpClient::onStateChanged);
    connect(m_socket, &QTcpSocket::readyRead, this, [=]() {
        if(m_socket->bytesAvailable() > 0) {
            QByteArray data = m_socket->readAll();
            COMMON_TRACE("data length {}", data.length());
            count_recvs += data.length();
            if (recv_callback != nullptr) {
                recv_callback(data);
            }
        }
    });

    m_socket->connectToHost(m_host, m_port);
}

void SimpleTcpClient::scheduleReconnect()
{
    if (!m_connectState && !m_stopFlag) {
        COMMON_INFO("schedule connecting to {}:{}", m_host.toStdString(), m_port);
        if (m_socket && m_socket->state() != QAbstractSocket::UnconnectedState) {
            m_socket->abort(); // 中止当前连接
            m_socket = nullptr;
        }

        if (m_socket) { // 删除socket
            m_socket->close();
            m_socket->deleteLater();
            m_socket = nullptr;
        }

        if (m_stopFlag.load() == 0x01) {
            COMMON_INFO("exit schedule {}", m_stopFlag.load());
            m_stopFlag = 2;
            return;
        }

        connectToHost();

        return;
    }

    if (count_recvs == 0) {
        ++count_unrecvdata;
        /// 一直没有收到消息数据10秒的时候
        if (count_unrecvdata > 3) {
            m_connectState = false;
        }
    }

    count_recvs = 0;
}
