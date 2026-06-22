#include "udpsocksimple.h"
#include "commacro.h"
#include "log_category.h"

UdpSocketSimple::UdpSocketSimple()
    : recv_buff(uv_buf_init((char *) malloc(1 << 16), 1 << 16)), write_server_addr(),
      write_multi_addr(), write_port(0), write_mport(0), multicast_mode(false), write_norm_sock(),
      write_multi_sock() {
    write_udp_handle.data = this;
    write_destroy_task.data = this;
}

UdpSocketSimple::~UdpSocketSimple() {
    if (recv_buff.base) {
        free(recv_buff.base);
        recv_buff.base = nullptr;
        recv_buff.len = 0;
    }
}
void UdpSocketSimple::read_alloc_cb(uv_handle_t *udp_sock_handle, size_t size, uv_buf_t *buf) {
    COMMON_ASSERT_HANDLE(udp_sock_handle->data);
    UdpSocketSimple *connection = (UdpSocketSimple *) udp_sock_handle->data;
    if (connection != nullptr) {
        *buf = connection->recv_buff;
    }
}

void UdpSocketSimple::read_recv_cb(uv_udp_t *udp_sock_handle, ssize_t nread, const uv_buf_t *buf,
                                   const struct sockaddr *addr, unsigned flags) {
    COMMON_ASSERT_HANDLE(udp_sock_handle->data);
    // struct sockaddr_in *sockaddr = (struct sockaddr_in *) (addr);
    UdpSocketSimple *connection = reinterpret_cast<UdpSocketSimple *>(udp_sock_handle->data);
    if ((connection != nullptr) && (connection->recv_callback != nullptr) && nread > 0) {
        connection->recv_callback(&connection->write_norm_sock, (uint8_t *) buf->base, nread);
        if (connection->proto_callback != nullptr) {
            connection->data_rx_cache.addData((uint8_t *) buf->base, nread);
            connection->proto_callback(connection->data_rx_cache, addr);
        }
    }
}

void UdpSocketSimple::destroyed_completed(uv_handle_t *handle) {
    COMMON_ASSERT_HANDLE(handle->data);
    UdpSocketSimple *connection = reinterpret_cast<UdpSocketSimple *>(handle->data);
    if ((connection != nullptr) && (connection->close_callback != nullptr)) {
        uv_udp_t *udphandle = reinterpret_cast<uv_udp_t *>(handle);
        COMMON_INFO("socket {} ", udphandle ? udphandle->socket : 0);
        long *add = reinterpret_cast<long *>(connection);
        connection->close_callback(*add);
    }
}

void UdpSocketSimple::send_handle_destroyed_completed(uv_handle_t *handle) {
    COMMON_ASSERT_HANDLE(handle->data);
    UdpSocketSimple *connection = reinterpret_cast<UdpSocketSimple *>(handle->data);
    if (connection != nullptr) {
        COMMON_WARN("stop udp channel: {}", (int) (connection->write_udp_handle.socket));
        uv_close((uv_handle_t *) &connection->write_udp_handle, destroyed_completed);
    }
}

void UdpSocketSimple::send_destroyed(uv_async_t *handle) {
    COMMON_ASSERT_HANDLE(handle->data);
    UdpSocketSimple *connection = reinterpret_cast<UdpSocketSimple *>(handle->data);
    if ((connection != nullptr) && (!connection->write_multi_addr.empty())) {
        uv_udp_set_membership(&connection->write_udp_handle, connection->write_multi_addr.c_str(),
                              NULL, UV_LEAVE_GROUP);
        COMMON_WARN("leave multicast: {}", connection->write_multi_addr.c_str());
    }

    if ((connection != nullptr)) {
        int rtn = uv_udp_recv_stop(&connection->write_udp_handle);
        uv_close((uv_handle_t *) handle, send_handle_destroyed_completed);
    }
}

