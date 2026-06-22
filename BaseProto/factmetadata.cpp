#include "factmetadata.h"
#include "log_category.h"
#include <QRegularExpression>

// (uint8_t)(data[0] & 0xff)

static int Translator8(TransMetaData *type, const uint8_t *data, int chls,
                       ReferDeviceCallback refer) {
    assert(type != nullptr);
    assert(data != nullptr);
    {
        uint8_t value = data[0] & 0xff;
        type->protoTrans(value, chls, refer);
    }
    return 1;
}

// (((uint16_t)(data[0] & 0x0f) << 8) + (data[1] & 0xff))

static int Translator308(TransMetaData *type, const uint8_t *data, int chls,
                         ReferDeviceCallback refer) {
    assert(type != nullptr);
    assert(data != nullptr);
    {
        uint16_t value = (((uint16_t)(data[0] & 0x0f) << 8) + (data[1] & 0xff));
        type->protoTrans(value, chls, refer);
    }
    return 2;
}

// (((uint16_t)((data[0] >> 4) & 0xF) << 8) + (data[1] & 0xff))

static int Translator748(TransMetaData *type, const uint8_t *data, int chls,
                         ReferDeviceCallback refer) {
    assert(type != nullptr);
    assert(data != nullptr);
    {
        uint16_t value = (((uint16_t)((data[0] >> 4) & 0xF) << 8) + (data[1] & 0xff));
        type->protoTrans(value, chls, refer);
    }
    return 2;
}

// return (((uint16_t)(data[0] & 0xff) << 4) + ((data[1] >> 4) & 0xf))

static int Translator874(TransMetaData *type, const uint8_t *data, int chls,
                         ReferDeviceCallback refer) {
    assert(type != nullptr);
    assert(data != nullptr);
    {
        uint16_t value = (((uint16_t)(data[0] & 0xff) << 4) + ((data[1] >> 4) & 0xf));
        type->protoTrans(value, chls, refer);
    }
    return 1;
}

// (((uint16_t)(data[0] & 0xff) << 4) + (data[1] & 0xf))

static int Translator830(TransMetaData *type, const uint8_t *data, int chls,
                         ReferDeviceCallback refer) {
    assert(type != nullptr);
    assert(data != nullptr);
    {
        uint16_t value = (((uint16_t)(data[0] & 0xff) << 4) + (data[1] & 0xf));
        type->protoTrans(value, chls, refer);
    }
    return 1;
}

// (((uint32_t)data[2] << 24) +
//           ((uint32_t)data[2] << 16) +
//           ((uint32_t)data[1] << 8) + (uint32_t)data[0]);

static int Translator32d(TransMetaData *type, const uint8_t *data, int chls,
                         ReferDeviceCallback refer) {
    assert(type != nullptr);
    assert(data != nullptr);
    {
        uint32_t value = (((uint32_t) data[2] << 24) + ((uint32_t) data[2] << 16) +
                          ((uint32_t) data[1] << 8) + (uint32_t) data[0]);
        type->protoTrans(value, chls, refer);
    }
    return 4;
}

// (((uint32_t)data[2] << 16) + ((uint32_t)data[1] << 8) + (uint32_t)data[0]);

static int Translator24(TransMetaData *type, const uint8_t *data, int chls,
                        ReferDeviceCallback refer) {
    assert(type != nullptr);
    assert(data != nullptr);
    {
        uint32_t value =
            (((uint32_t) data[2] << 16) + ((uint32_t) data[1] << 8) + (uint32_t) data[0]);
        type->protoTrans(value, chls, refer);
    }
    return 3;
}

// (((uint16_t)(data[1] & 0xff) << 8) + (data[0] & 0xff))

static int Translator16(TransMetaData *type, const uint8_t *data, int chls,
                        ReferDeviceCallback refer) {
    assert(type != nullptr);
    assert(data != nullptr);
    {
        uint16_t value = (((uint16_t)(data[1] & 0xff) << 8) + (data[0] & 0xff));
        type->protoTrans(value, chls, refer);
    }
    return 2;
}

// (((uint32_t)data[0] << 24) +
//  ((uint32_t)data[1] << 16) +
//  ((uint32_t)data[2] << 8) + data[3]);

static int TranslatorN32d(TransMetaData *type, const uint8_t *data, int chls,
                          ReferDeviceCallback refer) {
    assert(type != nullptr);
    assert(data != nullptr);
    {
        uint32_t value = (((uint32_t) data[0] << 24) + ((uint32_t) data[1] << 16) +
                          ((uint32_t) data[2] << 8) + data[3]);
        type->protoTrans(value, chls, refer);
    }
    return 4;
}

// ((uint32_t)(data[0] << 16) + ((uint32_t)data[1] << 8) + data[2])

static int TranslatorN24(TransMetaData *type, const uint8_t *data, int chls,
                         ReferDeviceCallback refer) {
    assert(type != nullptr);
    assert(data != nullptr);
    {
        uint32_t value = ((uint32_t)(data[0] << 16) + ((uint32_t) data[1] << 8) + data[2]);
        type->protoTrans(value, chls, refer);
    }
    return 3;
}

// (((uint16_t)(data[0] & 0xff) << 8) + (data[1] & 0xff))
static int TranslatorN16(TransMetaData *type, const uint8_t *data, int chls,
                         ReferDeviceCallback refer) {
    assert(type != nullptr);
    assert(data != nullptr);
    {
        uint16_t value = (((uint16_t)(data[0] & 0xff) << 8) + (data[1] & 0xff));
        type->protoTrans(value, chls, refer);
    }
    return 2;
}

//  data[0] & 0x1
static int Translator0(TransMetaData *type, const uint8_t *data, int chls,
                       ReferDeviceCallback refer) {
    assert(type != nullptr);
    assert(data != nullptr);
    uint8_t value = data[0] & 0x1;
    type->protoTrans(value, chls, refer);
    return 0;
}
// (data[0] & 0x2)>>1
static int Translator1(TransMetaData *type, const uint8_t *data, int chls,
                       ReferDeviceCallback refer) {
    assert(type != nullptr);
    assert(data != nullptr);
    uint8_t value = data[0] & 0x2;
    type->protoTrans(value >> 1, chls, refer);
    return 0;
}

// (data[0] & 0x4)>>2
static int Translator2(TransMetaData *type, const uint8_t *data, int chls,
                       ReferDeviceCallback refer) {
    assert(type != nullptr);
    assert(data != nullptr);
    uint8_t value = data[0] & 0x4;
    type->protoTrans(value >> 2, chls, refer);
    return 0;
}

// (data[0] & 0x8)>>3
static int Translator3(TransMetaData *type, const uint8_t *data, int chls,
                       ReferDeviceCallback refer) {
    assert(type != nullptr);
    assert(data != nullptr);
    uint8_t value = data[0] & 0x8;
    type->protoTrans(value >> 3, chls, refer);
    return 0;
}

// (data[0] & 0x10)>>4
static int Translator4(TransMetaData *type, const uint8_t *data, int chls,
                       ReferDeviceCallback refer) {
    assert(type != nullptr);
    assert(data != nullptr);
    uint8_t value = data[0] & 0x10;
    type->protoTrans(value >> 4, chls, refer);
    return 0;
}

// (data[0] & 0x20)>>5
static int Translator5(TransMetaData *type, const uint8_t *data, int chls,
                       ReferDeviceCallback refer) {
    assert(type != nullptr);
    assert(data != nullptr);
    uint8_t value = data[0] & 0x20;
    type->protoTrans(value >> 5, chls, refer);
    return 0;
}

// (data[0] & 0x40)>>6
static int Translator6(TransMetaData *type, const uint8_t *data, int chls,
                       ReferDeviceCallback refer) {
    assert(type != nullptr);
    assert(data != nullptr);
    uint8_t value = data[0] & 0x40;
    type->protoTrans(value >> 6, chls, refer);
    return 0;
}

// (data[0] & 0x80)>>7
static int Translator7(TransMetaData *type, const uint8_t *data, int chls,
                       ReferDeviceCallback refer) {
    assert(type != nullptr);
    assert(data != nullptr);
    uint8_t value = data[0] & 0x80;
    type->protoTrans(value >> 7, chls, refer);
    return 1;
}

// data[0] & 0x3
static int Translator10(TransMetaData *type, const uint8_t *data, int chls,
                        ReferDeviceCallback refer) {
    assert(type != nullptr);
    assert(data != nullptr);
    uint8_t value = data[0] & 0x3;
    type->protoTrans(value, chls, refer);
    return 0;
}

// data[0] & 0x7
static int Translator20(TransMetaData *type, const uint8_t *data, int chls,
                        ReferDeviceCallback refer) {
    assert(type != nullptr);
    assert(data != nullptr);
    uint8_t value = data[0] & 0x7;
    type->protoTrans(value, chls, refer);
    return 0;
}

// (data[0] >> 2) & 0x3
static int Translator32(TransMetaData *type, const uint8_t *data, int chls,
                        ReferDeviceCallback refer) {
    assert(type != nullptr);
    assert(data != nullptr);
    uint8_t value = (data[0] >> 2) & 0x3;
    type->protoTrans(value, chls, refer);
    return 0;
}

