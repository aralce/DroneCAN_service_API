#include <CAN_bus_adaptor.h>

class CAN_bus_adaptor_factory {
public:
    static CAN_bus_adaptor* get_CAN_bus_adaptor() {
        CAN_bus_adaptor* can = new CAN_bus_adaptor();
        return can;
    }

private:
    CAN_bus_adaptor_factory() {}
    CAN_bus_adaptor_factory(const CAN_bus_adaptor_factory&) = delete;
    CAN_bus_adaptor_factory& operator=(const CAN_bus_adaptor_factory&) = delete;
    CAN_bus_adaptor_factory& operator=(CAN_bus_adaptor_factory&&) = delete;
};