#include <common_to_all_mocks.h>

bool are_data_pointer_equal(uint8_t* ptr_1, uint8_t* ptr_2, uint16_t data_length) {
    for(int element=0; element<data_length; ++element, ++ptr_1, ++ptr_2)
        if(*ptr_1 != *ptr_2)
            return false;
    return true;
}