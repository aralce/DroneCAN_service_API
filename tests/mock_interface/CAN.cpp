#include <CAN.h>
#include "CppUTest/TestHarness.h"
#include "CppUTestExt/MockSupport.h"

int CAN::begin(long baudRate) {
    mock().actualCall("begin").withLongIntParameter("baudRate", baudRate);
    return mock().intReturnValue();
}

void CAN::setPins(int rx, int tx) {
    mock().actualCall("setPins").withIntParameter("rx", rx).withIntParameter("tx", tx);
}

CAN can;