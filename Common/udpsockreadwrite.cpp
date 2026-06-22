#include "udpsockreadwrite.h"
#include "commacro.h"
#include "log_category.h"

UdpSockReadWrite::UdpSockReadWrite()
    : rx_recv_buff(uv_buf_init((char *) malloc(1 << 16), 1 << 16)),
      tx_recv_buff(uv_buf_init((char *) malloc(1 << 16), 1 << 16)), reconn_delay(0), do_destory(0),
      multicast_mode(0), rw_server_addr(), rw_multiRx_addr(), rw_multiTx_addr(), rw_mportRx(0),
      rw_mportTx(0) {
    udp_rx_sock_handle.data = this;
    udp_tx_sock_handle.data = this;
    destroy_task.data = this;
}

UdpSockReadWrite::~UdpSockReadWrite() {
    if (rx_recv_buff.base) {
        free(rx_recv_buff.base);
        rx_recv_buff.base = nullptr;
        rx_recv_buff.len = 0;
    }
    if (tx_recv_buff.base) {
        free(tx_recv_buff.base);
        tx_recv_buff.base = nullptr;
        tx_recv_buff.len = 0;
    }
}

void UdpSockReadWrite::tx_send_callback(uv_udp_send_t *req, int status) {
    UdpPackage *pack = reinterpret_cast<UdpPackage *>(req->data);
    if ((pack != nullptr) && (pack->callback != nullptr)) {
        pack->callback(status, pack->buff.base, pack->buff.len);
    }
    if (pack != nullptr) {
        delete (pack);
    }
}

void UdpSockReadWrite::tx_read_alloc_cb(uv_handle_t *udp_sock_handle, size_t size, uv_buf_t *buf) {
    COMMON_ASSERT_HANDLE(udp_sock_handle->data);
    UdpSockReadWrite *connection = (UdpSockReadWrite *) udp_sock_handle->data;
    if (connection != nullptr) {
        *buf = connection->tx_recv_buff;
    }
}

void UdpSockReadWrite::tx_read_recv_cb(uv_udp_t *udp_sock_handle, ssize_t nread,
                                       const uv_buf_t *buf, const struct sockaddr *addr,
                                       unsigned flags) {
    COMMON_ASSERT_HANDLE(udp_sock_handle->data);
    // struct sockaddr_in *sockaddr = (struct sockaddr_in *) (addr);
    UdpSockReadWrite *connection = reinterpret_cast<UdpSockReadWrite *>(udp_sock_handle->data);
    if ((connection != nullptr) && nread > 0) {
        if (connection->recv_callback != nullptr) {
            connection->recv_callback(connection->multicast_mode ? &connection->rw_multiTx_sock
                                                                 : &connection->rw_tx_socket,
                                      (uint8_t *) buf->base, nread);
        }
        if (connection->proto_callback != nullptr) {
            connection->data_tx_cache.addData((uint8_t *) buf->base, nread);
            connection->proto_callback(connection->data_tx_cache, connection->multicast_mode
                                                                      ? &connection->rw_multiTx_sock
                                                                      : &connection->rw_tx_socket);
        }
    }
}

void UdpSockReadWrite::rx_read_alloc_cb(uv_handle_t *udp_sock_handle, size_t size, uv_buf_t *buf) {
    COMMON_ASSERT_HANDLE(udp_sock_handle->data);
    UdpSockReadWrite *connection = (UdpSockReadWrite *) udp_sock_handle->data;
    if (connection != nullptr) {
        *buf = connection->rx_recv_buff;
    }
}

void UdpSockReadWrite::rx_read_recv_cb(uv_udp_t *udp_sock_handle, ssize_t nread,
                                       const uv_buf_t *buf, const struct sockaddr *addr,
                                       unsigned flags) {
    COMMON_ASSERT_HANDLE(udp_sock_handle->data);
    // struct sockaddr_in *sockaddr = (struct sockaddr_in *) (addr);
    UdpSockReadWrite *connection = reinterpret_cast<UdpSockReadWrite *>(udp_sock_handle->data);
    if ((connection != nullptr) && (connection->recv_callback != nullptr) && nread > 0) {
        if (connection->recv_callback != nullptr) {
            connection->recv_callback(connection->multicast_mode ? &connection->rw_multiRx_sock
                                                                 : &connection->rw_rx_socket,
                                      (uint8_t *) buf->base, nread);
        }
        if (connection->proto_callback != nullptr) {
            connection->data_rx_cache.addData((uint8_t *) buf->base, nread);
            connection->proto_callback(connection->data_rx_cache, connection->multicast_mode
                                                                      ? &connection->rw_multiRx_sock
                                                                      : &connection->rw_rx_socket);
        }
    }
}

