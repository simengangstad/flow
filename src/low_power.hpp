#ifndef LOW_POWER_HPP
#define LOW_POWER_HPP

#include <stdint.h>

namespace low_power {

    void initialize();

    /**
     * @brief Puts the device and the wireless modem to sleep for @p interval
     * seconds.
     */
    void sleep(uint32_t interval);
} // namespace low_power

#endif