// (data[0] >> 3) & 0x3
static int Translator43(TransMetaData *type, const uint8_t *data, int chls,
                        ReferDeviceCallback refer) {
    assert(type != nullptr);
    assert(data != nullptr);
    uint8_t value = (data[0] >> 3) & 0x3;
    type->protoTrans(value, chls, refer);
    return 0;
}

// (data[0] >> 4) & 0x3
static int Translator54(TransMetaData *type, const uint8_t *data, int chls,
                        ReferDeviceCallback refer) {
    assert(type != nullptr);
    assert(data != nullptr);
    uint8_t value = (data[0] >> 4) & 0x3;
    type->protoTrans(value, chls, refer);
    return 0;
}

// (data[0] >> 4) & 0x7
static int Translator64(TransMetaData *type, const uint8_t *data, int chls,
                        ReferDeviceCallback refer) {
    assert(type != nullptr);
    assert(data != nullptr);
    uint8_t value = (data[0] >> 4) & 0x7;
    type->protoTrans(value, chls, refer);
    return 0;
}

// (data[0] >> 2) & 0xf
static int Translator52(TransMetaData *type, const uint8_t *data, int chls,
                        ReferDeviceCallback refer) {
    assert(type != nullptr);
    assert(data != nullptr);
    uint8_t value = (data[0] >> 2) & 0xf;
    type->protoTrans(value, chls, refer);
    return 0;
}

// data[0] & 0x0f
static int Translator30(TransMetaData *type, const uint8_t *data, int chls,
                        ReferDeviceCallback refer) {
    assert(type != nullptr);
    assert(data != nullptr);
    uint8_t value = data[0] & 0x0f;
    type->protoTrans(value, chls, refer);
    return 0;
}

// (data[0] >> 2) & 0x7
static int Translator42(TransMetaData *type, const uint8_t *data, int chls,
                        ReferDeviceCallback refer) {
    assert(type != nullptr);
    assert(data != nullptr);
    uint8_t value = (data[0] >> 2) & 0x7;
    type->protoTrans(value, chls, refer);
    return 0;
}

// (data[0] >> 6) & 0x3
static int Translator76(TransMetaData *type, const uint8_t *data, int chls,
                        ReferDeviceCallback refer) {
    assert(type != nullptr);
    assert(data != nullptr);
    uint8_t value = (data[0] >> 6) & 0x3;
    type->protoTrans(value, chls, refer);
    return 1;
}

// (data[0] >> 4) & 0xF
static int Translator74(TransMetaData *type, const uint8_t *data, int chls,
                        ReferDeviceCallback refer) {
    assert(type != nullptr);
    assert(data != nullptr);
    uint8_t value = (data[0] >> 4) & 0xF;
    type->protoTrans(value, chls, refer);
    return 1;
}

// (data[0]>>5) & 0x7
static int Translator75(TransMetaData *type, const uint8_t *data, int chls,
                        ReferDeviceCallback refer) {
    assert(type != nullptr);
    assert(data != nullptr);
    uint8_t value = (data[0] >> 5) & 0x7;
    type->protoTrans(value, chls, refer);
    return 1;
}
// (data[0] & 0xFC)>>2
static int Translator72(TransMetaData *type, const uint8_t *data, int chls,
                        ReferDeviceCallback refer) {
    assert(type != nullptr);
    assert(data != nullptr);
    uint8_t value = data[0] & 0xFC;
    type->protoTrans(value >> 2, chls, refer);
    return 1;
}
// undefine
static int Translator64d(TransMetaData *type, const uint8_t *data, int chls,
                         ReferDeviceCallback refer) {
    assert(type != nullptr);
    assert(data != nullptr);
    return 8;
}
// rechecking...
// ((data[0] & 0x03) << 1) + ((data[1] & 0x80) >> 7)
static int Translator107(TransMetaData *type, const uint8_t *data, int chls,
                         ReferDeviceCallback refer) {
    assert(type != nullptr);
    assert(data != nullptr);
    uint8_t value = ((data[0] & 0x03) << 1) + ((data[1] & 0x80) >> 7);
    type->protoTrans(value, chls, refer);
    return 0;
}

// data[0] & 0x1 == 取字节0Bit（uint8_t的Bit序列反向） == 下一个Byte
static int TranslatorN0(TransMetaData *type, const uint8_t *data, int chls,
                        ReferDeviceCallback refer) {
    assert(type != nullptr);
    assert(data != nullptr);
    uint8_t value = data[0] & 0x1;
    type->protoTrans(value, chls, refer);
    return 1;
}

// (data[0] & 0x80) >> 7  == 取字节7Bit（uint8_t的Bit序列反向）== 当前Byte
static int TranslatorN7(TransMetaData *type, const uint8_t *data, int chls,
                        ReferDeviceCallback refer) {
    assert(type != nullptr);
    assert(data != nullptr);
    uint8_t value = data[0] & 0x80;
    type->protoTrans(value >> 7, chls, refer);
    return 0;
}

// (data[0] & 0xE0) >> 5 == 取字节7Bit（uint8_t的Bit序列反向）== 当前Byte
static int TranslatorN75(TransMetaData *type, const uint8_t *data, int chls,
                         ReferDeviceCallback refer) {
    assert(type != nullptr);
    assert(data != nullptr);
    uint8_t value = data[0] & 0xE0;
    type->protoTrans(value >> 5, chls, refer);
    return 0;
}

// signed == data[0] == int8_t
// unsigned == data[0] == uint8_t
static int MsbTranslator8(TransMetaData *type, const uint8_t *data, int chls,
                          ReferDeviceCallback refer) {
    assert(type != nullptr);
    assert(data != nullptr);
    if (type->metaDataType() == signedType) {
        int8_t value = data[0];
        type->protoTrans(value, chls, refer);
    } else {
        uint8_t value = data[0];
        type->protoTrans(value, chls, refer);
    }
    return 1;
}

// signed == data[0] == int16_t
// unsigned == data[0] == uint16_t
static int MsbTranslator16(TransMetaData *type, const uint8_t *data, int chls,
                           ReferDeviceCallback refer) {
    assert(type != nullptr);
    assert(data != nullptr);
    if (type->metaDataType() == signedType) {
        int16_t *value = (int16_t *) (data);
        type->protoTrans(*value, chls, refer);
    } else {
        uint16_t *value = (uint16_t *) (data);
        type->protoTrans(*value, chls, refer);
    }
    return 2;
}

// data[2] 最高位Bit为符号位置
// uint32_t = (((uint32_t)data[2] << 16) + ((uint32_t)data[1] << 8) + data[0])
// int32_t = ±(((uint32_t)(data[2] & 0x7f) << 16) + ((uint32_t)data[1] << 8) + data[0])
static int MsbTranslator24(TransMetaData *type, const uint8_t *data, int chls,
                           ReferDeviceCallback refer) {
    assert(type != nullptr);
    assert(data != nullptr);
    if (type->metaDataType() == signedType) {
        int32_t value =
            (data[2] & 0x80)
                ? -(((uint32_t)(data[2] & 0x7f) << 16) + ((uint32_t) data[1] << 8) + data[0])
                : (((uint32_t)(data[2] & 0x7f) << 16) + ((uint32_t) data[1] << 8) + data[0]);
        type->protoTrans(value, chls, refer);
    } else {
        uint32_t value = (((uint32_t) data[2] << 16) + ((uint32_t) data[1] << 8) + data[0]);
        type->protoTrans(value, chls, refer);
    }
    return 3;
}

// data[0] 最高位Bit为符号位置
// uint32_t = (((uint32_t)data[0] << 16) + ((uint32_t)data[1] << 8) + data[2])
// int32_t = ±(((uint32_t)(data[0] & 0x7f) << 16) + ((uint32_t)data[1] << 8) + data[2])
static int MsbTranslatorN24(TransMetaData *type, const uint8_t *data, int chls,
                            ReferDeviceCallback refer) {
    assert(type != nullptr);
    assert(data != nullptr);
    if (type->metaDataType() == signedType) {
        int32_t value =
            (data[0] & 0x80)
                ? -(((uint32_t)(data[0] & 0x7f) << 16) + ((uint32_t) data[1] << 8) + data[2])
                : (((uint32_t)(data[0] & 0x7f) << 16) + ((uint32_t) data[1] << 8) + data[2]);
        type->protoTrans(value, chls, refer);
    } else {
        uint32_t value = (((uint32_t) data[0] << 16) + ((uint32_t) data[1] << 8) + data[2]);
        type->protoTrans(value, chls, refer);
    }
    return 3;
}

// data[0] 最高位Bit为符号位置
// uint16_t = (((uint16_t)(data[0] & 0xff) << 8) + (data[1] & 0xff))
// int16_t = ±(((uint16_t)(data[0] & 0x7f) << 8) + (data[1] & 0xff))

static int MsbTranslatorN16(TransMetaData *type, const uint8_t *data, int chls,
                            ReferDeviceCallback refer) {
    assert(type != nullptr);
    assert(data != nullptr);
    if (type->metaDataType() == signedType) {
        int16_t value = (data[0] & 0x80) ? -(((uint16_t)(data[0] & 0x7f) << 8) + (data[1] & 0xff))
                                         : (((uint16_t)(data[0] & 0x7f) << 8) + (data[1] & 0xff));
        type->protoTrans(value, chls, refer);
    } else {
        uint16_t value = (((uint16_t)(data[0] & 0xff) << 8) + (data[1] & 0xff));
        type->protoTrans(value, chls, refer);
    }
    return 2;
}

