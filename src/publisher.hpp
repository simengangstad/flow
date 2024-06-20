/**
 * @brief The publisher abstraction layer encapsulates a mesh
 * device which sends a periodic message and sleeps between the transmissions at
 * a given interval (with optional power save mode).
 */

#ifndef PUBLISHER_HPP
#define PUBLISHER_HPP

#include <stdbool.h>
#include <stdint.h>

#include "mesh.hpp"

#include <etl/vector.h>

namespace mesh::publisher {

    using PayloadUpdateCallback = void (*)(Payload& data);

    using ResetCallback = void (*)();

    enum class SleepStatus { Entering, Exiting };

    using SleepCallback = void (*)(const SleepStatus status);

    /**
     * @brief Initializes the publisher for the mesh network with the given
     * @p configuraiton.
     *
     * @param recipient_address [in] Where to publish the data.
     * @param sleep_interval [in] How long to sleep (in seconds).
     * @param payload_update_callback [in] Called such that the user of this
     * module can update the payload (do a sensor reading and update the data
     * etc.) before it is transmitted.
     * @param reset_callback [in] Called when the application should reset due
     * to an internal error in the network layer.
     */
    void initialise(const mesh::Configuration& configuration,
                    const mesh::Address& recipient_address,
                    uint32_t sleep_interval,
                    PayloadUpdateCallback payload_update_callback,
                    ResetCallback reset_callback);

    /**
     * @brief Registers callback that will be called before and after a sleep.
     */
    void register_sleep_callback(SleepCallback sleep_callback);

    /**
     * @brief Updates the state of the publisher and the network layer.
     */
    void update();
} // namespace mesh::publisher

#endif
