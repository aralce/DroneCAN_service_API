#include <cstdio>

class DroneCan_service {
public:
    static DroneCan_service& getInstance() {
        static DroneCan_service instance;
        return instance;
    }

private:
    DroneCan_service(){}

public:
    DroneCan_service(DroneCan_service const&) = delete;
    void operator=(DroneCan_service const&) = delete;
};
