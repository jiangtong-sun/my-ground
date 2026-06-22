#include "SimpleSerialListener.h"
#include "factdecodermgr.h"

bool openSerialService(itas109::CSerialPort &main_serail, SimpleSerialListener &listener,
                       SerialNodeConfig *net){
    assert(net != nullptr);
    if (net == nullptr) {
        COMMON_INFO("invalid param");
        return false;
    }

    // close serial port
    main_serail.close();

    // wait system resource release
    std::this_thread::sleep_for(std::chrono::milliseconds(100));

    main_serail.init(net->commName().toStdString().c_str(), itas109::BaudRate(net->bautrate()),
                     itas109::Parity(net->parity()), itas109::DataBits(net->databit()),
                     itas109::StopBits(net->stopbit()), itas109::FlowNone, NetworkSerailCache);

    main_serail.setReadIntervalTimeout(0);
    main_serail.setByteReadBufferFullNotify(NetworkSerailCache / 2);

    COMMON_INFO("serail {}, bautrate: {}, parity: {}, databit: {}, stopbit: {}",
                net->commName().toStdString().c_str(), net->bautrate(), net->parity(),
                net->databit(), net->stopbit());

    if (main_serail.open()) {
        main_serail.connectReadEvent(&listener);
        return true;
    }

    return false;
}

void SimpleSerialListener::onReadEvent(const char *portName, unsigned int readBufferLen) {
    if (readBufferLen > 0) {
        int len = serial_cache.checkLength(readBufferLen);
        char *buffer = (char *) (serial_cache.data);
        if (serialPort != nullptr) {
            int recLen = serialPort->readData(buffer + len, readBufferLen);
            if (recLen > 0) {
                serial_datalen += recLen;
                serial_cache.length += recLen;
            }
            FactDecoderMgr::dataDecoder(serial_cache, ChannelSelection::SerialNetwork,
                                        sessioncache);
        } else {
            COMMON_INFO("invalid session or invalid serial port");
        }
    }
};