// signed == data[0] == int32_t
// unsigned == data[0] == uint32_t
static int MsbTranslator32d(TransMetaData *type, const uint8_t *data, int chls,
                            ReferDeviceCallback refer) {
    assert(type != nullptr);
    assert(data != nullptr);
    if (type->metaDataType() == signedType) {
        int32_t *value = (int32_t *) (data);
        type->protoTrans(*value, chls, refer);
    } else {
        uint32_t *value = (uint32_t *) (data);
        type->protoTrans(*value, chls, refer);
    }
    return 4;
}

// data[0] 最高位Bit为符号位置
// uint32_t = (((uint32_t)data[0] << 24) + ((uint32_t)data[1] << 16) + ((uint32_t)data[2] << 8) +
// data[3]) int32_t = ±(((uint32_t)(data[0] & 0x7f) << 24) + ((uint32_t)data[1] << 16) +
// ((uint32_t)data[2] << 8) + data[3])
static int MsbTranslatorN32d(TransMetaData *type, const uint8_t *data, int chls,
                             ReferDeviceCallback refer) {
    assert(type != nullptr);
    assert(data != nullptr);
    if (type->metaDataType() == signedType) {
        int32_t value = (data[0] & 0x80)
                            ? -(((uint32_t)(data[0] & 0x7f) << 24) + ((uint32_t) data[1] << 16) +
                                ((uint32_t) data[2] << 8) + data[3])
                            : (((uint32_t)(data[0] & 0x7f) << 24) + ((uint32_t) data[1] << 16) +
                               ((uint32_t) data[2] << 8) + data[3]);
        type->protoTrans(value, chls, refer);
    } else {
        uint32_t value = (((uint32_t) data[0] << 24) + ((uint32_t) data[1] << 16) +
                          ((uint32_t) data[2] << 8) + data[3]);
        type->protoTrans(value, chls, refer);
    }
    return 4;
}

// data[0] & 0x1;
static int MsbTranslator0(TransMetaData *type, const uint8_t *data, int chls,
                          ReferDeviceCallback refer) {
    assert(type != nullptr);
    assert(data != nullptr);
    uint8_t value = data[0] & 0x1;
    type->protoTrans(value, chls, refer);
    return 0;
}

// (data[0] & 0x2) >> 1;
static int MsbTranslator1(TransMetaData *type, const uint8_t *data, int chls,
                          ReferDeviceCallback refer) {
    assert(type != nullptr);
    assert(data != nullptr);
    uint8_t value = data[0] & 0x2;
    type->protoTrans(value >> 1, chls, refer);
    return 0;
}

// (data[0] & 0x4) >> 2;
static int MsbTranslator2(TransMetaData *type, const uint8_t *data, int chls,
                          ReferDeviceCallback refer) {
    assert(type != nullptr);
    assert(data != nullptr);
    uint8_t value = data[0] & 0x4;
    type->protoTrans(value >> 2, chls, refer);
    return 0;
}

// (data[0] & 0x8) >> 3;
static int MsbTranslator3(TransMetaData *type, const uint8_t *data, int chls,
                          ReferDeviceCallback refer) {
    assert(type != nullptr);
    assert(data != nullptr);
    uint8_t value = data[0] & 0x8;
    type->protoTrans(value >> 3, chls, refer);
    return 0;
}

// (data[0] & 0x10) >> 4;
static int MsbTranslator4(TransMetaData *type, const uint8_t *data, int chls,
                          ReferDeviceCallback refer) {
    assert(type != nullptr);
    assert(data != nullptr);
    uint8_t value = data[0] & 0x10;
    type->protoTrans(value >> 4, chls, refer);
    return 0;
}
// (data[0] & 0x20) >> 5;
static int MsbTranslator5(TransMetaData *type, const uint8_t *data, int chls,
                          ReferDeviceCallback refer) {
    assert(type != nullptr);
    assert(data != nullptr);
    uint8_t value = data[0] & 0x20;
    type->protoTrans(value >> 5, chls, refer);
    return 0;
}

// (data[0] & 0x40) >> 6;
static int MsbTranslator6(TransMetaData *type, const uint8_t *data, int chls,
                          ReferDeviceCallback refer) {
    assert(type != nullptr);
    assert(data != nullptr);
    uint8_t value = data[0] & 0x40;
    type->protoTrans(value >> 6, chls, refer);
    return 0;
}

// (data[0] & 0x80) >> 7;
static int MsbTranslator7(TransMetaData *type, const uint8_t *data, int chls,
                          ReferDeviceCallback refer) {
    assert(type != nullptr);
    assert(data != nullptr);
    uint8_t value = data[0] & 0x80;
    type->protoTrans(value >> 7, chls, refer);
    return 1;
}

// (*(uint16_t *)(data)) & 0x01;
static int MsbTranslator0b(TransMetaData *type, const uint8_t *data, int chls,
                           ReferDeviceCallback refer) {
    assert(type != nullptr);
    assert(data != nullptr);
    uint16_t *pvalue = (uint16_t *) (data);
    uint16_t value = (*pvalue) & 0x1;
    type->protoTrans(value, chls, refer);
    return 0;
}

// ((*(uint16_t *)(data))>>1) & 0x01;
static int MsbTranslator1b(TransMetaData *type, const uint8_t *data, int chls,
                           ReferDeviceCallback refer) {
    assert(type != nullptr);
    assert(data != nullptr);
    uint16_t *pvalue = (uint16_t *) (data);
    uint16_t value = ((*pvalue) >> 1) & 0x1;
    type->protoTrans(value, chls, refer);
    return 0;
}

// ((*(uint16_t *)(data))>>2) & 0x01;
static int MsbTranslator2b(TransMetaData *type, const uint8_t *data, int chls,
                           ReferDeviceCallback refer) {
    assert(type != nullptr);
    assert(data != nullptr);
    uint16_t *pvalue = (uint16_t *) (data);
    uint16_t value = ((*pvalue) >> 2) & 0x1;
    type->protoTrans(value, chls, refer);
    return 0;
}
// ((*(uint16_t *)(data))>>3) & 0x01;
static int MsbTranslator3b(TransMetaData *type, const uint8_t *data, int chls,
                           ReferDeviceCallback refer) {
    assert(type != nullptr);
    assert(data != nullptr);
    uint16_t *pvalue = (uint16_t *) (data);
    uint16_t value = ((*pvalue) >> 3) & 0x1;
    type->protoTrans(value, chls, refer);
    return 0;
}
// ((*(uint16_t *)(data))>>4) & 0x01;
static int MsbTranslator4b(TransMetaData *type, const uint8_t *data, int chls,
                           ReferDeviceCallback refer) {
    assert(type != nullptr);
    assert(data != nullptr);
    uint16_t *pvalue = (uint16_t *) (data);
    uint16_t value = ((*pvalue) >> 4) & 0x1;
    type->protoTrans(value, chls, refer);
    return 0;
}
// ((*(uint16_t *)(data))>>5) & 0x01;
static int MsbTranslator5b(TransMetaData *type, const uint8_t *data, int chls,
                           ReferDeviceCallback refer) {
    assert(type != nullptr);
    assert(data != nullptr);
    uint16_t *pvalue = (uint16_t *) (data);
    uint16_t value = ((*pvalue) >> 5) & 0x1;
    type->protoTrans(value, chls, refer);
    return 0;
}
// ((*(uint16_t *)(data))>>6) & 0x01;
static int MsbTranslator6b(TransMetaData *type, const uint8_t *data, int chls,
                           ReferDeviceCallback refer) {
    assert(type != nullptr);
    assert(data != nullptr);
    uint16_t *pvalue = (uint16_t *) (data);
    uint16_t value = ((*pvalue) >> 6) & 0x1;
    type->protoTrans(value, chls, refer);
    return 0;
}
// ((*(uint16_t *)(data))>>7) & 0x01;
static int MsbTranslator7b(TransMetaData *type, const uint8_t *data, int chls,
                           ReferDeviceCallback refer) {
    assert(type != nullptr);
    assert(data != nullptr);
    uint16_t *pvalue = (uint16_t *) (data);
    uint16_t value = ((*pvalue) >> 7) & 0x1;
    type->protoTrans(value, chls, refer);
    return 0;
}
// ((*(uint16_t *)(data))>>8) & 0x01;
static int MsbTranslator8b(TransMetaData *type, const uint8_t *data, int chls,
                           ReferDeviceCallback refer) {
    assert(type != nullptr);
    assert(data != nullptr);
    uint16_t *pvalue = (uint16_t *) (data);
    uint16_t value = ((*pvalue) >> 8) & 0x1;
    type->protoTrans(value, chls, refer);
    return 0;
}
// ((*(uint16_t *)(data))>>9) & 0x01;
static int MsbTranslator9b(TransMetaData *type, const uint8_t *data, int chls,
                           ReferDeviceCallback refer) {
    assert(type != nullptr);
    assert(data != nullptr);
    uint16_t *pvalue = (uint16_t *) (data);
    uint16_t value = ((*pvalue) >> 9) & 0x1;
    type->protoTrans(value, chls, refer);
    return 0;
}
// ((*(uint16_t *)(data))>>10) & 0x01;
static int MsbTranslator10b(TransMetaData *type, const uint8_t *data, int chls,
                            ReferDeviceCallback refer) {
    assert(type != nullptr);
    assert(data != nullptr);
    uint16_t *pvalue = (uint16_t *) (data);
    uint16_t value = ((*pvalue) >> 10) & 0x1;
    type->protoTrans(value, chls, refer);
    return 0;
}
// ((*(uint16_t *)(data))>>11) & 0x01;
static int MsbTranslator11b(TransMetaData *type, const uint8_t *data, int chls,
                            ReferDeviceCallback refer) {
    assert(type != nullptr);
    assert(data != nullptr);
    uint16_t *pvalue = (uint16_t *) (data);
    uint16_t value = ((*pvalue) >> 11) & 0x1;
    type->protoTrans(value, chls, refer);
    return 0;
}
// ((*(uint16_t *)(data))>>12) & 0x01;
static int MsbTranslator12b(TransMetaData *type, const uint8_t *data, int chls,
                            ReferDeviceCallback refer) {
    assert(type != nullptr);
    assert(data != nullptr);
    uint16_t *pvalue = (uint16_t *) (data);
    uint16_t value = ((*pvalue) >> 12) & 0x1;
    type->protoTrans(value, chls, refer);
    return 0;
}
// ((*(uint16_t *)(data))>>13) & 0x01;
static int MsbTranslator13b(TransMetaData *type, const uint8_t *data, int chls,
                            ReferDeviceCallback refer) {
    assert(type != nullptr);
    assert(data != nullptr);
    uint16_t *pvalue = (uint16_t *) (data);
    uint16_t value = ((*pvalue) >> 13) & 0x1;
    type->protoTrans(value, chls, refer);
    return 0;
}
// ((*(uint16_t *)(data))>>14) & 0x01;
static int MsbTranslator14b(TransMetaData *type, const uint8_t *data, int chls,
                            ReferDeviceCallback refer) {
    assert(type != nullptr);
    assert(data != nullptr);
    uint16_t *pvalue = (uint16_t *) (data);
    uint16_t value = ((*pvalue) >> 14) & 0x1;
    type->protoTrans(value, chls, refer);
    return 0;
}
// ((*(uint16_t *)(data))>>15) & 0x01;
static int MsbTranslator15b(TransMetaData *type, const uint8_t *data, int chls,
                            ReferDeviceCallback refer) {
    assert(type != nullptr);
    assert(data != nullptr);
    uint16_t *pvalue = (uint16_t *) (data);
    uint16_t value = ((*pvalue) >> 15) & 0x1;
    type->protoTrans(value, chls, refer);
    return 2;
}

