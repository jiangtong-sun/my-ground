#pragma once

#include "uvloop.h"

class COMMON_EXPORT UdpSockReadWrite : public Channel2,
                                       public std::enable_shared_from_this<Channel2> {
  private:
    std::atomic_int reconn_delay = 0;
    std::atomic_bool do_destory = false;
    bool multicast_mode = false;
    std::string rw_server_addr;
    std::string rw_multiRx_addr;
    std::string rw_multiTx_addr;
    uint16_t rw_mportRx;
    uint16_t rw_mportTx;
    uv_udp_t udp_rx_sock_handle;
    uv_udp_t udp_tx_sock_handle;
    sockaddr_in rw_multiRx_sock;
    sockaddr_in rw_multiTx_sock;
    sockaddr_in rw_rx_socket;
    sockaddr_in rw_tx_socket;
    uv_async_t destroy_task;
    uv_buf_t rx_recv_buff;
    uv_buf_t tx_recv_buff;

  public:
    UdpSockReadWrite();
    ~UdpSockReadWrite();

    template <typename Derived>
    std::shared_ptr<Derived> shared_from_base() {
        return std::static_pointer_cast<Derived>(shared_from_this());
    }

    bool isMatch(const std::string &dst, uint16_t port) override {
        return (rw_mportTx == port) && (rw_multiTx_addr.compare(dst) == 0);
    }

    static void tx_send_callback(uv_udp_send_t *req, int status);
    static void tx_read_alloc_cb(uv_handle_t *handle, size_t suggested_size, uv_buf_t *buf);
    static void tx_read_recv_cb(uv_udp_t *handle, ssize_t nread, const uv_buf_t *buf,
                                const struct sockaddr *addr, unsigned flags);
    static void rx_read_alloc_cb(uv_handle_t *handle, size_t suggested_size, uv_buf_t *buf);
    static void rx_read_recv_cb(uv_udp_t *handle, ssize_t nread, const uv_buf_t *buf,
                                const struct sockaddr *addr, unsigned flags);

    static void destroyed_completed(uv_handle_t *handle);
    static void handle_destroyed_completed(uv_handle_t *handle);
    static void destroyed(uv_async_t *handle);

    bool Create(LoopHandle *loop_handle) override;
    std::string chlAddress() override { return rw_server_addr; }
    bool Initialize(const ChannelConfig &conf, LoopHandle *loop_handle, bool loop) override;
    int Send(char *data, int size, const char *ip, int port, OnSentCallback send_cb) override;
    int Send(char *data, int size, OnSentCallback send_cb) override;
    bool isSelf(sockaddr_in *sock);
    bool samePort();
    bool Destroy();
};
