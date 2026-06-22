#pragma once

#include "uvloop.h"

class COMMON_EXPORT UdpSocketSimple
    : public Channel2, public std::enable_shared_from_this<Channel2> {
  private:
    std::string write_server_addr;
    std::string write_multi_addr;
    uint16_t write_port;
    uint16_t write_mport;
    bool multicast_mode;
    std::atomic_bool do_destory = false;
    std::atomic_int reconn_delay = 0;
    sockaddr_in write_norm_sock;
    sockaddr_in write_multi_sock;
    uv_async_t write_destroy_task;
    uv_udp_t write_udp_handle;
    uv_buf_t recv_buff;

  public:
    UdpSocketSimple();
    ~UdpSocketSimple();

    sockaddr *norm_sock() { return (sockaddr *) &write_norm_sock; }
    sockaddr *multi_sock() { return (sockaddr *) &write_multi_sock; }

    template <typename Derived>
    std::shared_ptr<Derived> shared_from_base() {
        return std::static_pointer_cast<Derived>(shared_from_this());
    }

    static void read_alloc_cb(uv_handle_t *handle, size_t suggested_size, uv_buf_t *buf);
    static void read_recv_cb(uv_udp_t *handle, ssize_t nread, const uv_buf_t *buf,
                             const struct sockaddr *addr, unsigned flags);
    static void destroyed_completed(uv_handle_t *handle);
    static void send_destroyed(uv_async_t *handle);
    static void send_handle_destroyed_completed(uv_handle_t *handle);

    bool Create(LoopHandle *loop_handle);
    bool Initialize(const ChannelConfig &conf, LoopHandle *loop_handle, bool loop);
    int Send(char *data, int size, const char *ip, int port, OnSentCallback send_cb);
    int Send(char *data, int size, OnSentCallback send_cb);

    bool Destroy();
};
