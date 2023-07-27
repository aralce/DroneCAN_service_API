#pragma once
#include <canard.h>

#include "CppUTest/TestHarness.h"
#include "CppUTestExt/MockSupport.h"

enum class CAN_bitrate {CAN_1MBITS, CAN_800KBITS, CAN_500KBITS, CAN_250KBITS, CAN_125KBITS, CAN_100KBITS, CAN_50KBITS, CAN_25KBITS};

class CAN_bus_adaptor {
public:
    CAN_bus_adaptor() {

    }

    bool begin(CAN_bitrate baudRate) {
        mock().actualCall("CAN_bus_adaptor->begin")
            .withLongIntParameter("baudRate", (long int)baudRate);
        return mock().returnBoolValueOrDefault(SUCCESS);
    }

    void setPins(int rx, int tx) {
        mock().actualCall("CAN_bus_adaptor->setPins")
            .withIntParameter("rx", rx)
            .withIntParameter("tx", tx);
    }

    bool send_frame(CanardCANFrame& can_frame) {
        mock().actualCall("CAN_bus_adaptor->send_frame")
              .withParameterOfType("CanardCANFrame", "can_frame", (const void*)&can_frame);
        return mock().returnBoolValueOrDefault(SUCCESS);
    }

    void onReceive(void (*onReceive_callback)(int packet_size)) {
        mock().actualCall("CAN_bus_adaptor->onReceive")
              .withPointerParameter("onReceive_callback", (void*)onReceive_callback);
    }

    int read()  {
        mock().actualCall("CAN_bus_adaptor->read");
        return mock().returnIntValueOrDefault(0);
    }

    long get_packet_id()  {
        mock().actualCall("CAN_bus_adaptor->get_packet_id");
        return mock().returnLongIntValueOrDefault(0);
    }

private:
    const int SUCCESS = true;
};

class CanardCANFrame_comparator : public MockNamedValueComparator
{
public:
    virtual bool isEqual(const void* object1, const void* object2)
    {
        CanardCANFrame* can_frame_1 = (CanardCANFrame*)object1;
        CanardCANFrame* can_frame_2 = (CanardCANFrame*)object2;

        if(can_frame_1->id != can_frame_2->id)
            return false;
        if(can_frame_1->data_len != can_frame_2->data_len)
            return false;
        
        uint8_t* pointer_data_1 = can_frame_1->data;
        uint8_t* pointer_data_2 = can_frame_2->data;
        if(!are_data_pointer_equal(pointer_data_1, pointer_data_2, can_frame_1->data_len))
            return false;    
        return true;
    }

    virtual SimpleString valueToString(const void* can_frame)
    {
        return StringFrom(can_frame);
    }
};

