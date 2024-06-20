#include "publisher.hpp"

#include <reset.h>

#include <stdio.h>

#include <util/delay.h>

#include "low_power.hpp"

namespace mesh::publisher {

    /**
     * @brief States in state machine of the device.
     */
    enum class State {
        UpdatingPayload = 0,
        Transmitting,
        WaitingForTransmitAcknowledgement,
        Sleeping
    };

    /**
     * @brief Current state.
     */
    static State state = State::UpdatingPayload;

    /**
     * Used to detect if the transmission callback was called.
     */
    static bool got_transmission_result = false;

    /**
     * @brief The last transmission result.
     */
    static mesh::TransmissionResult transmission_result;

    /**
     * @brief Which address to send the messages to.
     */
    static mesh::Address recipient_address(0, Endpoint::Endpoint1);

    /**
     * @brief Called so that the user of the device module can update the
     * payload of the message being transmitted.
     */
    static PayloadUpdateCallback payload_update_callback;

    /**
     * @brief How long to sleep for (in seconds).
     */
    static uint32_t sleep_interval;

    /**
     * @brief Called before and after sleep (if registered).
     */
    static SleepCallback sleep_callback;

    /**
     * @brief Called when we encounter an irrecoverable error and need to notify
     * the application that we should reset.
     */
    static ResetCallback reset_callback;

    /**
     * @brief Called when a transmission has completed with or without an error.
     */
    static void transmission_callback(
        const mesh::TransmissionResult transmission_result_parameter) {
        transmission_result     = transmission_result_parameter;
        got_transmission_result = true;
    }

    void initialise(const mesh::Configuration& configuration,
                    const mesh::Address& recipient_address_parameter,
                    const uint32_t sleep_interval_parameter,
                    PayloadUpdateCallback payload_update_callback_parameter,
                    ResetCallback reset_callback_parameter) {

        mesh::initialise(configuration);
        mesh::register_transmission_callback(transmission_callback);

        recipient_address       = recipient_address_parameter;
        payload_update_callback = payload_update_callback_parameter;
        reset_callback          = reset_callback_parameter;
        sleep_interval          = sleep_interval_parameter;

#ifndef MESH_ENABLE_LOGGING
        low_power::initialize();
#endif
    }

    void register_sleep_callback(const SleepCallback sleep_callback_parameter) {
        sleep_callback = sleep_callback_parameter;
    }

    void update() {

        static Payload data;

        switch (state) {

        case State::UpdatingPayload:
            data.clear();
            payload_update_callback(data);

            state = State::Transmitting;

            break;

        case State::Transmitting: {

            const mesh::EnqueumentStatus status =
                mesh::enqueue_direct_transmission(0, recipient_address, data);

            switch (status) {

            case mesh::EnqueumentStatus::TransmissionBufferFull:
                // We're only sending one message per iteration and wait for
                // acknowledgement for each message, so we should not arrive at
                // this state. Time to reset.

#ifdef MESH_ENABLE_LOGGING
                printf("Transmission buffer is full, can't enqueue more "
                       "data!\r\rn");
#endif
                if (reset_callback != nullptr) {
                    reset_callback();
                }

                break;

            case mesh::EnqueumentStatus::Ok:
#ifdef MESH_ENABLE_LOGGING
                printf("Enqueued message\r\n");
#endif
                state = State::WaitingForTransmitAcknowledgement;
                break;
            }

        }

        break;

        case State::WaitingForTransmitAcknowledgement:

            // The wait time here is defined by the mesh configuration,
            // located in the config.h file. Thus we don't need an application
            // layer timeout timer here as it will be driven by the network
            // layer.

            if (!got_transmission_result) {
                break;
            }

            switch (transmission_result.status) {
            case mesh::TransmissionStatus::Success:

#ifdef MESH_ENABLE_LOGGING
                printf("Got ACK on message \r\n");
#endif

                break;

            default:
#ifdef MESH_ENABLE_LOGGING
                printf("Error, did not get ACK on message with identifier %d. "
                       "Error "
                       "code: 0x%X. Recipient might not be online.\r\n",
                       transmission_result.message_identifier,
                       static_cast<uint8_t>(transmission_result.status));
#endif

                // The network might be down for the moment. We do a sleep and
                // then we'll try again

                break;
            }

            got_transmission_result = false;
            state                   = State::Sleeping;

            break;

        case State::Sleeping:
            if (sleep_callback != nullptr) {
                sleep_callback(SleepStatus::Entering);
            }

#ifdef MESH_ENABLE_LOGGING
            printf("Sleeping disabled whilst logging, using delay...\r\n");

            // Have to do it in a loop as _delay_ms expects compile time
            // constant
            for (size_t i = 0; i < sleep_interval; i++) { _delay_ms(1000); }

#else
            low_power::sleep(sleep_interval);
#endif

            if (sleep_callback != nullptr) {
                sleep_callback(SleepStatus::Exiting);
            }

            state = State::UpdatingPayload;

            break;
        }

        mesh::update();
    }
} // namespace mesh::publisher
