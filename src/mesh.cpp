#include "mesh.hpp"

#include <etl/circular_buffer.h>

#include "board.h"
#include "phy.h"
#include "sys.h"

#include <stdio.h>
#include <string.h>

namespace mesh {

    static char device_name[DEVICE_NAME_LENGTH];

    Address::Address(const uint16_t address_parameter,
                     const Endpoint endpoint_parameter)
        : address(address_parameter), endpoint(endpoint_parameter) {}

    Configuration::Configuration(
        const uint16_t address_parameter,
        const char device_name_parameter[DEVICE_NAME_LENGTH],
        const uint16_t personal_area_network_identifier_parameter,
        const uint8_t channel_parameter,
        const char security_key_parameter[SECURITY_KEY_LENGTH])
        : address(address_parameter),
          personal_area_network_identifier(
              personal_area_network_identifier_parameter),
          channel(channel_parameter) {

        memcpy(device_name, device_name_parameter, sizeof(device_name));
        memcpy(security_key, security_key_parameter, sizeof(security_key));
    }

    void initialise(const Configuration& configuration) {

#ifdef MESH_ENABLE_LOGGING
        printf("\r\n\r\n");
        printf("Initializing mesh stack\r\n");
        printf("- Device address: 0x%X\r\n", configuration.address);
        printf("- Device name: %s\r\n", configuration.device_name);
        printf("- PAN ID: 0x%X\r\n",
               configuration.personal_area_network_identifier);
        printf("- Channel: 0x%X\r\n", configuration.channel);
        printf("\r\n\r\n");
#endif

        memcpy(device_name, configuration.device_name, sizeof(device_name));
        device_name[sizeof(device_name) - 1] = '\0';

        SYS_Init();

        NWK_SetAddr(configuration.address);
        NWK_SetPanId(configuration.personal_area_network_identifier);

        PHY_SetChannel(configuration.channel);
        PHY_SetRxState(true);

        char security_key[sizeof(configuration.security_key)];
        memcpy(security_key,
               configuration.security_key,
               sizeof(configuration.security_key));

        const size_t security_key_length = strnlen(security_key,
                                                   sizeof(security_key));

        // Fill with zeros at the end if it is not a full length key. If, not we
        // might have some trailing bytes which are not equal between devices
        // even though the string part of the buffer is equal
        if (security_key_length < sizeof(security_key)) {
            memset(security_key + security_key_length,
                   0,
                   sizeof(security_key) - security_key_length);
        }

        NWK_SetSecurityKey((uint8_t*)security_key);
    }

    void update() { SYS_TaskHandler(); }

    // ------------------------------------------------------------------------
    //                                 Listener
    // ------------------------------------------------------------------------

    static ReceiveCallback receive_callbacks[15];

    static auto internal_receive_callback(NWK_DataInd_t* indication) -> bool {

        // We include the device name in the payload, so we have to extract that
        // and only pass the data that the user enqueued in the callback
        char source_device_name[DEVICE_NAME_LENGTH];
        memcpy(source_device_name, indication->data, DEVICE_NAME_LENGTH);

        if (receive_callbacks[indication->dstEndpoint - 1] != nullptr) {
            receive_callbacks[indication->dstEndpoint - 1](
                indication->srcAddr,
                source_device_name,
                indication->data + DEVICE_NAME_LENGTH,
                indication->size - DEVICE_NAME_LENGTH);
        }

        return true;
    }

    void register_listener(const Endpoint& endpoint,
                           ReceiveCallback receive_callback) {

        receive_callbacks[static_cast<uint8_t>(endpoint) - 1] =
            receive_callback;

        NWK_OpenEndpoint(static_cast<uint8_t>(endpoint),
                         internal_receive_callback);
    }

    // ------------------------------------------------------------------------
    //                               Transmission
    // ------------------------------------------------------------------------

    struct TransmissionPacket {
        uint16_t message_identifier;

        uint8_t data[MAX_TRANSMISSION_PACKET_SIZE + DEVICE_NAME_LENGTH];

        NWK_DataReq_t request;
    };

    static etl::circular_buffer<TransmissionPacket, 4> transmission_packets;

    static TransmissionCallback transmission_callback;

    static void internal_transmission_callback(struct NWK_DataReq_t* request) {

        if (transmission_packets.empty()) {
            return;
        }

        if (transmission_callback != nullptr) {

            const TransmissionPacket& packet = transmission_packets.front();

            transmission_callback(TransmissionResult{
                packet.message_identifier,
                static_cast<TransmissionStatus>(request->status)});
        }

        transmission_packets.pop();
    }

    void register_transmission_callback(
        TransmissionCallback transmission_callback_paramter) {
        transmission_callback = transmission_callback_paramter;
    }

    static auto enqueue(
        const uint16_t message_identifier,
        const Address& destination_address,
        const uint8_t options,
        const etl::vector<uint8_t, mesh::MAX_TRANSMISSION_PACKET_SIZE>& data)
        -> EnqueumentStatus {

        if (transmission_packets.full()) {
            return EnqueumentStatus::TransmissionBufferFull;
        }

        transmission_packets.push(TransmissionPacket{});

        TransmissionPacket& packet = transmission_packets.back();
        memcpy(packet.data, device_name, DEVICE_NAME_LENGTH);
        memcpy(packet.data + DEVICE_NAME_LENGTH, data.data(), data.size());

        packet.message_identifier  = message_identifier;
        packet.request.dstAddr     = destination_address.address;
        packet.request.dstEndpoint = static_cast<uint8_t>(
            destination_address.endpoint);
        packet.request.srcEndpoint = static_cast<uint8_t>(
            destination_address.endpoint);
        packet.request.options = options;
        packet.request.size    = data.size() + DEVICE_NAME_LENGTH;
        packet.request.confirm = internal_transmission_callback;
        packet.request.data    = packet.data;

        NWK_DataReq(&packet.request);

        return EnqueumentStatus::Ok;
    }

    auto enqueue_direct_transmission(const uint16_t message_identifier,
                                     const Address& destination_address,
                                     const Payload& data) -> EnqueumentStatus {

        return enqueue(message_identifier,
                       destination_address,
                       NWK_OPT_ACK_REQUEST | NWK_OPT_ENABLE_SECURITY,
                       data);
    }

    auto enqueue_broadcast(const uint16_t message_identifier,
                           const Endpoint endpoint,
                           const Payload& data) -> EnqueumentStatus {

        const Address destination_address(0xFFFF, endpoint);

        return enqueue(message_identifier,
                       destination_address,
                       NWK_OPT_BROADCAST_PAN_ID | NWK_OPT_ENABLE_SECURITY,
                       data);
    }

    // ------------------------------------------------------------------------
    //                              Low Power
    // ------------------------------------------------------------------------

    void sleep() {
        if (!NWK_Busy()) {
            NWK_SleepReq();
        }
    }

    void wakeup() { NWK_WakeupReq(); }

} // namespace mesh
