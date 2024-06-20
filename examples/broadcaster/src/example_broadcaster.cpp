/**
 * @brief This example demonstrates a node broadcasting messages to all devices
 * on the mesh network using the ATmega256RFR2 Xplained Pro.
 *
 * Note: the baudrate for this example is 38400.
 */

#include <led.h>
#include <reset.h>
#include <sio2host.h>
#include <stringz.h>
#include <sysclk.h>
#include <system.h>
#include <tc_megarf.h>
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

#define TIMER (&TCCR5A)

static bool should_broadcast = false;

static void timer_interrupt() {

    static size_t counter = 0;

    counter++;

    // Broadcast every 5th second
    if (counter >= 5) {
        should_broadcast = true;
        counter          = 0;
    }
}

static void start_broadcast_timer() {
    tc_enable(TIMER);

    // Clear timer on compare match mode
    tc_set_mode(TIMER, CTC_Mode1);
    tc_write_clock_source(TIMER, TC_CLKSEL_DIV256_gc);

    // Configure timer for a 1Hz frequency to count every second
    tc_write_cc(TIMER, TC_COMPA, sysclk_get_peripheral_bus_hz(TIMER) / 256);

    tc_set_compa_interrupt_callback(TIMER, timer_interrupt);
    tc_enable_compa_int(TIMER);
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

    printf("\r\n\r\n=== Broadcaster ===\r\n");

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
    const mesh::Configuration configuration(0x0001,
                                            "mybroadcaster",
                                            APP_PANID,
                                            APP_CHANNEL,
                                            SECURITY_KEY);
    mesh::initialise(configuration);

    start_broadcast_timer();

    // Setup watchdog
    wdt_set_timeout_period(WDT_TIMEOUT_PERIOD_1024KCLK);
    wdt_enable(SYSTEM_RESET_MODE);

    while (true) {

        if (should_broadcast) {

            mesh::Payload data;

            data.push_back('h');
            data.push_back('e');
            data.push_back('l');
            data.push_back('l');
            data.push_back('o');

            const mesh::EnqueumentStatus status =
                mesh::enqueue_broadcast(0, mesh::Endpoint::Endpoint2, data);

            if (status != mesh::EnqueumentStatus::Ok) {
                printf("Failed to enqueue broadcast\r\n");
            } else {
                printf("Enqueued broadcasting message!\r\n");
            }

            should_broadcast = false;
        }

        mesh::update();

        // Keep watchdog happy
        wdt_reset();
    }
}
