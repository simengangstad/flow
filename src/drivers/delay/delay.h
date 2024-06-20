/**
 * \file
 *
 * \brief Common Delay Service
 *
 * Copyright (c) 2014-2018 Microchip Technology Inc. and its subsidiaries.
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
 */
/*
 * Support and FAQ: visit <a href="https://www.microchip.com/support/">Microchip
 * Support</a>
 */
#ifndef _DELAY_H_
#define _DELAY_H_

#include <parts.h>

#ifdef __cplusplus
extern "C" {
#endif

#if UC3
#include <cycle_counter.h>
#include <sysclk.h>

#elif XMEGA
#include "xmega/cycle_counter.h"
#include <sysclk.h>

#elif MEGA
#include "mega/cycle_counter.h"
#include <sysclk.h>

#elif SAM
#ifdef SYSTICK_MOD
#include "sam0/systick_counter.h"
#endif
#ifdef CYCLE_MODE
#include "sam0/cycle_counter.h"
#endif

#else
#error "Architecture not supported"
#endif

/**
 * @defgroup group_common_services_delay Busy-Wait Delay Routines
 *
 * This module provides simple loop-based delay routines for those
 * applications requiring a brief wait during execution. Common API
 * for UC3, XMEGA, and AVR MEGA.
 *
 * @{
 */

/**
 * @def F_CPU
 * @brief MCU Clock Frequency (Hertz)
 *
 * @deprecated
 * The \ref F_CPU configuration constant is used for compatibility with the
 * \ref group_common_services_delay routines. The common loop-based delay
 * routines are designed to use the \ref clk_group modules while anticipating
 * support for legacy applications assuming a statically defined clock
 * frequency. Applications using a statically configured MCU clock frequency
 * can define \ref F_CPU (Hertz), in which case the common delay routines will
 * use this value rather than calling sysclk_get_cpu_hz() to get the current
 * MCU clock frequency.
 */
#ifndef F_CPU
#define F_CPU sysclk_get_cpu_hz()
#endif

#if (MEGA || XMEGA)

/**
 * @def delay_s
 * @brief Delay in seconds.
 * @param delay Delay in seconds
 */
#define delay_s(delay) \
    ((delay) ? cpu_delay_ms(1000 * delay, F_CPU) : cpu_delay_us(1, F_CPU))

/**
 * @def delay_ms
 * @brief Delay in milliseconds.
 * @param delay Delay in milliseconds
 */
#define delay_ms(delay) \
    ((delay) ? cpu_delay_ms(delay, F_CPU) : cpu_delay_us(1, F_CPU))

/**
 * @def delay_us
 * @brief Delay in microseconds.
 * @param delay Delay in microseconds
 */
#define delay_us(delay) \
    ((delay) ? cpu_delay_us(delay, F_CPU) : cpu_delay_us(1, F_CPU))

#elif SAM

/**
 * @def delay_s
 * @brief Delay in seconds.
 * @param delay Delay in seconds
 */
#define delay_s(delay)  ((delay) ? cpu_delay_ms(1000 * delay) : cpu_delay_us(1))

/**
 * @def delay_ms
 * @brief Delay in milliseconds.
 * @param delay Delay in milliseconds
 */
#define delay_ms(delay) ((delay) ? cpu_delay_ms(delay) : cpu_delay_us(1))

/**
 * @def delay_us
 * @brief Delay in microseconds.
 * @param delay Delay in microseconds
 */
#define delay_us(delay) ((delay) ? cpu_delay_us(delay) : cpu_delay_us(1))

#endif

#ifdef __cplusplus
}
#endif

/**
 * @}
 */

#endif /* _DELAY_H_ */