// ((*(uint32_t *)(data))>>0) & 0x07;
static int MsbTranslator02d(TransMetaData *type, const uint8_t *data, int chls,
                            ReferDeviceCallback refer) {
    assert(type != nullptr);
    assert(data != nullptr);
    uint32_t *pvalue = (uint32_t *) (data);
    uint32_t value = (*pvalue) & 0x7;
    type->protoTrans(value, chls, refer);
    return 0;
}

// ((*(uint32_t *)(data))>>0) & 0x01;
static int MsbTranslator0d(TransMetaData *type, const uint8_t *data, int chls,
                           ReferDeviceCallback refer) {
    assert(type != nullptr);
    assert(data != nullptr);
    uint32_t *pvalue = (uint32_t *) (data);
    uint32_t value = (*pvalue) & 0x1;
    type->protoTrans(value, chls, refer);
    return 0;
}
// ((*(uint32_t *)(data))>>1) & 0x01;
static int MsbTranslator1d(TransMetaData *type, const uint8_t *data, int chls,
                           ReferDeviceCallback refer) {
    assert(type != nullptr);
    assert(data != nullptr);
    uint32_t *pvalue = (uint32_t *) (data);
    uint32_t value = ((*pvalue) >> 1) & 0x1;
    type->protoTrans(value, chls, refer);
    return 0;
}
// ((*(uint32_t *)(data))>>2) & 0x01;
static int MsbTranslator2d(TransMetaData *type, const uint8_t *data, int chls,
                           ReferDeviceCallback refer) {
    assert(type != nullptr);
    assert(data != nullptr);
    uint32_t *pvalue = (uint32_t *) (data);
    uint32_t value = (*pvalue >> 2) & 0x1;
    type->protoTrans(value, chls, refer);
    return 0;
}
// ((*(uint32_t *)(data))>>3) & 0x01;
static int MsbTranslator3d(TransMetaData *type, const uint8_t *data, int chls,
                           ReferDeviceCallback refer) {
    assert(type != nullptr);
    assert(data != nullptr);
    uint32_t *pvalue = (uint32_t *) (data);
    uint32_t value = ((*pvalue) >> 3) & 0x1;
    type->protoTrans(value, chls, refer);
    return 0;
}
// ((*(uint32_t *)(data))>>4) & 0x01;
static int MsbTranslator4d(TransMetaData *type, const uint8_t *data, int chls,
                           ReferDeviceCallback refer) {
    assert(type != nullptr);
    assert(data != nullptr);
    uint32_t *pvalue = (uint32_t *) (data);
    uint32_t value = ((*pvalue) >> 4) & 0x1;
    type->protoTrans(value, chls, refer);
    return 0;
}
// ((*(uint32_t *)(data))>>5) & 0x01;
static int MsbTranslator5d(TransMetaData *type, const uint8_t *data, int chls,
                           ReferDeviceCallback refer) {
    assert(type != nullptr);
    assert(data != nullptr);
    uint32_t *pvalue = (uint32_t *) (data);
    uint32_t value = ((*pvalue) >> 5) & 0x1;
    type->protoTrans(value, chls, refer);
    return 0;
}
// ((*(uint32_t *)(data))>>4) & 0x03;
static int MsbTranslator45d(TransMetaData *type, const uint8_t *data, int chls,
                            ReferDeviceCallback refer) {
    assert(type != nullptr);
    assert(data != nullptr);
    uint32_t *pvalue = (uint32_t *) (data);
    uint32_t value = ((*pvalue) >> 4) & 0x3;
    type->protoTrans(value, chls, refer);
    return 0;
}
// ((*(uint32_t *)(data))>>6) & 0x07;
static int MsbTranslator68d(TransMetaData *type, const uint8_t *data, int chls,
                            ReferDeviceCallback refer) {
    assert(type != nullptr);
    assert(data != nullptr);
    uint32_t *pvalue = (uint32_t *) (data);
    uint32_t value = ((*pvalue) >> 6) & 0x7;
    type->protoTrans(value, chls, refer);
    return 0;
}
// ((*(uint32_t *)(data))>>9) & 0x03;
static int MsbTranslator9Ad(TransMetaData *type, const uint8_t *data, int chls,
                            ReferDeviceCallback refer) {
    assert(type != nullptr);
    assert(data != nullptr);
    uint32_t *pvalue = (uint32_t *) (data);
    uint32_t value = ((*pvalue) >> 9) & 0x3;
    type->protoTrans(value, chls, refer);
    return 0;
}
// ((*(uint32_t *)(data))>>6) & 0x01;
static int MsbTranslator6d(TransMetaData *type, const uint8_t *data, int chls,
                           ReferDeviceCallback refer) {
    assert(type != nullptr);
    assert(data != nullptr);
    uint32_t *pvalue = (uint32_t *) (data);
    uint32_t value = ((*pvalue) >> 6) & 0x1;
    type->protoTrans(value, chls, refer);
    return 0;
}

