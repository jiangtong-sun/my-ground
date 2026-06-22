#pragma once

#include "commacro.h"
#include <string>

struct COMMON_EXPORT ProtoMessage {
    uint16_t port = 0;
    uint16_t length = 0;
    uint16_t channel = 0;
    uint16_t select = 0;
    uint32_t address = 0;
    uint8_t buffer[132];
};

typedef ProtoMessage *PtrMessage;

COMMON_EXPORT void deleteProtoMessage(ProtoMessage *&);
COMMON_EXPORT ProtoMessage *newProtoMessage(uint32_t channel, const uint8_t *buf, int len);

// template <class T>
// T *GAllocate() {
//     char *buffer = (char *) calloc(1, sizeof(T));
//     T *msg = reinterpret_cast<T *>(buffer);
//     return msg;
// }

COMMON_EXPORT std::string base64_encode(const char *bytes_to_encode, unsigned int in_len);
COMMON_EXPORT std::string base64_decode(const char *encoded_string, unsigned int in_len);
