#pragma once

#include "commacro.h"
#include "protochannel.h"
#include "uncopyable.h"
#include "uv.h"

#include <atomic>
#include <stdint.h>

struct COMMON_EXPORT UdpPackage {
    uv_udp_t *handle;
    uv_udp_send_t request;
    OnSentCallback callback;
    sockaddr_in address;
    uv_buf_t buff;

    void setAddress(sockaddr_in addr) { address = addr; }
    UdpPackage() : callback(), handle(nullptr), buff(uv_buf_init(nullptr, 0)) {}
    UdpPackage(uv_udp_t *h, char *data, uint16_t size, OnSentCallback cb)
        : handle(h), callback(cb), buff(uv_buf_init(data, size)) {
        request.data = this;
    }
};

class COMMON_EXPORT LoopHandle : public Uncopyable {
  public:
    uv_loop_t loop;

  private:
    std::atomic<int> status = 0;
    std::string by_file_record_res;
    bool wait_resource_to_release;
    uv_async_t *_async_destroy;
    uv_thread_t _thread;

  public:
    LoopHandle();
    ~LoopHandle();
    int Initialize();
    bool AsyncRun(const char *by_file_record_res);
    bool SyncRun(const char *file_to_record_res_info);
    void Destroy();
};