void UdpSockReadWrite::destroyed_completed(uv_handle_t *udp_sock_handle) {
    COMMON_ASSERT_HANDLE(udp_sock_handle->data);
    UdpSockReadWrite *connection = reinterpret_cast<UdpSockReadWrite *>(udp_sock_handle->data);
    if ((connection != nullptr) && (connection->close_callback != nullptr)) {
        uv_udp_t *udphandle = reinterpret_cast<uv_udp_t *>(udp_sock_handle);
        COMMON_INFO("socket {} ", udphandle ? udphandle->socket : 0);
        long *add = reinterpret_cast<long *>(connection);
        connection->close_callback(*add);
    }
}

void UdpSockReadWrite::handle_destroyed_completed(uv_handle_t *handle) {
    COMMON_ASSERT_HANDLE(handle->data);
    UdpSockReadWrite *connection = reinterpret_cast<UdpSockReadWrite *>(handle->data);
    if ((connection != nullptr)) {
        if (connection->samePort()) {
            COMMON_INFO("stop udp channel: {}", (int) (connection->udp_tx_sock_handle.socket));
            uv_close((uv_handle_t *) &connection->udp_tx_sock_handle, destroyed_completed);
        } else {
            COMMON_INFO("stop udp channel: {}", (int) (connection->udp_rx_sock_handle.socket));
            COMMON_INFO("stop udp channel: {}", (int) (connection->udp_tx_sock_handle.socket));
            uv_close((uv_handle_t *) &connection->udp_rx_sock_handle, destroyed_completed);
            uv_close((uv_handle_t *) &connection->udp_tx_sock_handle, destroyed_completed);
        }
    }
}

void UdpSockReadWrite::destroyed(uv_async_t *handle) {
    COMMON_ASSERT_HANDLE(handle->data);
    UdpSockReadWrite *connection = reinterpret_cast<UdpSockReadWrite *>(handle->data);
    if ((connection != nullptr) && connection->multicast_mode) {
        if (connection->samePort()) {
            uv_udp_set_membership(&connection->udp_tx_sock_handle,
                                  connection->rw_multiTx_addr.c_str(), NULL, UV_LEAVE_GROUP);
            COMMON_INFO("leave multicast: {}", connection->rw_multiTx_addr.c_str());
        } else {
            uv_udp_set_membership(&connection->udp_rx_sock_handle,
                                  connection->rw_multiRx_addr.c_str(), NULL, UV_LEAVE_GROUP);
            COMMON_INFO("leave multicast: {}", connection->rw_multiRx_addr.c_str());
            uv_udp_set_membership(&connection->udp_tx_sock_handle,
                                  connection->rw_multiTx_addr.c_str(), NULL, UV_LEAVE_GROUP);
            COMMON_INFO("leave multicast: {}", connection->rw_multiTx_addr.c_str());
        }
    }

    if ((connection != nullptr)) {
        if (connection->samePort()) {
            uv_udp_recv_stop(&connection->udp_tx_sock_handle);
        } else {
            uv_udp_recv_stop(&connection->udp_rx_sock_handle);
            uv_udp_recv_stop(&connection->udp_tx_sock_handle);
        }

        uv_close((uv_handle_t *) handle, handle_destroyed_completed);
    }
}

bool UdpSockReadWrite::samePort() { return rw_mportTx == rw_mportRx ? true : false; }