// ((*(uint32_t *)(data))>>7) & 0x01;
static int MsbTranslator7d(TransMetaData *type, const uint8_t *data, int chls,
                           ReferDeviceCallback refer) {
    assert(type != nullptr);
    assert(data != nullptr);
    uint32_t *pvalue = (uint32_t *) (data);
    uint32_t value = ((*pvalue) >> 7) & 0x1;
    type->protoTrans(value, chls, refer);
    return 0;
}
// ((*(uint32_t *)(data))>>8) & 0x01;
static int MsbTranslator8d(TransMetaData *type, const uint8_t *data, int chls,
                           ReferDeviceCallback refer) {
    assert(type != nullptr);
    assert(data != nullptr);
    uint32_t *pvalue = (uint32_t *) (data);
    uint32_t value = ((*pvalue) >> 8) & 0x1;
    type->protoTrans(value, chls, refer);
    return 0;
}
// ((*(uint32_t *)(data))>>9) & 0x01;
static int MsbTranslator9d(TransMetaData *type, const uint8_t *data, int chls,
                           ReferDeviceCallback refer) {
    assert(type != nullptr);
    assert(data != nullptr);
    uint32_t *pvalue = (uint32_t *) (data);
    uint32_t value = ((*pvalue) >> 9) & 0x1;
    type->protoTrans(value, chls, refer);
    return 0;
}
// ((*(uint32_t *)(data))>>10) & 0x01;
static int MsbTranslator10d(TransMetaData *type, const uint8_t *data, int chls,
                            ReferDeviceCallback refer) {
    assert(type != nullptr);
    assert(data != nullptr);
    uint32_t *pvalue = (uint32_t *) (data);
    uint32_t value = ((*pvalue) >> 10) & 0x1;
    type->protoTrans(value, chls, refer);
    return 0;
}
// ((*(uint32_t *)(data))>>11) & 0x01;
static int MsbTranslator11d(TransMetaData *type, const uint8_t *data, int chls,
                            ReferDeviceCallback refer) {
    assert(type != nullptr);
    assert(data != nullptr);
    uint32_t *pvalue = (uint32_t *) (data);
    uint32_t value = ((*pvalue) >> 11) & 0x1;
    type->protoTrans(value, chls, refer);
    return 0;
}
// ((*(uint32_t *)(data))>>12) & 0x01;
static int MsbTranslator12d(TransMetaData *type, const uint8_t *data, int chls,
                            ReferDeviceCallback refer) {
    assert(type != nullptr);
    assert(data != nullptr);
    uint32_t *pvalue = (uint32_t *) (data);
    uint32_t value = ((*pvalue) >> 12) & 0x1;
    type->protoTrans(value, chls, refer);
    return 0;
}
// ((*(uint32_t *)(data))>>13) & 0x01;
static int MsbTranslator13d(TransMetaData *type, const uint8_t *data, int chls,
                            ReferDeviceCallback refer) {
    assert(type != nullptr);
    assert(data != nullptr);
    uint32_t *pvalue = (uint32_t *) (data);
    uint32_t value = ((*pvalue) >> 13) & 0x1;
    type->protoTrans(value, chls, refer);
    return 0;
}
// ((*(uint32_t *)(data))>>14) & 0x01;
static int MsbTranslator14d(TransMetaData *type, const uint8_t *data, int chls,
                            ReferDeviceCallback refer) {
    assert(type != nullptr);
    assert(data != nullptr);
    uint32_t *pvalue = (uint32_t *) (data);
    uint32_t value = ((*pvalue) >> 14) & 0x1;
    type->protoTrans(value, chls, refer);
    return 0;
}
// ((*(uint32_t *)(data))>>15) & 0x01;
static int MsbTranslator15d(TransMetaData *type, const uint8_t *data, int chls,
                            ReferDeviceCallback refer) {
    assert(type != nullptr);
    assert(data != nullptr);
    uint32_t *pvalue = (uint32_t *) (data);
    uint32_t value = ((*pvalue) >> 15) & 0x1;
    type->protoTrans(value, chls, refer);
    return 0;
}
// ((*(uint32_t *)(data))>>16) & 0x01;
static int MsbTranslator16d(TransMetaData *type, const uint8_t *data, int chls,
                            ReferDeviceCallback refer) {
    assert(type != nullptr);
    assert(data != nullptr);
    uint32_t *pvalue = (uint32_t *) (data);
    uint32_t value = ((*pvalue) >> 16) & 0x1;
    type->protoTrans(value, chls, refer);
    return 0;
}
// ((*(uint32_t *)(data))>>17) & 0x01;
static int MsbTranslator17d(TransMetaData *type, const uint8_t *data, int chls,
                            ReferDeviceCallback refer) {
    assert(type != nullptr);
    assert(data != nullptr);
    uint32_t *pvalue = (uint32_t *) (data);
    uint32_t value = ((*pvalue) >> 17) & 0x1;
    type->protoTrans(value, chls, refer);
    return 0;
}
// ((*(uint32_t *)(data))>>18) & 0x01;
static int MsbTranslator18d(TransMetaData *type, const uint8_t *data, int chls,
                            ReferDeviceCallback refer) {
    assert(type != nullptr);
    assert(data != nullptr);
    uint32_t *pvalue = (uint32_t *) (data);
    uint32_t value = ((*pvalue) >> 18) & 0x1;
    type->protoTrans(value, chls, refer);
    return 0;
}
// ((*(uint32_t *)(data))>>19) & 0x01;
static int MsbTranslator19d(TransMetaData *type, const uint8_t *data, int chls,
                            ReferDeviceCallback refer) {
    assert(type != nullptr);
    assert(data != nullptr);
    uint32_t *pvalue = (uint32_t *) (data);
    uint32_t value = ((*pvalue) >> 19) & 0x1;
    type->protoTrans(value, chls, refer);
    return 0;
}
// ((*(uint32_t *)(data))>>20) & 0x01;
static int MsbTranslator20d(TransMetaData *type, const uint8_t *data, int chls,
                            ReferDeviceCallback refer) {
    assert(type != nullptr);
    assert(data != nullptr);
    uint32_t *pvalue = (uint32_t *) (data);
    uint32_t value = ((*pvalue) >> 20) & 0x1;
    type->protoTrans(value, chls, refer);
    return 0;
}
// ((*(uint32_t *)(data))>>21) & 0x01;
static int MsbTranslator21d(TransMetaData *type, const uint8_t *data, int chls,
                            ReferDeviceCallback refer) {
    assert(type != nullptr);
    assert(data != nullptr);
    uint32_t *pvalue = (uint32_t *) (data);
    uint32_t value = ((*pvalue) >> 21) & 0x1;
    type->protoTrans(value, chls, refer);
    return 0;
}
// ((*(uint32_t *)(data))>>22) & 0x01;
static int MsbTranslator22d(TransMetaData *type, const uint8_t *data, int chls,
                            ReferDeviceCallback refer) {
    assert(type != nullptr);
    assert(data != nullptr);
    uint32_t *pvalue = (uint32_t *) (data);
    uint32_t value = ((*pvalue) >> 22) & 0x1;
    type->protoTrans(value, chls, refer);
    return 0;
}
// ((*(uint32_t *)(data))>>23) & 0x01;
static int MsbTranslator23d(TransMetaData *type, const uint8_t *data, int chls,
                            ReferDeviceCallback refer) {
    assert(type != nullptr);
    assert(data != nullptr);
    uint32_t *pvalue = (uint32_t *) (data);
    uint32_t value = ((*pvalue) >> 23) & 0x1;
    type->protoTrans(value, chls, refer);
    return 0;
}
// ((*(uint32_t *)(data))>>24) & 0x01;
static int MsbTranslator24d(TransMetaData *type, const uint8_t *data, int chls,
                            ReferDeviceCallback refer) {
    assert(type != nullptr);
    assert(data != nullptr);
    uint32_t *pvalue = (uint32_t *) (data);
    uint32_t value = ((*pvalue) >> 24) & 0x1;
    type->protoTrans(value, chls, refer);
    return 0;
}
// ((*(uint32_t *)(data))>>25) & 0x01;
static int MsbTranslator25d(TransMetaData *type, const uint8_t *data, int chls,
                            ReferDeviceCallback refer) {
    assert(type != nullptr);
    assert(data != nullptr);
    uint32_t *pvalue = (uint32_t *) (data);
    uint32_t value = ((*pvalue) >> 25) & 0x1;
    type->protoTrans(value, chls, refer);
    return 0;
}
// ((*(uint32_t *)(data))>>26) & 0x01;
static int MsbTranslator26d(TransMetaData *type, const uint8_t *data, int chls,
                            ReferDeviceCallback refer) {
    assert(type != nullptr);
    assert(data != nullptr);
    uint32_t *pvalue = (uint32_t *) (data);
    uint32_t value = ((*pvalue) >> 26) & 0x1;
    type->protoTrans(value, chls, refer);
    return 0;
}
// ((*(uint32_t *)(data))>>27) & 0x01;
static int MsbTranslator27d(TransMetaData *type, const uint8_t *data, int chls,
                            ReferDeviceCallback refer) {
    assert(type != nullptr);
    assert(data != nullptr);
    uint32_t *pvalue = (uint32_t *) (data);
    uint32_t value = ((*pvalue) >> 27) & 0x1;
    type->protoTrans(value, chls, refer);
    return 0;
}
// ((*(uint32_t *)(data))>>28) & 0x01;
static int MsbTranslator28d(TransMetaData *type, const uint8_t *data, int chls,
                            ReferDeviceCallback refer) {
    assert(type != nullptr);
    assert(data != nullptr);
    uint32_t *pvalue = (uint32_t *) (data);
    uint32_t value = ((*pvalue) >> 28) & 0x1;
    type->protoTrans(value, chls, refer);
    return 0;
}
// ((*(uint32_t *)(data))>>29) & 0x01;
static int MsbTranslator29d(TransMetaData *type, const uint8_t *data, int chls,
                            ReferDeviceCallback refer) {
    assert(type != nullptr);
    assert(data != nullptr);
    uint32_t *pvalue = (uint32_t *) (data);
    uint32_t value = ((*pvalue) >> 29) & 0x1;
    type->protoTrans(value, chls, refer);
    return 0;
}
// ((*(uint32_t *)(data))>>30) & 0x01;
static int MsbTranslator30d(TransMetaData *type, const uint8_t *data, int chls,
                            ReferDeviceCallback refer) {
    assert(type != nullptr);
    assert(data != nullptr);
    uint32_t *pvalue = (uint32_t *) (data);
    uint32_t value = ((*pvalue) >> 30) & 0x1;
    type->protoTrans(value, chls, refer);
    return 0;
}
// ((*(uint32_t *)(data))>>26) & 0x03;
static int MsbTranslator2627d(TransMetaData *type, const uint8_t *data, int chls,
                              ReferDeviceCallback refer) {
    assert(type != nullptr);
    assert(data != nullptr);
    uint32_t *pvalue = (uint32_t *) (data);
    uint32_t value = ((*pvalue) >> 26) & 0x3;
    type->protoTrans(value, chls, refer);
    return 0;
}
// ((*(uint32_t *)(data))>>28) & 0x03;
static int MsbTranslator2829d(TransMetaData *type, const uint8_t *data, int chls,
                              ReferDeviceCallback refer) {
    assert(type != nullptr);
    assert(data != nullptr);
    uint32_t *pvalue = (uint32_t *) (data);
    uint32_t value = ((*pvalue) >> 28) & 0x3;
    type->protoTrans(value, chls, refer);
    return 0;
}
// ((*(uint16_t *)(data))>>0) & 0x03;
static int MsbTranslator01b(TransMetaData *type, const uint8_t *data, int chls,
                            ReferDeviceCallback refer) {
    assert(type != nullptr);
    assert(data != nullptr);
    uint16_t *pvalue = (uint16_t *) (data);
    uint16_t value = ((*pvalue) >> 0) & 0x3;
    type->protoTrans(value, chls, refer);
    return 0;
}
// ((*(uint16_t *)(data))>>2) & 0x03;
static int MsbTranslator23b(TransMetaData *type, const uint8_t *data, int chls,
                            ReferDeviceCallback refer) {
    assert(type != nullptr);
    assert(data != nullptr);
    uint16_t *pvalue = (uint16_t *) (data);
    uint16_t value = ((*pvalue) >> 2) & 0x3;
    type->protoTrans(value, chls, refer);
    return 0;
}
// ((*(uint16_t *)(data))>>4) & 0x03;
static int MsbTranslator45b(TransMetaData *type, const uint8_t *data, int chls,
                            ReferDeviceCallback refer) {
    assert(type != nullptr);
    assert(data != nullptr);
    uint16_t *pvalue = (uint16_t *) (data);
    uint16_t value = ((*pvalue) >> 4) & 0x3;
    type->protoTrans(value, chls, refer);
    return 0;
}
// ((*(uint16_t *)(data))>>6) & 0x03;
static int MsbTranslator67b(TransMetaData *type, const uint8_t *data, int chls,
                            ReferDeviceCallback refer) {
    assert(type != nullptr);
    assert(data != nullptr);
    uint16_t *pvalue = (uint16_t *) (data);
    uint16_t value = ((*pvalue) >> 6) & 0x3;
    type->protoTrans(value, chls, refer);
    return 0;
}
// ((*(uint16_t *)(data))>>8) & 0x03;
static int MsbTranslator89b(TransMetaData *type, const uint8_t *data, int chls,
                            ReferDeviceCallback refer) {
    assert(type != nullptr);
    assert(data != nullptr);
    uint16_t *pvalue = (uint16_t *) (data);
    uint16_t value = ((*pvalue) >> 8) & 0x3;
    type->protoTrans(value, chls, refer);
    return 0;
}
// ((*(uint16_t *)(data))>>10) & 0x03;
static int MsbTranslatorABb(TransMetaData *type, const uint8_t *data, int chls,
                            ReferDeviceCallback refer) {
    assert(type != nullptr);
    assert(data != nullptr);
    uint16_t *pvalue = (uint16_t *) (data);
    uint16_t value = ((*pvalue) >> 10) & 0x3;
    type->protoTrans(value, chls, refer);
    return 0;
}
// ((*(uint16_t *)(data))>>12) & 0x03;
static int MsbTranslatorCDb(TransMetaData *type, const uint8_t *data, int chls,
                            ReferDeviceCallback refer) {
    assert(type != nullptr);
    assert(data != nullptr);
    uint16_t *pvalue = (uint16_t *) (data);
    uint16_t value = ((*pvalue) >> 12) & 0x3;
    type->protoTrans(value, chls, refer);
    return 0;
}
// ((*(uint16_t *)(data))>>0) & 0xff;
static int MsbTranslator07b(TransMetaData *type, const uint8_t *data, int chls,
                            ReferDeviceCallback refer) {
    assert(type != nullptr);
    assert(data != nullptr);
    uint16_t *pvalue = (uint16_t *) (data);
    uint16_t value = ((*pvalue) >> 0) & 0xff;
    type->protoTrans(value, chls, refer);
    return 0;
}
// ((*(uint16_t *)(data))>>0) & 0xf;
static int MsbTranslator03b(TransMetaData *type, const uint8_t *data, int chls,
                            ReferDeviceCallback refer) {
    assert(type != nullptr);
    assert(data != nullptr);
    uint16_t *pvalue = (uint16_t *) (data);
    uint16_t value = ((*pvalue) >> 0) & 0xf;
    type->protoTrans(value, chls, refer);
    return 0;
}
// ((*(uint16_t *)(data))>>4) & 0xf;
static int MsbTranslator47b(TransMetaData *type, const uint8_t *data, int chls,
                            ReferDeviceCallback refer) {
    assert(type != nullptr);
    assert(data != nullptr);
    uint16_t *pvalue = (uint16_t *) (data);
    uint16_t value = ((*pvalue) >> 4) & 0xf;
    type->protoTrans(value, chls, refer);
    return 0;
}
// ((*(uint16_t *)(data))>>8) & 0xf;
static int MsbTranslator8Bb(TransMetaData *type, const uint8_t *data, int chls,
                            ReferDeviceCallback refer) {
    assert(type != nullptr);
    assert(data != nullptr);
    uint16_t *pvalue = (uint16_t *) (data);
    uint16_t value = ((*pvalue) >> 8) & 0xf;
    type->protoTrans(value, chls, refer);
    return 0;
}
// ((*(uint16_t *)(data))>>8) & 0x7;
static int MsbTranslator8Ab(TransMetaData *type, const uint8_t *data, int chls,
                            ReferDeviceCallback refer) {
    assert(type != nullptr);
    assert(data != nullptr);
    uint16_t *pvalue = (uint16_t *) (data);
    uint16_t value = ((*pvalue) >> 8) & 0x7;
    type->protoTrans(value, chls, refer);
    return 0;
}
// ((*(uint16_t *)(data))>>11) & 0x3;
static int MsbTranslatorBCb(TransMetaData *type, const uint8_t *data, int chls,
                            ReferDeviceCallback refer) {
    assert(type != nullptr);
    assert(data != nullptr);
    uint16_t *pvalue = (uint16_t *) (data);
    uint16_t value = ((*pvalue) >> 11) & 0x3;
    type->protoTrans(value, chls, refer);
    return 0;
}

