#pragma once

#include "commacro.h"
#include <functional>
#include <memory>

#define MaxZmqPackageCache 512
#define NetworkSerailCache 8192
#define MaxPackageCache 8192
#define PROTO_PACKAGE_LEN 65536

struct COMMON_EXPORT ChannelConfig {
    bool multi;
    int netPort;
    int multiRxPort;
    int multiTxPort;
    int dataTransFreq;
    std::string address;
    std::string multiRxAddr;
    std::string multiTxAddr;

    ChannelConfig();

    void printString() const;
};

struct COMMON_EXPORT ProtoDataCache {
    int length = 0;
    int lastPos = 0;
    uint8_t *data = nullptr;
    std::shared_ptr<uint8_t> buffer = nullptr;

    void addData(const uint8_t *cache, int nread);
    int checkLength(int set);
    void reset();

    ~ProtoDataCache() = default;
    ProtoDataCache();
};

using OnSentCallback = std::function<void(int status, char *buf, int len)>;
using OnRecvCallback = std::function<void(const void *adr, const uint8_t *buf, int len)>;
using OnProtoCallback = std::function<void(ProtoDataCache &, const void *addr)>;
using OnCloseCompleted = std::function<void(long)>;