bool UdpSockReadWrite::Create(LoopHandle *loops) {
    if (net_conn_state) {
        return false;
    }

    if (reconn_delay.fetch_add(-1) > 0) {
        int count = 0;
        uv_interface_address_t *info;
        uv_interface_addresses(&info, &count);
        uv_free_interface_addresses(info, count);
        COMMON_WARN("wait {} resource is free", reconn_delay.load());
        return false;
    }

    if (cur_loop_handle == nullptr) {
        COMMON_WARN("socket loop handler is nullptr");
        return false;
    }

    COMMON_INFO("Rx {}:{}:{}, Tx {}:{}:{}", rw_server_addr, rw_multiRx_addr, rw_mportRx,
                rw_server_addr, rw_multiTx_addr, rw_mportTx);

    unsigned int flag = multicast_mode ? UV_UDP_REUSEADDR : 0;
    if (rw_mportTx != rw_mportRx) {
        int res = 0;
        if ((res = uv_udp_init(&cur_loop_handle->loop, &udp_rx_sock_handle)) < 0) {
            COMMON_WARN("init udp_sock_handle failed, reason {}", uv_err_name(res));
            reconn_delay = 3;
            return 0;
        }

        if ((res = uv_udp_bind(&udp_rx_sock_handle, (const sockaddr *) &rw_rx_socket, flag)) < 0) {
            COMMON_WARN("bind address failed, reason {}", uv_err_name(res));
            uv_close((uv_handle_t *) &udp_rx_sock_handle, nullptr);
            reconn_delay = 3;
            return 0;
        }

        if (uv_udp_set_multicast_interface(&udp_rx_sock_handle, rw_server_addr.c_str()) < 0){
            COMMON_WARN("uv_udp_set_multicast_interface, reason {}", uv_err_name(res));
        }

        if (multicast_mode) {
            if ((res = uv_udp_set_membership(&udp_rx_sock_handle, rw_multiRx_addr.c_str(), nullptr, UV_JOIN_GROUP)) < 0) {
                COMMON_WARN("set membership failed, reason {}", uv_err_name(res));
                uv_close((uv_handle_t *) &udp_rx_sock_handle, nullptr);
                reconn_delay = 3;
                return 0;
            }

            if ((res = uv_udp_set_multicast_loop(&udp_rx_sock_handle, local_loop_mode)) < 0) {
                COMMON_WARN("set multicast loop, reason {}", uv_err_name(res));
                reconn_delay = 3;
                return 0;
            }
        }

        if (uv_udp_set_multicast_interface(&udp_tx_sock_handle, rw_server_addr.c_str()) < 0){
            COMMON_WARN("uv_udp_set_multicast_interface, reason {}", uv_err_name(res));
        }

        if ((res = uv_udp_init(&cur_loop_handle->loop, &udp_tx_sock_handle)) < 0) {
            COMMON_WARN("init udp_sock_handle failed, reason {}", uv_err_name(res));
            reconn_delay = 3;
            return 0;
        }

        if ((res = uv_udp_bind(&udp_tx_sock_handle, (const sockaddr *) &rw_tx_socket, flag)) < 0) {
            COMMON_WARN("bind address failed, reason {}", uv_err_name(res));
            uv_close((uv_handle_t *) &udp_tx_sock_handle, nullptr);
            reconn_delay = 3;
            return 0;
        }

        if (multicast_mode) {
            if ((res = uv_udp_set_membership(&udp_tx_sock_handle, rw_multiTx_addr.c_str(), nullptr, UV_JOIN_GROUP)) < 0) {
                COMMON_WARN("set membership failed, reason {}", uv_err_name(res));
                uv_close((uv_handle_t *) &udp_tx_sock_handle, nullptr);
                reconn_delay = 3;
                return 0;
            }

            if ((res = uv_udp_set_multicast_loop(&udp_tx_sock_handle, local_loop_mode)) < 0) {
                COMMON_WARN("set multicast loop, reason {}", uv_err_name(res));
                reconn_delay = 3;
                return 0;
            }
        }

        if ((res = uv_async_init(&cur_loop_handle->loop, &destroy_task, destroyed)) < 0) {
            COMMON_WARN("aysnc init destroy task failed, reason {}", uv_err_name(res));
            uv_close((uv_handle_t *) &udp_rx_sock_handle, nullptr);
            uv_close((uv_handle_t *) &udp_tx_sock_handle, nullptr);
            return 0;
        }

        uv_udp_recv_start(&udp_rx_sock_handle, rx_read_alloc_cb, rx_read_recv_cb);
        uv_udp_recv_start(&udp_tx_sock_handle, tx_read_alloc_cb, tx_read_recv_cb);

    } else {
        int res = 0;
        if ((res = uv_udp_init(&cur_loop_handle->loop, &udp_tx_sock_handle)) < 0) {
            COMMON_WARN("init udp_sock_handle failed, reason {}", uv_err_name(res));
            reconn_delay = 3;
            return 0;
        }
        if ((res = uv_udp_bind(&udp_tx_sock_handle, (const sockaddr *) &rw_tx_socket, flag)) < 0) {
            COMMON_WARN("bind address failed, reason {}", uv_err_name(res));
            uv_close((uv_handle_t *) &udp_tx_sock_handle, nullptr);
            reconn_delay = 3;
            return 0;
        }
        if (multicast_mode) {
            if ((res = uv_udp_set_membership(&udp_tx_sock_handle, rw_multiTx_addr.c_str(), nullptr,
                                             UV_JOIN_GROUP)) < 0) {
                COMMON_WARN("set membership failed, reason {}", uv_err_name(res));
                uv_close((uv_handle_t *) &udp_tx_sock_handle, nullptr);
                reconn_delay = 3;
                return 0;
            }
            if ((res = uv_udp_set_multicast_loop(&udp_tx_sock_handle, local_loop_mode)) < 0) {
                COMMON_WARN("set multicast loop, reason {}", uv_err_name(res));
                reconn_delay = 3;
                return 0;
            }
        }

        if ((res = uv_async_init(&cur_loop_handle->loop, &destroy_task, destroyed)) < 0) {
            COMMON_WARN("aysnc init destroy task failed, reason {}", uv_err_name(res));
            uv_close((uv_handle_t *) &udp_tx_sock_handle, nullptr);
            return 0;
        }

        uv_udp_recv_start(&udp_tx_sock_handle, tx_read_alloc_cb, tx_read_recv_cb);
    }


    net_conn_state = true;
    do_destory = false;
    return true;
}

