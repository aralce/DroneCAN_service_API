#include "common_to_paramGetSet.h"

DroneCAN_service* droneCAN_service = nullptr;

void teardown_paramGetSet_tests() {
    if (droneCAN_service != nullptr) {
        delete droneCAN_service;
       droneCAN_service = nullptr;
    }
    teardown_mocks();
}