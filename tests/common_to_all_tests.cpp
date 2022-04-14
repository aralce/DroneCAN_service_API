#include "common_to_all_tests.h"

void teardown_mocks()
{
    mock().checkExpectations();
    mock().clear();
}