bool UdpSockReadWrite::Initialize(const ChannelConfig &conf, LoopHandle *loop_handle, bool loop) {

    if (loop_handle == nullptr) {
        COMMON_WARN("error input udp_sock_handle: {}", (void *) loop);
        return false;
    }

    if (conf.multi && (conf.multiRxAddr.empty() || conf.multiTxAddr.empty())) {
        COMMON_WARN("multicast address is empty");
        return false;
    }

    int res = UV_ENOMEM;
    if (conf.address.empty() || conf.multiRxPort == 0 || conf.multiRxPort == 65535 ||
        conf.multiTxPort == 0 || conf.multiTxPort == 65535) {
        conf.printString();
        return false;
    }

    multicast_mode = conf.multi;
    cur_loop_handle = loop_handle;
    local_loop_mode = loop;

    rw_mportRx = conf.multiRxPort;
    rw_mportTx = conf.multiTxPort;
    rw_multiTx_addr = conf.multiTxAddr;
    rw_multiRx_addr = conf.multiRxAddr;

    rw_server_addr = conf.address;

    uv_ip4_addr(rw_server_addr.c_str(), rw_mportTx, &rw_tx_socket);
    uv_ip4_addr(rw_server_addr.c_str(), rw_mportRx, &rw_rx_socket);

    uv_ip4_addr(rw_multiTx_addr.c_str(), rw_mportTx, &rw_multiTx_sock);
    uv_ip4_addr(rw_multiRx_addr.c_str(), rw_mportRx, &rw_multiRx_sock);

    return 0;
}

int UdpSockReadWrite::Send(char *data, int size, const char *ip, int port, OnSentCallback send_cb) {
    int status = 0;
    if (net_conn_state) {
        sockaddr_in dst;
        uv_ip4_addr(ip, port, &dst);
        uv_buf_t buff = uv_buf_init(data, size);
        const struct sockaddr *mx = (const struct sockaddr *) &dst;
        status = uv_udp_try_send(&udp_tx_sock_handle, &buff, 1, mx);
        if (status != size) {
            COMMON_WARN("sock send failed, reason {}", uv_err_name(status));
        }
    } else {
        COMMON_WARN("error socket, discard message");
    }

    if (send_cb != nullptr) {
        send_cb(status, data, size);
    }

    return status;
}

bool UdpSockReadWrite::isSelf(sockaddr_in *sockaddr) {
    // uint32_t addr = ntohl(sockaddr->sin_addr.s_addr);
    // uint32_t port = ntohs(sockaddr->sin_port);
    return false;
}

int UdpSockReadWrite::Send(char *data, int size, OnSentCallback send_cb) {
    // int status = 0;
    // if (net_conn_state) {
    //     UdpPackage * reqpack = new UdpPackage(&udp_tx_sock_handle, data, size, send_cb);
    //     if (reqpack == nullptr) {
    //         if (send_cb != nullptr) {
    //             send_cb(status, data, size);
    //         }
    //         return status;
    //     }
    //     reqpack->setAddress(rw_multiTx_sock);
    //     uv_udp_send(&reqpack->request, reqpack->handle,
    //                 &reqpack->buff, 1, (sockaddr *)&reqpack->address, tx_send_callback);
    // }
    // return status;

    int status = 0;
    if (net_conn_state) {
        uv_buf_t buff = uv_buf_init(data, size);
        const struct sockaddr *mx = (const struct sockaddr *) &rw_multiTx_sock;
        status = uv_udp_try_send(&udp_tx_sock_handle, &buff, 1, mx);
        if (status != size) {
            COMMON_WARN("sock send failed, reason {}", uv_err_name(status));
        }
    } else {
        COMMON_WARN("error socket, discard message");
    }
    if (send_cb != nullptr) {
        send_cb(status, data, size);
    }
    return status;
}

bool UdpSockReadWrite::Destroy() {
    bool expect = false;
    if (do_destory.compare_exchange_strong(expect, true)) {
        COMMON_INFO("socket write async destroy {}", (uint64_t) this);
        uv_async_send(&destroy_task);
    }

    return do_destory;
}
