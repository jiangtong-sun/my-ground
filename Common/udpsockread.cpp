#include "udpsockread.h"

#include "commacro.h"
#include "log_category.h"

UdpSocketR::UdpSocketR()
    : recv_buff(uv_buf_init((char *) malloc(1 << 12), 1 << 12)),
      cache_buff(uv_buf_init((char *) malloc(1 << 12), 1 << 12)), recv_multicast(),
      multicast_mode(false), recv_port(0), recv_mport(0), do_destory(false), last_pos(0) {
    recv_udp_handle.data = this;
    recv_destroy_task.data = this;
}

UdpSocketR::~UdpSocketR() {
    recv_buff.len = 0;
    cache_buff.len = 0;
    COMMON_RESET_HANDLE(recv_buff.base);
    COMMON_RESET_HANDLE(cache_buff.base);
}

void UdpSocketR::read_alloc_cb(uv_handle_t *udp_handle, size_t suggested_size, uv_buf_t *buf) {
    UdpSocketR *connection = (UdpSocketR *) udp_handle->data;
    if (connection != nullptr) {
        buf->base = connection->recv_buff.base;
        buf->len = connection->recv_buff.len;
    }
}

void UdpSocketR::read_recv_cb(uv_udp_t *udp_handle, ssize_t nread, const uv_buf_t *buf,
                              const struct sockaddr *addr, unsigned flags) {
    COMMON_ASSERT_HANDLE(udp_handle->data);
    UdpSocketR *conn = reinterpret_cast<UdpSocketR *>(udp_handle->data);
    if ((conn != nullptr) && (conn->recv_callback != nullptr)) {
        // struct sockaddr_in *sockaddr = (struct sockaddr_in *) (addr);
        conn->data_rx_cache.addData((uint8_t *) buf->base, nread);
        if (conn->recv_callback != nullptr) {
            conn->recv_callback(&conn->recv_norm_sock, (uint8_t *) buf->base, nread);
        }
        if (conn->proto_callback != nullptr) {
            conn->proto_callback(conn->data_rx_cache, addr);
        }
    }
}

void UdpSocketR::destroyed_completed(uv_handle_t *handle) {
    COMMON_ASSERT_HANDLE(handle->data);
    UdpSocketR *connection = reinterpret_cast<UdpSocketR *>(handle->data);
    if ((connection != nullptr) && (connection->close_callback != nullptr)) {
        uv_udp_t *udphandle = reinterpret_cast<uv_udp_t *>(handle);
        COMMON_INFO("socket {} ", udphandle ? udphandle->socket : 0);
        long *add = reinterpret_cast<long *>(connection);
        connection->close_callback(*add);
    }
}

void UdpSocketR::recv_handle_destroyed_completed(uv_handle_t *handle) {
    COMMON_ASSERT_HANDLE(handle->data);
    UdpSocketR *connection = reinterpret_cast<UdpSocketR *>(handle->data);
    if (connection != nullptr) {
        COMMON_INFO("stop udp channel: {}", (int) (connection->recv_udp_handle.socket));
        uv_close((uv_handle_t *) &connection->recv_udp_handle, destroyed_completed);
    }
}

void UdpSocketR::recv_destroyed(uv_async_t *handle) {
    COMMON_ASSERT_HANDLE(handle->data);
    UdpSocketR *connection = reinterpret_cast<UdpSocketR *>(handle->data);
    if ((connection != nullptr) && (!connection->recv_multicast.empty())) {
        uv_udp_set_membership(&connection->recv_udp_handle, connection->recv_multicast.c_str(),
                              NULL, UV_LEAVE_GROUP);
        COMMON_WARN("leave multicast: {}", connection->recv_multicast.c_str());
    }

    if ((connection != nullptr)) {
        uv_udp_recv_stop(&connection->recv_udp_handle);
        uv_close((uv_handle_t *) handle, recv_handle_destroyed_completed);
    }
}

