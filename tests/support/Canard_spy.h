#pragma once

#include "Canard_wrapper.h"

class Canard_spy
{
public:
    Canard_spy(Canard& canard)
    : _canard(canard)
    {

    }

    uint8_t get_node_id() const {return _canard.canard_instance.node_id;}

private:
    Canard& _canard;
};