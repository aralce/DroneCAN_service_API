#include <CAN.h>
#include <common_to_all_mocks.h>

int CAN::begin(long baudRate) {
    mock().actualCall("begin").withLongIntParameter("baudRate", baudRate);
    return mock().intReturnValue();
}

void CAN::setPins(int rx, int tx) {
    mock().actualCall("setPins").withIntParameter("rx", rx).withIntParameter("tx", tx);
}

CAN can;