bool UdpSocketR::Create(LoopHandle *loops) {

    if (net_conn_state) {
        return true;
    }

    // create socket is failed, then destroy resource
    if (reconn_delay.fetch_add(-1) > 0) {
        int count = 0;
        uv_interface_address_t *info;
        uv_interface_addresses(&info, &count);
        uv_free_interface_addresses(info, count);
        COMMON_WARN("wait {} resource is free", reconn_delay.load());
        return false;
    }

    COMMON_INFO("multi address {}:{}, service address {}:{}", recv_multicast, recv_mport,
                recv_server_addr, recv_port);

    int res = 0;
    if ((res = uv_udp_init(&cur_loop_handle->loop, &recv_udp_handle)) < 0) {
        COMMON_WARN("init udp_handle failed, reason {}", uv_err_name(res));
        reconn_delay = 3;
        return res;
    }

    unsigned int flag = multicast_mode ? UV_UDP_REUSEADDR : 0;
    if ((res = uv_udp_bind(&recv_udp_handle, (const sockaddr *) &recv_norm_sock, flag)) < 0) {
        COMMON_WARN("bind address failed, reason {}", uv_err_name(res));
        uv_close((uv_handle_t *) &recv_udp_handle, nullptr);
        reconn_delay = 3;
        return false;
    }

    if (multicast_mode) {

        if ((res = uv_udp_set_membership(&recv_udp_handle, recv_multicast.c_str(), nullptr,
                                         UV_JOIN_GROUP)) < 0) {
            COMMON_WARN("set membership failed, reason {}", uv_err_name(res));
            uv_close((uv_handle_t *) &recv_udp_handle, nullptr);
            reconn_delay = 3;
            return res;
        }

        if ((res = uv_udp_set_multicast_loop(&recv_udp_handle, local_loop_mode)) < 0) {
            COMMON_WARN("set multicast loop, reason {}", uv_err_name(res));
            reconn_delay = 3;
            return res;
        }
    }

    if ((res = uv_async_init(&cur_loop_handle->loop, &recv_destroy_task, recv_destroyed)) < 0) {
        COMMON_WARN("aysnc destroy task failed, reason {}", uv_err_name(res));
        do_destory = true;
        reconn_delay = 3;
        return res;
    }

    uv_udp_recv_start(&recv_udp_handle, read_alloc_cb, read_recv_cb);
    net_conn_state = true;
    do_destory = false;
    return 0;
}

bool UdpSocketR::Initialize(const ChannelConfig &conf, LoopHandle *loop_handle, bool loop) {

    if (loop_handle == nullptr) {
        COMMON_WARN("error input udp_handle: {}", (void *) loop);
        return false;
    }

    if (conf.multi && (conf.multiRxAddr.empty() || conf.multiTxAddr.empty())) {
        COMMON_WARN("multicast address is empty");
        return false;
    }

    int res = UV_ENOMEM;
    if (conf.address.empty() || conf.multiRxPort == 0 || conf.multiRxPort == 65535) {
        conf.printString();
        return false;
    }

    cur_loop_handle = loop_handle;
    local_loop_mode = loop;
    net_conn_state = false;

    recv_port = conf.multiRxPort;
    recv_mport = conf.multiRxPort;

    multicast_mode = conf.multi;

    recv_server_addr = conf.address;
    recv_multicast = conf.multiRxAddr;

    uv_ip4_addr(recv_multicast.c_str(), recv_mport, &recv_multi_sock);
    uv_ip4_addr(recv_server_addr.c_str(), recv_port, &recv_norm_sock);
    return true;
}

bool UdpSocketR::Destroy() {
    bool expect = false;
    if (do_destory.compare_exchange_strong(expect, true)) {
        COMMON_INFO("socket recv async destroy {}", (uint64_t) this);
        uv_async_send(&recv_destroy_task);
    }
    return do_destory;
}
