#pragma once

#include "uvloop.h"

class COMMON_EXPORT UdpSocketR : public Channel2, public std::enable_shared_from_this<Channel2> {
  private:
    std::string recv_server_addr;
    std::string recv_multicast;
    bool multicast_mode = false;
    std::atomic_int reconn_delay = 0;
    std::atomic_bool do_destory = false;
    sockaddr_in recv_norm_sock;
    sockaddr_in recv_multi_sock;
    uint16_t recv_port;
    uint16_t recv_mport;
    uint16_t last_pos;
    uv_buf_t recv_buff;
    uv_buf_t cache_buff;
    uv_udp_t recv_udp_handle;
    uv_async_t recv_destroy_task;

  public:
    UdpSocketR();
    ~UdpSocketR();

    sockaddr *norm_sock() { return (sockaddr *) &recv_norm_sock; }
    sockaddr *multi_sock() { return (sockaddr *) &recv_multi_sock; }

    template <typename Derived>
    std::shared_ptr<Derived> shared_from_base() {
        return std::static_pointer_cast<Derived>(shared_from_this());
    }

    static void read_alloc_cb(uv_handle_t *handle, size_t suggested_size, uv_buf_t *buf);

    static void read_recv_cb(uv_udp_t *handle, ssize_t nread, const uv_buf_t *buf,
                             const struct sockaddr *addr, unsigned flags);

    static void destroyed_completed(uv_handle_t *handle);
    static void recv_destroyed(uv_async_t *handle);
    static void recv_handle_destroyed_completed(uv_handle_t *handle);

    bool Create(LoopHandle *loop_handle);
    bool Initialize(const ChannelConfig &conf, LoopHandle *loop_handle, bool loop);
    bool Destroy();
};
