/**
 *
 * \file
 *
 * \brief Common Standard I/O Serial Management.
 *
 * This file defines a useful set of functions for the Stdio Serial interface on
 *AVR and SAM devices.
 *
 * Copyright (c) 2009-2018 Microchip Technology Inc. and its subsidiaries.
 *
 * \asf_license_start
 *
 * \page License
 *
 * Subject to your compliance with these terms, you may use Microchip
 * software and any derivatives exclusively with Microchip products.
 * It is your responsibility to comply with third party license terms applicable
 * to your use of third party software (including open source software) that
 * may accompany Microchip software.
 *
 * THIS SOFTWARE IS SUPPLIED BY MICROCHIP "AS IS". NO WARRANTIES,
 * WHETHER EXPRESS, IMPLIED OR STATUTORY, APPLY TO THIS SOFTWARE,
 * INCLUDING ANY IMPLIED WARRANTIES OF NON-INFRINGEMENT, MERCHANTABILITY,
 * AND FITNESS FOR A PARTICULAR PURPOSE. IN NO EVENT WILL MICROCHIP BE
 * LIABLE FOR ANY INDIRECT, SPECIAL, PUNITIVE, INCIDENTAL OR CONSEQUENTIAL
 * LOSS, DAMAGE, COST OR EXPENSE OF ANY KIND WHATSOEVER RELATED TO THE
 * SOFTWARE, HOWEVER CAUSED, EVEN IF MICROCHIP HAS BEEN ADVISED OF THE
 * POSSIBILITY OR THE DAMAGES ARE FORESEEABLE.  TO THE FULLEST EXTENT
 * ALLOWED BY LAW, MICROCHIP'S TOTAL LIABILITY ON ALL CLAIMS IN ANY WAY
 * RELATED TO THIS SOFTWARE WILL NOT EXCEED THE AMOUNT OF FEES, IF ANY,
 * THAT YOU HAVE PAID DIRECTLY TO MICROCHIP FOR THIS SOFTWARE.
 *
 * \asf_license_stop
 *
 ******************************************************************************/
/*
 * Support and FAQ: visit <a href="https://www.microchip.com/support/">Microchip
 * Support</a>
 */

#ifndef _STDIO_SERIAL_H_
#define _STDIO_SERIAL_H_

/**
 * \defgroup group_common_utils_stdio_stdio_serial Standard serial I/O (stdio)
 * \ingroup group_common_utils_stdio
 *
 * Common standard serial I/O management driver that
 * implements a stdio serial interface on AVR and SAM devices.
 *
 * \{
 */

#include <compiler.h>
#include <serial.h>
#include <stdio.h>

#if (XMEGA || MEGA_RF) && defined(__GNUC__)
extern int _write(char c, int* f);
extern int _read(int* f);
#endif

//! Pointer to the base of the USART module instance to use for stdio.
extern volatile void* volatile stdio_base;
//! Pointer to the external low level write function.
extern int (*ptr_put)(void volatile*, char);

//! Pointer to the external low level read function.
extern void (*ptr_get)(void volatile*, char*);

#if (XMEGA || MEGA_RF || UC3)

/*! \brief Initializes the stdio in Serial Mode.
 *
 * \param usart       Base address of the USART instance.
 * \param opt         Options needed to set up RS232 communication (see \ref
 * usart_options_t).
 *
 */
static inline void stdio_serial_init(volatile void* usart,
                                     const usart_serial_options_t* opt) {
    stdio_base = (void*)usart;
#pragma GCC diagnostic push
#pragma GCC diagnostic ignore "-Wpedantic"
    ptr_put = (status_code_t(*)(void volatile*, char)) & usart_serial_putchar;
#pragma GCC diagnostic pop
    ptr_get = (void (*)(void volatile*, char*)) & usart_serial_getchar;
#if (XMEGA || MEGA_RF)
    usart_serial_init((USART_t*)usart, opt);
#elif UC3
    usart_serial_init(usart, (usart_serial_options_t*)opt);
#else
#error Unsupported chip type
#endif

#if defined(__GNUC__)
#if (XMEGA || MEGA_RF)
    // For AVR GCC libc print redirection uses fdevopen.
    fdevopen((int (*)(char, FILE*))(_write), (int (*)(FILE*))(_read));
#endif
#if UC3
    // For AVR32 and SAM GCC
    // Specify that stdout and stdin should not be buffered.
    setbuf(stdout, NULL);
    setbuf(stdin, NULL);
    // Note: Already the case in IAR's Normal DLIB default configuration
    // and AVR GCC library:
    // - printf() emits one character at a time.
    // - getchar() requests only 1 byte to exit.
#endif
#endif
}

#endif

#if SAM0
/** \brief Initializes the stdio in Serial Mode.
 *
 * \param module       Software USART instance to associate with the hardware.
 * \param hw           Base address of the USART hardware instance.
 * \param config       USART configuration parameters for the STDIO stream.
 *
 */
static inline void stdio_serial_init(struct usart_module* const module,
                                     usart_inst_t const hw,
                                     const struct usart_config* const config) {
    stdio_base = (void*)module;

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wcast-function-type"

    ptr_put = (int (*)(void volatile*, char))(&usart_serial_putchar);
    ptr_get = (void (*)(void volatile*, char*))(&usart_serial_getchar);

#pragma GCC diagnostic pop

    usart_serial_init(module, hw, config);
#if defined(__GNUC__)
    // Specify that stdout and stdin should not be buffered.
    setbuf(stdout, NULL);
    setbuf(stdin, NULL);
    // Note: Already the case in IAR's Normal DLIB default configuration
    // and AVR GCC library:
    // - printf() emits one character at a time.
    // - getchar() requests only 1 byte to exit.
#endif
}
#endif

#if SAMB
/** \brief Initializes the stdio in Serial Mode.
 *
 * \param module       Software UART instance to associate with the hardware.
 * \param hw           Base address of the UART hardware instance.
 * \param config       UART configuration parameters for the STDIO stream.
 *
 */
static inline void stdio_serial_init(struct uart_module* const module,
                                     Uart* const hw,
                                     const struct uart_config* const config) {
    stdio_base = (void*)module;
    ptr_put    = (int (*)(void volatile*, char)) & usart_serial_putchar;
    ptr_get    = (void (*)(void volatile*, char*)) & usart_serial_getchar;

    usart_serial_init(module, hw, config);
#if defined(__GNUC__)
    // Specify that stdout and stdin should not be buffered.
    setbuf(stdout, NULL);
    setbuf(stdin, NULL);
    // Note: Already the case in IAR's Normal DLIB default configuration
    // and AVR GCC library:
    // - printf() emits one character at a time.
    // - getchar() requests only 1 byte to exit.
#endif
}
#endif

/**
 * \}
 */

#endif // _STDIO_SERIAL_H_
