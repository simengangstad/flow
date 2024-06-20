/**
 * \file
 *
 * \brief ATmega256RFR2 Xplained Pro board header file.
 *
 * This file contains definitions and services related to the features of the
 * ATmega256RFR2 Xplained Pro board.
 *
 * To use this board, define BOARD= ATMEGA256RFR2_XPLAINED_PRO.
 *
 * Copyright (c) 2013-2018 Microchip Technology Inc. and its subsidiaries.
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
#ifndef _ATMEGA256RFR2_XPLAINED_PRO_
#define _ATMEGA256RFR2_XPLAINED_PRO_
#include "compiler.h"

#include "led.h"

#define MCU_SOC_NAME "ATMEGA256RFR2"
#define BOARD_NAME   "ATMEGA256RFR2-XPRO"

/*! \name GPIO Connections of LED
 * LED0 is connected to PORTB pin 4
 */
#define LED_ON_BOARD IOPORT_CREATE_PIN(PORTB, 4)
#define LED0_GPIO    LED_ON_BOARD
#define LED0         LED0_GPIO
#define LED_COUNT    1

/*!  \name GPIO Connections of Switch
 * Push button is connected to PORTE pin 4.
 */
#define GPIO_PUSH_BUTTON_ON_BOARD IOPORT_CREATE_PIN(PORTE, 4)
#define GPIO_PUSH_BUTTON_0        GPIO_PUSH_BUTTON_ON_BOARD

#define EXT1_SPI_MODULE &SPCR
#define EXT1_SPI_IRQn   SPI_STC_vect

// EXT1_PIN_1: Device ID
// EXT1_PIN_2: GND
#define EXT1_PIN_3  IOPORT_CREATE_PIN(PORTF, 0)
#define EXT1_PIN_4  IOPORT_CREATE_PIN(PORTF, 1)
#define EXT1_PIN_5  IOPORT_CREATE_PIN(PORTE, 2)
#define EXT1_PIN_6  IOPORT_CREATE_PIN(PORTE, 3)
#define EXT1_PIN_7  IOPORT_CREATE_PIN(PORTB, 5)
#define EXT1_PIN_8  IOPORT_CREATE_PIN(PORTB, 6)
#define EXT1_PIN_9  IOPORT_CREATE_PIN(PORTE, 5)
#define EXT1_PIN_10 IOPORT_CREATE_PIN(PORTD, 5)
#define EXT1_PIN_11 IOPORT_CREATE_PIN(PORTD, 1)
#define EXT1_PIN_12 IOPORT_CREATE_PIN(PORTD, 0)
#define EXT1_PIN_13 IOPORT_CREATE_PIN(PORTE, 0)
#define EXT1_PIN_14 IOPORT_CREATE_PIN(PORTE, 1)
#define EXT1_PIN_15 IOPORT_CREATE_PIN(PORTG, 0)
#define EXT1_PIN_16 IOPORT_CREATE_PIN(PORTB, 2)
#define EXT1_PIN_17 IOPORT_CREATE_PIN(PORTB, 3)
#define EXT1_PIN_18 IOPORT_CREATE_PIN(PORTB, 1)
// EXT1_PIN_19: GND
// EXT1_PIN_20: VCC

#define EXT1_SPI_CS   EXT1_PIN_15
#define EXT1_SPI_MOSI EXT1_PIN_16
#define EXT1_SPI_MISO EXT1_PIN_17
#define EXT1_SPI_SCK  EXT1_PIN_18

#define EXT1_IRQ_PIN IOPORT_CREATE_PIN(PORTE, 5)

#endif /* _ATMEGA256RFR2_XPLAINED_PRO_ */
