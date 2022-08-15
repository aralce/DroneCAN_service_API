#include <DroneCAN_message_sender.h>

DroneCAN_message_sender::DroneCAN_message_sender(Canard& canard, CAN_bus_adaptor& can_driver, droneCAN_handle_error_t handle_error) {
    mock().actualCall("DroneCAN_message_sender->constructor")
          .withParameter("canard", (const void*)&canard)
          .withParameter("can_driver", (const void*)&can_driver)
          .withPointerParameter("handle_error", (void*)handle_error);
}

bool DroneCAN_message_sender::is_healthy() {
    mock().actualCall("DroneCAN_message_sender->is_healthy");
    return mock().returnBoolValueOrDefault(true);
}