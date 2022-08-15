#pragma once
#include <cstdio>
#include <cstdint>
#include <common_to_all_mocks.h>

class CAN_class {
public:
    bool begin(uint32_t baudRate) {
        mock().actualCall("can_class->begin")
              .withLongIntParameter("baudRate", baudRate);
        return mock().returnBoolValueOrDefault(true);
    }
    void setPins(uint8_t rx, uint8_t tx) {
        mock().actualCall("can_class->setPins")
              .withIntParameter("rx", rx)
              .withIntParameter("tx", tx);
    }
    
    bool beginExtendedPacket(long id) {
        mock().actualCall("can_class->beginExtendedPacket")
              .withLongIntParameter("id", id);
        return mock().returnBoolValueOrDefault(true);
    }

    size_t write(const uint8_t *buffer, size_t size) {
        mock().actualCall("can_class->write")
              .withPointerParameter("buffer", (void*)buffer)
              .withUnsignedLongIntParameter("size", size);
        return mock().returnUnsignedLongIntValueOrDefault(0);
    }

    bool endPacket() {
        mock().actualCall("can_class->endPacket");
        return mock().returnBoolValueOrDefault(true);
    }

    void onReceive(void (*onReceive_callback)(int)) {
        mock().actualCall("can_class->onReceive")
              .withPointerParameter("onReceive_callback", (void*)onReceive_callback);
    }

    int read() {
        mock().actualCall("can_class->read");
        return mock().returnIntValueOrDefault(0);
    }

};

extern CAN_class CAN;