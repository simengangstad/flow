/**
 * @brief This example implements a simple base station on the ATmega256RFR2
 * Xplained Pro which listens on endpoint 1 for sensor data and endpoint 2 for
 * broadcast data.
 *
 * Note: the baudrate for this example is 38400.
 */

#include <led.h>
#include <reset.h>
#include <sio2host.h>
#include <stringz.h>
#include <sysclk.h>
#include <system.h>
#include <wdt_megarf.h>

#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>

#include <etl/vector.h>

#include <util/delay.h>

#include "mesh.hpp"

// These are from the env file in the top of the examples folder:
//
// _SECURITY_KEY=MySecurityKey
// _APP_PANID=0x1234
// _APP_CHANNEL=0x0f
//
// This is to increase security and not keep these settings under source control
// (e.g., keep the env file in .gitignore so that the key is not exposed in a
// repository).
//
// Look in the CMakeLists.txt for how these elements are extracted from the file
// and set as preprocessor directives.

#define APP_PANID    _APP_PANID
#define APP_CHANNEL  _APP_CHANNEL
#define SECURITY_KEY ASTRINGZ(_SECURITY_KEY)

static void receive_callback(__attribute__((unused))
                             const uint16_t source_address,
                             __attribute__((unused))
                             const char source_device_name[DEVICE_NAME_LENGTH],
                             uint8_t* data,
                             uint8_t size) {

    data[size] = '\0';

    LED_On(LED0);
    _delay_ms(10);
    LED_Off(LED0);

    printf("Received from %04X (%s), payload of size %d: %s\r\n",
           source_address,
           source_device_name,
           size,
           data);
}

static void
broadcast_callback(__attribute__((unused)) const uint16_t source_address,
                   __attribute__((unused))
                   const char source_device_name[DEVICE_NAME_LENGTH],
                   uint8_t* data,
                   uint8_t size) {
    data[size] = '\0';

    LED_On(LED0);
    _delay_ms(10);
    LED_Off(LED0);

    printf("Received broadcast from %04X (%s), payload of size %d: %s\r\n",
           source_address,
           source_device_name,
           size,
           data);
}

constexpr const size_t NUMBER_OF_RESET_CAUSES = 5;

constexpr const char* RESET_CAUSE[] = {"Power-on",
                                       "External",
                                       "Brown-out",
                                       "Watchdog",
                                       "JTAG (programming)"};

auto main() -> int {
    system_init();
    cpu_irq_enable();
    sio2host_init();

    printf("\r\n\r\n=== Base station ===\r\n");

    const reset_cause_t reset_cause = reset_get_causes();

    if (reset_cause != 0) {
        reset_clear_causes(CHIP_RESET_CAUSE_EXTRST | CHIP_RESET_CAUSE_BOD_CPU |
                           CHIP_RESET_CAUSE_POR | CHIP_RESET_CAUSE_JTAG |
                           CHIP_RESET_CAUSE_WDT);

        printf("System was reset. Cause(s):\r\n");

        for (size_t i = 0; i < NUMBER_OF_RESET_CAUSES; i++) {

            if ((reset_cause & (1U << i)) != 0) {
                printf("- %s\r\n", RESET_CAUSE[i]);
            }
        }
    }

    // Set that we have address 0x0000 and that our name in the mesh network is
    // mybasestation
    const mesh::Configuration configuration(0x0000,
                                            "mybasestation",
                                            APP_PANID,
                                            APP_CHANNEL,
                                            SECURITY_KEY);
    mesh::initialise(configuration);

    // Register listener such that we can be notified when other nodes transmit
    // data to us
    mesh::register_listener(mesh::Endpoint::Endpoint1, receive_callback);

    // Register broadcast callback on endpoint 2
    mesh::register_listener(mesh::Endpoint::Endpoint2, broadcast_callback);

    // Setup watchdog
    wdt_set_timeout_period(WDT_TIMEOUT_PERIOD_1024KCLK);
    wdt_enable(SYSTEM_RESET_MODE);

    while (true) {
        mesh::update();

        // Keep watchdog happy
        wdt_reset();
    }
}