// ((*(uint16_t *)(data))>>31) & 0x1;
static int MsbTranslator31d(TransMetaData *type, const uint8_t *data, int chls,
                            ReferDeviceCallback refer) {
    assert(type != nullptr);
    assert(data != nullptr);
    uint32_t *pvalue = (uint32_t *) (data);
    uint32_t value = ((*pvalue) >> 31) & 0x1;
    type->protoTrans(value, chls, refer);
    return 4;
}

// data[0] & 0x3;
static int MsbTranslator10(TransMetaData *type, const uint8_t *data, int chls,
                           ReferDeviceCallback refer) {
    assert(type != nullptr);
    assert(data != nullptr);
    uint8_t value = data[0] & 0x3;
    type->protoTrans(value, chls, refer);
    return 0;
}
// data[0] & 0x7;
static int MsbTranslator20(TransMetaData *type, const uint8_t *data, int chls,
                           ReferDeviceCallback refer) {
    assert(type != nullptr);
    assert(data != nullptr);
    uint8_t value = data[0] & 0x7;
    type->protoTrans(value, chls, refer);
    return 0;
}
// data[0] & 0xf;
static int MsbTranslator30(TransMetaData *type, const uint8_t *data, int chls,
                           ReferDeviceCallback refer) {
    assert(type != nullptr);
    assert(data != nullptr);
    uint8_t value = data[0] & 0x0f;
    type->protoTrans(value, chls, refer);
    return 0;
}
// data[0] & 0xf;
static int MsbTranslator40(TransMetaData *type, const uint8_t *data, int chls,
                           ReferDeviceCallback refer) {
    assert(type != nullptr);
    assert(data != nullptr);
    uint8_t value = data[0] & 0x1f;
    type->protoTrans(value, chls, refer);
    return 0;
}
// (data[0]>>1) & 0x3;
static int MsbTranslator21(TransMetaData *type, const uint8_t *data, int chls,
                           ReferDeviceCallback refer) {
    assert(type != nullptr);
    assert(data != nullptr);
    uint8_t value = (data[0] >> 1) & 0x3;
    type->protoTrans(value, chls, refer);
    return 0;
}
// (data[0]>>1) & 0x3;
static int MsbTranslator31(TransMetaData *type, const uint8_t *data, int chls,
                           ReferDeviceCallback refer) {
    assert(type != nullptr);
    assert(data != nullptr);
    uint8_t value = (data[0] >> 1) & 0x7;
    type->protoTrans(value, chls, refer);
    return 0;
}
// (data[0]>>2) & 0x3;
static int MsbTranslator32(TransMetaData *type, const uint8_t *data, int chls,
                           ReferDeviceCallback refer) {
    assert(type != nullptr);
    assert(data != nullptr);
    uint8_t value = (data[0] >> 2) & 0x3;
    type->protoTrans(value, chls, refer);
    return 0;
}

