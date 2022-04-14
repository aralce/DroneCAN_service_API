#pragma once

class CAN {
public:
    int begin(long baudRate);
    void setPins(int rx, int tx);
};

extern CAN can;