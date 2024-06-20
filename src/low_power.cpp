#include "low_power.hpp"

#include "mesh.hpp"
#include "sleep_mgr.h"

namespace low_power {

    void initialize() { sm_init(); }

    void sleep(const uint32_t interval) {

        mesh::sleep();
        sm_sleep(interval);
        mesh::wakeup();
    }

} // namespace low_power
