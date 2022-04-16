#pragma once
#include <common_to_all_mocks.h>

typedef struct {
    uint8_t* pointer;
    size_t length;
}CAN_data_t;

typedef struct {
    long id;
    CAN_data_t data;
}CAN_bus_driver_fame_t;

class CAN_bus_adaptor {
public:
    CAN_bus_adaptor() {

    }

    int begin(long baudRate) {
        mock().actualCall("begin")
            .withLongIntParameter("baudRate", baudRate);
        return mock().intReturnValue();
    }

    void setPins(int rx, int tx) {
        mock().actualCall("setPins")
            .withIntParameter("rx", rx)
            .withIntParameter("tx", tx);
    }

    bool send_frame(CAN_bus_driver_fame_t can_frame) {
        return true;
    }
};
