#include "networkdata.h"
#include "log_category.h"

ChannelConfig::ChannelConfig() {
    multi = true;
    multiRxPort = 10002;
    multiTxPort = 10004;
    address = "127.0.0.1";
    multiRxAddr = "239.3.3.9";
    multiTxAddr = "239.3.3.9";
}

void ChannelConfig::printString() const {
    COMMON_WARN("service {}, multi Tx {}:{}, multi Rx {}:{} ", address.c_str(), multiTxAddr.c_str(),
                multiTxPort, multiRxAddr.c_str(), multiRxPort);
}

void ProtoDataCache::addData(const uint8_t *cache, int nread) {
    if ((nread >= 0) && (cache != nullptr) && (length + nread < PROTO_PACKAGE_LEN)) {
        memcpy(data + length, cache, nread);
        length += nread;
    } else {
        COMMON_WARN("each package overflow {} {}", length, nread);
        nread = nread > PROTO_PACKAGE_LEN ? PROTO_PACKAGE_LEN : nread;
        memcpy(data, cache, nread);
        length = nread;
    }
}

int ProtoDataCache::checkLength(int set) {

    if (length + set > PROTO_PACKAGE_LEN) {
        int diff = length + set - PROTO_PACKAGE_LEN;
        COMMON_WARN("discard data, length: {}, set: {}, capcity: {}", length, set, PROTO_PACKAGE_LEN);
        memmove(data, data + diff, length - diff);
        lastPos = 0; //
        length -= diff;
    }

    return length;
}

ProtoDataCache::ProtoDataCache() {
    buffer = std::shared_ptr<uint8_t>(new uint8_t[PROTO_PACKAGE_LEN]);
    memset(buffer.get(), 0, PROTO_PACKAGE_LEN);
    data = buffer.get();
    length = 0;
    lastPos = 0;
}

void ProtoDataCache::reset() {
    lastPos = 0;
    length = 0;
}