// (data[0]>>2) & 0x7;
static int MsbTranslator42(TransMetaData *type, const uint8_t *data, int chls,
                           ReferDeviceCallback refer) {
    assert(type != nullptr);
    assert(data != nullptr);
    uint8_t value = (data[0] >> 2) & 0x7;
    type->protoTrans(value, chls, refer);
    return 0;
}

// (data[0]>>2) & 0xf;
static int MsbTranslator52(TransMetaData *type, const uint8_t *data, int chls,
                           ReferDeviceCallback refer) {
    assert(type != nullptr);
    assert(data != nullptr);
    uint8_t value = (data[0] >> 2) & 0xf;
    type->protoTrans(value, chls, refer);
    return 0;
}

// (data[0] & 0xFC) >>2)
static int MsbTranslator72(TransMetaData *type, const uint8_t *data, int chls,
                           ReferDeviceCallback refer) {
    assert(type != nullptr);
    assert(data != nullptr);
    uint8_t value = data[0] & 0xFC;
    type->protoTrans(value >> 2, chls, refer);
    return 1;
}

// (data[0]>>3) & 0x3;
static int MsbTranslator43(TransMetaData *type, const uint8_t *data, int chls,
                           ReferDeviceCallback refer) {
    assert(type != nullptr);
    assert(data != nullptr);
    uint8_t value = (data[0] >> 3) & 0x3;
    type->protoTrans(value, chls, refer);
    return 0;
}

// (data[0]>>3) & 0x7;
static int MsbTranslator53(TransMetaData *type, const uint8_t *data, int chls,
                           ReferDeviceCallback refer) {
    assert(type != nullptr);
    assert(data != nullptr);
    uint8_t value = (data[0] >> 3) & 0x7;
    type->protoTrans(value, chls, refer);
    return 0;
}

// (data[0]>>3) & 0xf;
static int MsbTranslator63(TransMetaData *type, const uint8_t *data, int chls,
                           ReferDeviceCallback refer) {
    assert(type != nullptr);
    assert(data != nullptr);
    uint8_t value = (data[0] >> 3) & 0xf;
    type->protoTrans(value, chls, refer);
    return 0;
}
// (data[0]>>4) & 0x3;
static int MsbTranslator54(TransMetaData *type, const uint8_t *data, int chls,
                           ReferDeviceCallback refer) {
    assert(type != nullptr);
    assert(data != nullptr);
    uint8_t value = (data[0] >> 4) & 0x3;
    type->protoTrans(value, chls, refer);
    return 0;
}
// (data[0]>>4) & 0x7;
static int MsbTranslator64(TransMetaData *type, const uint8_t *data, int chls,
                           ReferDeviceCallback refer) {
    assert(type != nullptr);
    assert(data != nullptr);
    uint8_t value = (data[0] >> 4) & 0x7;
    type->protoTrans(value, chls, refer);
    return 0;
}
// (data[0]>>4) & 0xf;
static int MsbTranslator74(TransMetaData *type, const uint8_t *data, int chls,
                           ReferDeviceCallback refer) {
    assert(type != nullptr);
    assert(data != nullptr);
    uint8_t value = (data[0] >> 4) & 0xf;
    type->protoTrans(value, chls, refer);
    return 1;
}
// (data[0]>>5) & 0x3;
static int MsbTranslator65(TransMetaData *type, const uint8_t *data, int chls,
                           ReferDeviceCallback refer) {
    assert(type != nullptr);
    assert(data != nullptr);
    uint8_t value = (data[0] >> 5) & 0x3;
    type->protoTrans(value, chls, refer);
    return 0;
}
// (data[0]>>5) & 0x7;
static int MsbTranslator75(TransMetaData *type, const uint8_t *data, int chls,
                           ReferDeviceCallback refer) {
    assert(type != nullptr);
    assert(data != nullptr);
    uint8_t value = (data[0] >> 5) & 0x7;
    type->protoTrans(value, chls, refer);
    return 1;
}

// (data[0]>>6) & 0x3;
static int MsbTranslator76(TransMetaData *type, const uint8_t *data, int chls,
                           ReferDeviceCallback refer) {
    assert(type != nullptr);
    assert(data != nullptr);
    uint8_t value = (data[0] >> 6) & 0x3;
    type->protoTrans(value, chls, refer);
    return 1;
}

// undefine
static int MsbTranslator64d(TransMetaData *type, const uint8_t *data, int chls,
                            ReferDeviceCallback refer) {
    assert(type != nullptr);
    assert(data != nullptr);
    return 8;
}

// data[0] & 0x1 === 下一个字节
static int MsbTranslatorN0(TransMetaData *type, const uint8_t *data, int chls,
                           ReferDeviceCallback refer) {
    assert(type != nullptr);
    assert(data != nullptr);
    uint8_t value = data[0] & 0x1;
    type->protoTrans(value, chls, refer);
    return 1;
}
// ((data[0] & 0x80) >> 7) === 当前字节
static int MsbTranslatorN7(TransMetaData *type, const uint8_t *data, int chls,
                           ReferDeviceCallback refer) {
    assert(type != nullptr);
    assert(data != nullptr);
    uint8_t value = data[0] & 0x80;
    type->protoTrans(value >> 7, chls, refer);
    return 0;
}

// struct8 == data[0] & 0xff
static int TranslatorStruct8(TransMetaData *type, const uint8_t *data, int chls,
                             ReferDeviceCallback refer) {
    assert(type != nullptr);
    assert(data != nullptr);
    uint8_t value = data[0] & 0xff;
    type->protoTrans(value, chls, refer);
    return 1;
}

// struct 16N == (((data[1] & 0xff) << 8) + (data[0] & 0xff))
static int TranslatorStruct16N(TransMetaData *type, const uint8_t *data, int chls,
                               ReferDeviceCallback refer) {
    assert(type != nullptr);
    assert(data != nullptr);
    uint16_t value = (((uint16_t)(data[1] & 0xff) << 8) + (data[0] & 0xff));
    type->protoTrans(value, chls, refer);
    return 2;
}
// struct 16 == (((data[0] & 0xff) << 8) + (data[1] & 0xff))
static int TranslatorStructN16(TransMetaData *type, const uint8_t *data, int chls,
                               ReferDeviceCallback refer) {
    assert(type != nullptr);
    assert(data != nullptr);
    uint16_t value = (((uint16_t)(data[0] & 0xff) << 8) + (data[1] & 0xff));
    type->protoTrans(value, chls, refer);
    return 2;
}
// struct 32N == (((uint16_t)data[2] << 24) + ((uint16_t)data[2] << 16) + ((uint16_t)data[1] << 8) +
// data[0])
static int TranslatorStruct32N(TransMetaData *type, const uint8_t *data, int chls,
                               ReferDeviceCallback refer) {
    assert(type != nullptr);
    assert(data != nullptr);
    uint32_t value = (((uint32_t) data[2] << 24) + ((uint32_t) data[2] << 16) +
                      ((uint32_t) data[1] << 8) + data[0]);
    type->protoTrans(value, chls, refer);
    return 4;
}
// struct 32 == (((uint32_t)data[0] << 24) + ((uint32_t)data[1] << 16) + ((uint32_t)data[2] << 8) +
// data[3])
static int TranslatorStructN32(TransMetaData *type, const uint8_t *data, int chls,
                               ReferDeviceCallback refer) {
    assert(type != nullptr);
    assert(data != nullptr);
    uint32_t value = (((uint32_t) data[0] << 24) + ((uint32_t) data[1] << 16) +
                      ((uint32_t) data[2] << 8) + data[3]);
    type->protoTrans(value, chls, refer);
    return 4;
}