bool UdpSocketSimple::Create(LoopHandle *loops) {
    if (net_conn_state) {
        return true;
    }

    assert(cur_loop_handle != nullptr);
    if (cur_loop_handle == nullptr) {
        COMMON_WARN("unknown loop handler");
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

    COMMON_INFO("multi address {}:{}, local address {}:{}", write_multi_addr, write_mport,
                write_server_addr, write_port);

    int res = 0;
    if ((res = uv_udp_init(&cur_loop_handle->loop, &write_udp_handle)) < 0) {
        COMMON_WARN("init upd udp_handle failed, reason {}", uv_err_name(res));
        reconn_delay = 3;
        return 0;
    }

    unsigned int flag = multicast_mode ? UV_UDP_REUSEADDR : 0;
    COMMON_INFO("bind send server {}:{}", write_multi_addr, write_mport);

    if ((res = uv_udp_bind(&write_udp_handle, (const sockaddr *) &write_norm_sock, flag)) < 0) {
        COMMON_WARN("bind address failed, reason {}", uv_err_name(res));
        uv_close((uv_handle_t *) &write_udp_handle, nullptr);
        reconn_delay = 3;
        return 0;
    }

    if (multicast_mode) {
        if ((res = uv_udp_set_membership(&write_udp_handle, write_multi_addr.c_str(), nullptr,
                                         UV_JOIN_GROUP)) < 0) {
            COMMON_WARN("set membership failed, reason {}", uv_err_name(res));
            uv_close((uv_handle_t *) &write_udp_handle, nullptr);
            reconn_delay = 3;
            return 0;
        }

        if ((res = uv_udp_set_multicast_loop(&write_udp_handle, local_loop_mode)) < 0) {
            COMMON_WARN("set multicast loop, reason {}", uv_err_name(res));
            reconn_delay = 3;
            return 0;
        }
    }

    if ((res = uv_async_init(&cur_loop_handle->loop, &write_destroy_task, send_destroyed)) < 0) {
        COMMON_WARN("aysnc destroy task failed, reason {}", uv_err_name(res));
        reconn_delay = 3;
        return 0;
    }

    uv_udp_recv_start(&write_udp_handle, read_alloc_cb, read_recv_cb);
    net_conn_state = true;
    do_destory = false;
    return 0;
}

bool UdpSocketSimple::Initialize(const ChannelConfig &conf, LoopHandle *loop_handle, bool loop) {

    if (loop_handle == nullptr) {
        COMMON_WARN("error input udp_handle: {}", (void *) loop);
        return false;
    }

    if (conf.multi && conf.multiTxAddr.empty()) {
        COMMON_WARN("multicast address is empty");
        return false;
    }

    int res = UV_ENOMEM;
    if (conf.address.empty() || conf.multiTxPort == 0 || conf.multiTxPort == 65535) {
        conf.printString();
        return false;
    }

    cur_loop_handle = loop_handle;
    local_loop_mode = loop;

    write_port = conf.multiTxPort;
    write_mport = conf.multiTxPort;

    write_multi_addr = conf.multiTxAddr;
    write_server_addr = conf.address;

    multicast_mode = conf.multi;

    uv_ip4_addr(write_server_addr.c_str(), write_port, &write_norm_sock);
    uv_ip4_addr(write_multi_addr.c_str(), write_mport, &write_multi_sock);

    net_conn_state = false;
    return true;
}

int UdpSocketSimple::Send(char *data, int size, const char *ip, int port, OnSentCallback send_cb) {
    int status = 0;
    if (net_conn_state) {
        sockaddr_in dst;
        uv_ip4_addr(ip, port, &dst);
        uv_buf_t buff = uv_buf_init(data, size);
        const struct sockaddr *mx = (const struct sockaddr *) &dst;
        status = uv_udp_try_send(&write_udp_handle, &buff, 1, mx);
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

int UdpSocketSimple::Send(char *data, int size, OnSentCallback send_cb) {
    int status = 0;
    if (net_conn_state) {
        uv_buf_t buff = uv_buf_init(data, size);
        status = uv_udp_try_send(&write_udp_handle, &buff, 1,
                                 multicast_mode ? (const struct sockaddr *) &write_multi_sock
                                                : (const struct sockaddr *) &write_norm_sock);
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

bool UdpSocketSimple::Destroy() {
    bool expect = false;
    if (do_destory.compare_exchange_strong(expect, true)) {
        COMMON_INFO("socket write async destroy {}", (uint64_t) this);
        uv_async_send(&write_destroy_task);
    }
    return do_destory;
}
