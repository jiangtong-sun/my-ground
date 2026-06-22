#include "protochannel.h"

int Channel2::Send(char *data, int size, OnSentCallback send_cb) { return 0; }

int Channel2::Send(char *data, int size, const char *ip, int port, OnSentCallback cb) { return 0; }

bool Channel2::Initialize(const ChannelConfig &conf, LoopHandle *loop_handle, bool loop) {
    return false;
}

bool Channel2::Create(LoopHandle *loop_handle) { return false; }