QMap<QString, TransMetaData::DataTranslator> &TransMetaData::loadDataTranslator() {
    static QMap<QString, TransMetaData::DataTranslator> allTranslator2 = {
        {"8", MsbTranslator8},
        {"16", MsbTranslator16},
        {"..16", MsbTranslatorN16},
        {"24", MsbTranslator24},
        {"..24", MsbTranslatorN24},
        {"32", MsbTranslator32d},
        {"..32", MsbTranslatorN32d},
        {".0", MsbTranslatorN0},
        {"0.", MsbTranslator0},
        {"1.", MsbTranslator1},
        {"2.", MsbTranslator2},
        {"3.", MsbTranslator3},
        {"4.", MsbTranslator4},
        {"5.", MsbTranslator5},
        {"6.", MsbTranslator6},
        {"7.", MsbTranslator7},
        {".7", MsbTranslatorN7},
        {"0.1", MsbTranslator10},
        {"0.2", MsbTranslator20},
        {"0.3", MsbTranslator30},
        {"0.4", MsbTranslator40},
        {"1.2", MsbTranslator21},
        {"1.3", MsbTranslator31},
        {"2.3", MsbTranslator32},
        {"2.4", MsbTranslator42},
        {"2.5", MsbTranslator52},
        {"2.7", MsbTranslator72},
        {"3.4", MsbTranslator43},
        {"3.5", MsbTranslator53},
        {"3.6", MsbTranslator63},
        {"4.5", MsbTranslator54},
        {"4.6", MsbTranslator64},
        {"4.7", MsbTranslator74},
        {"5.6", MsbTranslator65},
        {"5.7", MsbTranslator75},
        {"6.7", MsbTranslator76},
        {"0..", MsbTranslator0b},
        {"1..", MsbTranslator1b},
        {"2..", MsbTranslator2b},
        {"3..", MsbTranslator3b},
        {"4..", MsbTranslator4b},
        {"5..", MsbTranslator5b},
        {"6..", MsbTranslator6b},
        {"7..", MsbTranslator7b},
        {"8..", MsbTranslator8b},
        {"9..", MsbTranslator9b},
        {"10..", MsbTranslator10b},
        {"11..", MsbTranslator11b},
        {"12..", MsbTranslator12b},
        {"13..", MsbTranslator13b},
        {"14..", MsbTranslator14b},
        {"15..", MsbTranslator15b},
        {"0..1", MsbTranslator01b},
        {"2..3", MsbTranslator23b},
        {"4..5", MsbTranslator45b},
        {"6..7", MsbTranslator67b},
        {"8..9", MsbTranslator89b},
        {"10..11", MsbTranslatorABb},
        {"12..13", MsbTranslatorCDb},
        {"0..7", MsbTranslator07b},
        {"0..3", MsbTranslator03b},
        {"4..7", MsbTranslator47b},
        {"8..11", MsbTranslator8Bb},
        {"8..10", MsbTranslator8Ab},
        {"11..12", MsbTranslatorBCb},
        {"0...2", MsbTranslator02d},
        {"0...", MsbTranslator0d},
        {"1...", MsbTranslator1d},
        {"2...", MsbTranslator2d},
        {"3...", MsbTranslator3d},
        {"4...", MsbTranslator4d},
        {"5...", MsbTranslator5d},
        {"4...5", MsbTranslator45d},
        {"6...8", MsbTranslator68d},
        {"9...10", MsbTranslator9Ad},
        {"6...", MsbTranslator6d},
        {"7...", MsbTranslator7d},
        {"8...", MsbTranslator8d},
        {"9...", MsbTranslator9d},
        {"10...", MsbTranslator10d},
        {"11...", MsbTranslator11d},
        {"12...", MsbTranslator12d},
        {"13...", MsbTranslator13d},
        {"14...", MsbTranslator14d},
        {"15...", MsbTranslator15d},
        {"16...", MsbTranslator16d},
        {"17...", MsbTranslator17d},
        {"18...", MsbTranslator18d},
        {"19...", MsbTranslator19d},
        {"20...", MsbTranslator20d},
        {"21...", MsbTranslator21d},
        {"22...", MsbTranslator22d},
        {"23...", MsbTranslator23d},
        {"24...", MsbTranslator24d},
        {"25...", MsbTranslator25d},
        {"26...", MsbTranslator26d},
        {"27...", MsbTranslator27d},
        {"28...", MsbTranslator28d},
        {"29...", MsbTranslator29d},
        {"30...", MsbTranslator30d},
        {"26...27", MsbTranslator2627d},
        {"28...29", MsbTranslator2829d},
        {"31...", MsbTranslator31d},
        {"64", MsbTranslator64d},
        {"Struct", TranslatorStruct8},
        {"Struct16N", TranslatorStruct16N},
        {"Struct16", TranslatorStructN16},
        {"Struct32N", TranslatorStruct32N},
        {"Struct32", TranslatorStructN32},
    };

    return allTranslator2;

}

template <class Type>
void TransMetaData::protoTrans(Type val, int chl, ReferDeviceCallback referFunc) {

    uint16_t refCond = 0;
    int dChl = (chl >> 8) & 0xF;
    if ((referFunc != nullptr) && _refer._enable) {
        if (referFunc(&_refer, dChl)) {
            refCond = 1 << 8;
        } else {
            refCond = 0;
        }
    }

    if (_refer._enable) {
        datas[dChl]._refValue = refCond;
        // /// 参考没有改变，本身也没有改变
        // if ((datas[dChl]._refValue == refCond) && (datas[dChl]._rawValue == val)) {
        //     if (((++_updateFreq) % 2 != 0)) {
        //         return;
        //     }
        // } else {
        //     datas[dChl]._refValue = refCond;
        // }
    }
    // else {
    //     if (datas[dChl]._rawValue == val) {
    //         if (((++_updateFreq) % 2 != 0)) {
    //             return;
    //         }
    //     }
    // }

    datas[dChl]._rawValue = val;
    _metaData.transferValue(val, datas[dChl]._display, sizeof(datas[dChl]._display));
    // COMMON_INFO("channel: {} frame: {}, item: {}, color: {}, dispay: {}, refCond: {}", chl,
    //             frameName().toStdString(), name().toStdString(), acolor(),
    //             displayStr.toStdString(), refCond);
    /// data come from the selected channel
    if (dChl == (chl & 0xF)) {
        QString displayStr = QString::fromStdString(datas[dChl]._display);
        /// add device state
        setColor(refCond);
        /// multi display node
        if (_multiDisp) {
            emit multiDisplayChanged(acolor(), dChl);
        }
        /// raw data notify mode
        if (_rawNofify) {
            emit rawDisplayChanged(displayStr, acolor(), val);
        } // end if
        /// normal notify mode
        emit displayChanged(displayStr, acolor());
    }
}

int TransMetaData::protoData(const uint8_t *data, int chl, ReferDeviceCallback refer) {

    QMap<QString, DataTranslator> &translator = loadDataTranslator();
    if (getBitSet().isEmpty()) {
        return 1;
    }

    auto get = translator.find(getBitSet());
    if (get != translator.end()) {
        return get.value()(this, data, chl, refer);
    }

    return 1;
}

TransMetaData::TransMetaData(QObject *parent) : FactMetaData(), _copyData() { _refItem = 0; }

TransMetaData &TransMetaData::operator=(const TransMetaData &other) {
    if (&other == this) {
        return *this;
    }

    _copyData = other._copyData;
    _refItem = other._refItem;

    auto mx = other._cacheNode.begin();
    while (mx != other._cacheNode.end()) {
        std::shared_ptr<TransMetaData> newElem(new TransMetaData(*mx.value().get()));
        _cacheNode.insert(mx.key(), newElem);
        _subfactDatas.append(newElem.get());
    }

    return *this;
}

TransMetaData::TransMetaData(const TransMetaData &other) {

    _copyData = other._copyData;
    _refItem = other._refItem;

    auto mx = other._cacheNode.begin();
    while (mx != other._cacheNode.end()) {
        std::shared_ptr<TransMetaData> newElem(new TransMetaData(*mx.value().get()));
        _cacheNode.insert(mx.key(), newElem);
        _subfactDatas.append(newElem.get());
    }
}

TransMetaData::~TransMetaData() {}

std::shared_ptr<WarnConfig> TransMetaData::setErrString(const QVariant &val, const QString &frame,
                                                        uint8_t id) {
    int st = val.toString().indexOf("{");
    int et = val.toString().indexOf("}");
    if (st >= 0 && et >= 0) {
        std::shared_ptr<WarnConfig> config(new WarnConfig());
        config->decoder(val.toString());
        return config;
    } else {
        _warnConfig._frameItem = id;
        _warnConfig._frameCode = frame;
        _warnConfig.decoder(val.toString());
    }
    return nullptr;
}

int TransMetaData::setProtoValue(const uint8_t *data, int select, int chl,
                                 ReferDeviceCallback refer) {
    if (metaDataType() == MetaDataType::structType) {
        int len = 0;
        QMap<QString, DataTranslator> &translator = loadDataTranslator();
        for (auto get = _cacheNode.begin(); get != _cacheNode.end(); get++) {
            TransMetaData *first = get.value().get();
            assert(first != nullptr);
            if (first == nullptr) {
                COMMON_TRACE("data package unpack {}", select);
                continue;
            }
            auto gets = translator.find(first->getBitSet());
            if (gets != translator.end()) {
                len += gets.value()(first, data, chl, refer);
            }
        }
        return len;
    } else {
        return protoData(data, chl, refer);
    }
}

void TransMetaData::doFactCopy() {
    int left = _copyData.indexOf("{");
    int right = _copyData.indexOf("}");
    if (right > left) {
        int len = (right - left - 1);
        QString value = _copyData.mid(left + 1, len);
        static QRegularExpression mSplitExpress("[,;；]");
        QStringList sets = value.split(mSplitExpress, Qt::SkipEmptyParts);
        if (sets.size() > 1) {
            foreach (QString var, sets) {
                int counts = var.toInt();
                if (counts >= 0 && counts < 8) { // todo...
                    std::shared_ptr<TransMetaData> fectnode = createFactNode();
                    insertFactNode(counts, fectnode); //
                    fectnode->_rawInValue = QString();
                    fectnode->_metaData = _metaData;
                    fectnode->_subfactDatas.clear();
                }
            }
        }
    }
}

std::shared_ptr<TransMetaData> TransMetaData::createFactNode() {
    std::shared_ptr<TransMetaData> v(new TransMetaData());
    assert(v != nullptr); //
    v->setRefItem(itemNum());
    return v;
}

void TransMetaData::insertFactNode(int set, std::shared_ptr<TransMetaData> node) {
    node->setRefItem(itemNum());
    _cacheNode.insert(set, node);
    _subfactDatas.append(node.get());
}
