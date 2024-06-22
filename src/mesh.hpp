#ifndef MESH_HPP
#define MESH_HPP

#include <stddef.h>
#include <stdint.h>

#include "nwk.h"

#include <etl/vector.h>

#define DEVICE_NAME_LENGTH  (24)
#define SECURITY_KEY_LENGTH (16)

namespace mesh {

    /**
     * @brief Every device has 15 endpoints (actually 16, but 0 is reserved by
     * the network layer). This can be seen as different "channels"
     * for which communication can occur on. There can be one for, e.g., command
     * and control and one for just reporting sensor data.
     */
    enum class Endpoint {
        Endpoint1 = 1,
        Endpoint2,
        Endpoint3,
        Endpoint4,
        Endpoint5,
        Endpoint6,
        Endpoint7,
        Endpoint8,
        Endpoint9,
        Endpoint10,
        Endpoint11,
        Endpoint12,
        Endpoint13,
        Endpoint14,
        Endpoint15,
    };

    /**
     * @brief Convenience struct for encapsulating the destination node's
     * address and the endpoint to transmit messages to.
     */
    struct Address {
        uint16_t address;
        Endpoint endpoint;

        Address(uint16_t address, Endpoint endpoint);
    };

    /**
     * @brief Configuration for every device on the network.
     */
    struct Configuration {
        /**
         * @brief Address of the device. < 0x8000 is for routers
         * (coordinator/base and relays) and >= 0x8000 for non-routers (end
         * devices).
         */
        uint16_t address;

        /**
         * @brief Human-friendly device name (including null-termination).
         */
        char device_name[DEVICE_NAME_LENGTH];

        /**
         * @brief Identifier of the Personal Area Network. This has to be the
         * same for each device on the network.
         */
        uint16_t personal_area_network_identifier;

        /**
         * @brief Channel for the network, also has to be equal for each device
         * on the network.
         */
        uint8_t channel;

        /**
         * @brief The key the messages will be encrypted with.
         */
        char security_key[NWK_SECURITY_KEY_SIZE];

        Configuration(uint16_t address,
                      const char device_name[DEVICE_NAME_LENGTH],
                      uint16_t personal_area_network_identifier,
                      uint8_t channel,
                      const char security_key[SECURITY_KEY_LENGTH]);
    };

    /**
     * @brief Initializes a device for the mesh network with the given @p
     * configuration.
     */
    void initialise(const Configuration& configuration);

    /**
     * @brief Updates the network and physical layer, should be called regularly
     * in the main loop of the applicaiton.
     */
    void update();

    // ------------------------------------------------------------------------
    //                                 Listener
    // ------------------------------------------------------------------------
    //
    using ReceiveCallback =
        void (*)(const uint16_t source_address,
                 const char source_device_name[DEVICE_NAME_LENGTH],
                 uint8_t* data,
                 uint8_t size);

    /**
     * @brief Registers to listen on a particular @p endpoint. When another
     * device transmits to us on this given @p endpint, @p receive_callback will
     * be callback with the data and the source address of the data.
     *
     * @see #Endpoint
     */
    void register_listener(const Endpoint& endpoint,
                           ReceiveCallback receive_callback);

    // ------------------------------------------------------------------------
    //                               Transmission
    // ------------------------------------------------------------------------

    /**
     * @brief The various status code reported from the network layer from a
     * transmission.
     */
    enum class TransmissionStatus {
        Success              = NWK_SUCCESS_STATUS,
        Error                = NWK_ERROR_STATUS,
        OutOfMemory          = NWK_OUT_OF_MEMORY_STATUS,
        NoAck                = NWK_NO_ACK_STATUS,
        NoRoute              = NWK_NO_ROUTE_STATUS,
        ChannelAccessFailure = NWK_PHY_CHANNEL_ACCESS_FAILURE_STATUS,
        PhyNoAck             = NWK_PHY_NO_ACK_STATUS,
        None
    };

    struct TransmissionResult {
        uint16_t message_identifier;
        TransmissionStatus status;
    };

    using TransmissionCallback =
        void (*)(const TransmissionResult transmission_result);

    /**
     * @brief Registers a function which will be called when a transmission has
     * completed.
     */
    void
    register_transmission_callback(TransmissionCallback transmission_callback);

    /**
     * @brief Used to determine the status of a transmission enqueuement.
     */
    enum class EnqueumentStatus { Ok, TransmissionBufferFull };

    constexpr uint8_t MAX_TRANSMISSION_PACKET_SIZE = 196;

    using Payload = etl::vector<uint8_t, mesh::MAX_TRANSMISSION_PACKET_SIZE>;

    /**
     * @brief Queues a message for transmission. Note that the data will be
     * copied to an internal buffer in this function, so it is safe that @p data
     * goes out of scope.
     *
     * @param message_identifier [in] Identifier which can be used to
     * distinguish this mesage in the transmission callback.
     * @param destination_address [in] Where to send the message.
     * @param data [in] The payload to send.
     *
     * @return See #EnqueumentStatus
     */
    [[nodiscard]] auto
    enqueue_direct_transmission(uint16_t message_identifier,
                                const Address& destination_address,
                                const Payload& data) -> EnqueumentStatus;

    /**
     * @brief Same as #enqueue_direct_transmission, except that the message is
     * broadcasted and all devices on the mesh network can receive it.
     */
    [[nodiscard]] auto enqueue_broadcast(uint16_t message_identifier,
                                         Endpoint endpoint,
                                         const Payload& data)
        -> EnqueumentStatus;

    // ------------------------------------------------------------------------
    //                              Low Power
    // ------------------------------------------------------------------------

    /**
     * @brief Make the network layer sleep to save power.
     */
    void sleep();

    /**
     * @brief Wake up the network layer.
     */
    void wakeup();

} // namespace mesh

#endif
