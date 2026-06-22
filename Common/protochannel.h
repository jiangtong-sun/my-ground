#pragma once

#include "networkdata.h"
#include "uncopyable.h"
#include <atomic>

class LoopHandle;

class COMMON_EXPORT Channel2 : public Uncopyable {
  protected:
    ProtoDataCache data_rx_cache;
    ProtoDataCache data_tx_cache;
    bool local_loop_mode = false;
    uint16_t channel_code = 0;
    uint16_t min_proto_len = 16;
    std::atomic_bool net_conn_state = false;
    OnCloseCompleted close_callback = nullptr;
    OnRecvCallback recv_callback = nullptr;
    OnProtoCallback proto_callback = nullptr;
    LoopHandle *cur_loop_handle = nullptr;

  public:
    bool IsConnected() { return net_conn_state; }
    bool IsLocalLoop() { return local_loop_mode; }

    OnCloseCompleted GetCloseCallback() { return close_callback; }
    void SetCloseCallback(OnCloseCompleted code) { close_callback = std::move(code); }

    OnProtoCallback GetDecoder() { return proto_callback; }
    void SetDecoder(OnProtoCallback code) { proto_callback = std::move(code); }

    OnRecvCallback GetRecvCallback() { return recv_callback; }
    void SetRecvCallback(OnRecvCallback code) { recv_callback = std::move(code); }

    uint16_t GetMinProtoLen() { return min_proto_len; }
    void SetMinProtoLen(uint16_t code) { min_proto_len = code; }

    uint16_t ChannelCode() { return channel_code; }
    void SetChannelCode(uint16_t code) { channel_code = code; }

    virtual bool isMatch(const std::string &dst, uint16_t port) { return false; }
    virtual int Send(char *data, int size, OnSentCallback send_cb);
    virtual int Send(char *data, int size, const char *ip, int port, OnSentCallback cb);
    virtual bool Initialize(const ChannelConfig &conf, LoopHandle *loop_handle, bool loop);
    virtual bool Create(LoopHandle *loop_handle);
    virtual bool Destory() { return false; }
    virtual std::string chlAddress() { return ""; }

    uint64_t Refence() { return uint64_t(this); }
};
