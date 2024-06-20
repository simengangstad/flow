/**
 * @brief This example implements a simple publisher on the ATZB-X-233-USB
 * which publihes to a base station located at address 0x0000 at a regular
 * interval. Between the transmission, it goes to sleep using the low power
 * module.
 *
 * This example also uses a watch dog.
 *
 * Note: the baudrate for this example is 38400.
 */

#include <led.h>
#include <reset.h>
#include <stringz.h>
#include <sysclk.h>
#include <system.h>
#include <wdt.h>

#include <stdio.h>
#include <string.h>

#include <etl/vector.h>
#include <util/delay.h>

#include "publisher.hpp"

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

/**
 * @brief Called when the application should reset due to an irrecoverable
 * error.
 */
static void reset_callback() { reset(); }

/**
 * @brief This callback is used to disable the watch dog before we go to sleep.
 */
static void sleep_callback(const mesh::publisher::SleepStatus sleep_status) {
    switch (sleep_status) {
    case mesh::publisher::SleepStatus::Entering:

        wdt_disable();
        break;

    case mesh::publisher::SleepStatus::Exiting:

        wdt_enable();
        break;
    }
}

/**
 * @brief Called when it is time to update the payload before a new
 * transmission.
 */
static void payload_update_callback(mesh::Payload& data) {

    LED_On(LED0);
    _delay_ms(10);
    LED_Off(LED0);

    static size_t count     = 0;
    static char message[32] = "";

    sprintf(message, "hello world %d", count);
    count++;

    for (size_t i = 0; (i < strlen(message)) && !data.full(); i++) {
        data.push_back(message[i]);
    }
}

auto main() -> int {
    system_init();
    cpu_irq_enable();

    const reset_cause_t reset_cause = reset_get_causes();

    if (reset_cause != 0) {
        reset_clear_causes(CHIP_RESET_CAUSE_POR | CHIP_RESET_CAUSE_EXTRST |
                           CHIP_RESET_CAUSE_BOD_CPU | CHIP_RESET_CAUSE_OCD |
                           CHIP_RESET_CAUSE_SOFT | CHIP_RESET_CAUSE_SPIKE |
                           CHIP_RESET_CAUSE_WDT);

        // Do something based on what caused the reset
    }

    // We set our address to 0x8000 and our name to mypublisher
    const mesh::Configuration configuration(0x8000,
                                            "mypublisher",
                                            APP_PANID,
                                            APP_CHANNEL,
                                            SECURITY_KEY);

    // The base station we want to publish to
    const mesh::Address recipient_address(0x0000, mesh::Endpoint::Endpoint1);

    // Configure that we publish on a interval of 10 seconds
    mesh::publisher::initialise(configuration,
                                recipient_address,
                                10,
                                payload_update_callback,
                                reset_callback);

    // We register a callback here so that we are notified before the publisher
    // module goes to sleep. With this, we can disable the watch dog before
    // going to sleep.
    mesh::publisher::register_sleep_callback(sleep_callback);

    // Setup watchdog
    wdt_set_timeout_period(WDT_TIMEOUT_PERIOD_8KCLK);
    wdt_enable();

    while (true) {
        mesh::publisher::update();

        // Keep the watch dog happy
        wdt_reset();
    }
}
