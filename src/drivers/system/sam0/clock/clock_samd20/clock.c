/**
 * \file
 *
 * \brief SAM D20 Clock Driver
 *
 * Copyright (c) 2012-2018 Microchip Technology Inc. and its subsidiaries.
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
#include <clock.h>
#include <conf_clocks.h>
#include <system.h>

/**
 * \internal
 * \brief DFLL-specific data container.
 */
struct _system_clock_dfll_config {
    uint32_t control;
    uint32_t val;
    uint32_t mul;
};

/**
 * \internal
 * \brief XOSC-specific data container.
 */
struct _system_clock_xosc_config {
    uint32_t frequency;
};

/**
 * \internal
 * \brief System clock module data container.
 */
struct _system_clock_module {
    volatile struct _system_clock_dfll_config dfll;
    volatile struct _system_clock_xosc_config xosc;
    volatile struct _system_clock_xosc_config xosc32k;
};

/**
 * \internal
 * \brief Internal module instance to cache configuration values.
 */
static struct _system_clock_module _system_clock_inst = {
    .dfll =
        {
            .control = 0,
            .val     = 0,
            .mul     = 0,
        },
    .xosc =
        {
            .frequency = 0,
        },
    .xosc32k =
        {
            .frequency = 0,
        },
};

/**
 * \internal
 * \brief Wait for sync to the DFLL control registers.
 */
static inline void _system_dfll_wait_for_sync(void) {
    while (!(SYSCTRL->PCLKSR.reg & SYSCTRL_PCLKSR_DFLLRDY)) {
        /* Wait for DFLL sync */
    }
}

/**
 * \internal
 * \brief Wait for sync to the OSC32K control registers.
 */
static inline void _system_osc32k_wait_for_sync(void) {
    while (!(SYSCTRL->PCLKSR.reg & SYSCTRL_PCLKSR_OSC32KRDY)) {
        /* Wait for OSC32K sync */
    }
}

static inline void _system_clock_source_dfll_set_config_errata_9905(void) {

    /* Disable ONDEMAND mode while writing configurations */
    SYSCTRL->DFLLCTRL.reg = _system_clock_inst.dfll.control &
                            ~SYSCTRL_DFLLCTRL_ONDEMAND;
    _system_dfll_wait_for_sync();

    SYSCTRL->DFLLMUL.reg = _system_clock_inst.dfll.mul;
    SYSCTRL->DFLLVAL.reg = _system_clock_inst.dfll.val;

    /* Write full configuration to DFLL control register */
    SYSCTRL->DFLLCTRL.reg = _system_clock_inst.dfll.control;
}

/**
 * \brief Retrieve the frequency of a clock source.
 *
 * Determines the current operating frequency of a given clock source.
 *
 * \param[in] clock_source  Clock source to get the frequency
 *
 * \returns Frequency of the given clock source, in Hz.
 */
uint32_t
system_clock_source_get_hz(const enum system_clock_source clock_source) {
    switch (clock_source) {
    case SYSTEM_CLOCK_SOURCE_XOSC:
        return _system_clock_inst.xosc.frequency;

    case SYSTEM_CLOCK_SOURCE_OSC8M:
        return 8000000UL >> SYSCTRL->OSC8M.bit.PRESC;

    case SYSTEM_CLOCK_SOURCE_OSC32K:
        return 32768UL;

    case SYSTEM_CLOCK_SOURCE_ULP32K:
        return 32768UL;

    case SYSTEM_CLOCK_SOURCE_XOSC32K:
        return _system_clock_inst.xosc32k.frequency;

    case SYSTEM_CLOCK_SOURCE_DFLL:

        /* Check if the DFLL has been configured */
        if (!(_system_clock_inst.dfll.control & SYSCTRL_DFLLCTRL_ENABLE))
            return 0;

        /* Make sure that the DFLL module is ready */
        _system_dfll_wait_for_sync();

        /* Check if operating in closed loop mode */
        if (_system_clock_inst.dfll.control & SYSCTRL_DFLLCTRL_MODE) {
            return system_gclk_chan_get_hz(SYSCTRL_GCLK_ID_DFLL48) *
                   (_system_clock_inst.dfll.mul & 0xffff);
        }

        return 48000000UL;

    default:
        return 0;
    }
}

/**
 * \brief Configure the internal OSC8M oscillator clock source.
 *
 * Configures the 8MHz (nominal) internal RC oscillator with the given
 * configuration settings.
 *
 * \param[in] config  OSC8M configuration structure containing the new config
 */
void system_clock_source_osc8m_set_config(
    struct system_clock_source_osc8m_config* const config) {
    SYSCTRL_OSC8M_Type temp = SYSCTRL->OSC8M;

    /* Use temporary struct to reduce register access */
    temp.bit.PRESC    = config->prescaler;
    temp.bit.ONDEMAND = config->on_demand;
    temp.bit.RUNSTDBY = config->run_in_standby;

    SYSCTRL->OSC8M = temp;
}

/**
 * \brief Configure the internal OSC32K oscillator clock source.
 *
 * Configures the 32KHz (nominal) internal RC oscillator with the given
 * configuration settings.
 *
 * \param[in] config  OSC32K configuration structure containing the new config
 */
void system_clock_source_osc32k_set_config(
    struct system_clock_source_osc32k_config* const config) {
    SYSCTRL_OSC32K_Type temp = SYSCTRL->OSC32K;

    /* Update settings via a temporary struct to reduce register access */
    temp.bit.EN1K     = config->enable_1khz_output;
    temp.bit.EN32K    = config->enable_32khz_output;
    temp.bit.STARTUP  = config->startup_time;
    temp.bit.ONDEMAND = config->on_demand;
    temp.bit.RUNSTDBY = config->run_in_standby;
    temp.bit.WRTLOCK  = config->write_once;

    SYSCTRL->OSC32K = temp;
}

/**
 * \brief Configure the external oscillator clock source.
 *
 * Configures the external oscillator clock source with the given configuration
 * settings.
 *
 * \param[in] config  External oscillator configuration structure containing
 *                    the new config
 */
void system_clock_source_xosc_set_config(
    struct system_clock_source_xosc_config* const config) {
    SYSCTRL_XOSC_Type temp = SYSCTRL->XOSC;

    temp.bit.STARTUP = config->startup_time;

    if (config->external_clock == SYSTEM_CLOCK_EXTERNAL_CRYSTAL) {
        temp.bit.XTALEN = 1;
    } else {
        temp.bit.XTALEN = 0;
    }

    temp.bit.AMPGC = config->auto_gain_control;

    /* Set gain */
    if (config->frequency <= 2000000) {
        temp.bit.GAIN = 0;
    } else if (config->frequency <= 4000000) {
        temp.bit.GAIN = 1;
    } else if (config->frequency <= 8000000) {
        temp.bit.GAIN = 2;
    } else if (config->frequency <= 16000000) {
        temp.bit.GAIN = 3;
    } else if (config->frequency <= 32000000) {
        temp.bit.GAIN = 4;
    }

    temp.bit.ONDEMAND = config->on_demand;
    temp.bit.RUNSTDBY = config->run_in_standby;

    /* Store XOSC frequency for internal use */
    _system_clock_inst.xosc.frequency = config->frequency;

    SYSCTRL->XOSC = temp;
}

/**
 * \brief Configure the XOSC32K external 32KHz oscillator clock source.
 *
 * Configures the external 32KHz oscillator clock source with the given
 * configuration settings.
 *
 * \param[in] config  XOSC32K configuration structure containing the new config
 */
void system_clock_source_xosc32k_set_config(
    struct system_clock_source_xosc32k_config* const config) {
    SYSCTRL_XOSC32K_Type temp = SYSCTRL->XOSC32K;

    temp.bit.STARTUP = config->startup_time;

    if (config->external_clock == SYSTEM_CLOCK_EXTERNAL_CRYSTAL) {
        temp.bit.XTALEN = 1;
    } else {
        temp.bit.XTALEN = 0;
    }

    temp.bit.AAMPEN = config->auto_gain_control;
    temp.bit.EN1K   = config->enable_1khz_output;
    temp.bit.EN32K  = config->enable_32khz_output;

    temp.bit.ONDEMAND = config->on_demand;
    temp.bit.RUNSTDBY = config->run_in_standby;
    temp.bit.WRTLOCK  = config->write_once;

    /* Cache the new frequency in case the user needs to check the current
     * operating frequency later */
    _system_clock_inst.xosc32k.frequency = config->frequency;

    SYSCTRL->XOSC32K = temp;
}
///@cond INTERNAL

/**
 * \internal
 * \name Header file macro copies for runtime support of different revisions
 *
 * These macroes are copied from the header file to be able to support both new
 * and old register layout runtime.
 *
 * @{
 */
#define _SYSTEM_OLD_DFLLVAL_FINE_Pos 0
#define _SYSTEM_OLD_DFLLVAL_FINE_Msk (0xFFu << _SYSTEM_OLD_DFLLVAL_FINE_Pos)
#define _SYSTEM_OLD_DFLLVAL_FINE(value) \
    ((_SYSTEM_OLD_DFLLVAL_FINE_Msk & ((value) << _SYSTEM_OLD_DFLLVAL_FINE_Pos)))

#define _SYSTEM_OLD_DFLLVAL_COARSE_Pos 8
#define _SYSTEM_OLD_DFLLVAL_COARSE_Msk (0x1Fu << _SYSTEM_OLD_DFLLVAL_COARSE_Pos)
#define _SYSTEM_OLD_DFLLVAL_COARSE(value) \
    ((_SYSTEM_OLD_DFLLVAL_COARSE_Msk &    \
      ((value) << _SYSTEM_OLD_DFLLVAL_COARSE_Pos)))

#define _SYSTEM_NEW_DFLLVAL_FINE_Pos 0
#define _SYSTEM_NEW_DFLLVAL_FINE_Msk (0x3FFu << _SYSTEM_NEW_DFLLVAL_FINE_Pos)
#define _SYSTEM_NEW_DFLLVAL_FINE(value) \
    ((_SYSTEM_NEW_DFLLVAL_FINE_Msk & ((value) << _SYSTEM_NEW_DFLLVAL_FINE_Pos)))

#define _SYSTEM_NEW_DFLLVAL_COARSE_Pos 10
#define _SYSTEM_NEW_DFLLVAL_COARSE_Msk (0x3Fu << _SYSTEM_NEW_DFLLVAL_COARSE_Pos)
#define _SYSTEM_NEW_DFLLVAL_COARSE(value) \
    ((_SYSTEM_NEW_DFLLVAL_COARSE_Msk &    \
      ((value) << _SYSTEM_NEW_DFLLVAL_COARSE_Pos)))

#define _SYSTEM_OLD_DFLLMUL_FSTEP_Pos 16
#define _SYSTEM_OLD_DFLLMUL_FSTEP_Msk (0xFFu << _SYSTEM_OLD_DFLLMUL_FSTEP_Pos)
#define _SYSTEM_OLD_DFLLMUL_FSTEP(value) \
    ((_SYSTEM_OLD_DFLLMUL_FSTEP_Msk &    \
      ((value) << _SYSTEM_OLD_DFLLMUL_FSTEP_Pos)))

#define _SYSTEM_OLD_DFLLMUL_CSTEP_Pos 24
#define _SYSTEM_OLD_DFLLMUL_CSTEP_Msk (0x1Fu << _SYSTEM_OLD_DFLLMUL_CSTEP_Pos)
#define _SYSTEM_OLD_DFLLMUL_CSTEP(value) \
    ((_SYSTEM_OLD_DFLLMUL_CSTEP_Msk &    \
      ((value) << _SYSTEM_OLD_DFLLMUL_CSTEP_Pos)))

#define _SYSTEM_NEW_DFLLMUL_FSTEP_Pos 16
#define _SYSTEM_NEW_DFLLMUL_FSTEP_Msk (0x3FFu << _SYSTEM_NEW_DFLLMUL_FSTEP_Pos)
#define _SYSTEM_NEW_DFLLMUL_FSTEP(value) \
    ((_SYSTEM_NEW_DFLLMUL_FSTEP_Msk &    \
      ((value) << _SYSTEM_NEW_DFLLMUL_FSTEP_Pos)))

#define _SYSTEM_NEW_DFLLMUL_CSTEP_Pos 26
#define _SYSTEM_NEW_DFLLMUL_CSTEP_Msk (0x3Fu << _SYSTEM_NEW_DFLLMUL_CSTEP_Pos)
#define _SYSTEM_NEW_DFLLMUL_CSTEP(value) \
    ((_SYSTEM_NEW_DFLLMUL_CSTEP_Msk &    \
      ((value) << _SYSTEM_NEW_DFLLMUL_CSTEP_Pos)))

#define _SYSTEM_MCU_REVISION_D 3

///@endcond

/**
 * \brief Configure the DFLL clock source.
 *
 * Configures the Digital Frequency Locked Loop clock source with the given
 * configuration settings.
 *
 * \note The DFLL will be running when this function returns, as the DFLL module
 *       needs to be enabled in order to perform the module configuration.
 *
 * \param[in] config  DFLL configuration structure containing the new config
 */
void system_clock_source_dfll_set_config(
    struct system_clock_source_dfll_config* const config) {

    /* Get MCU revision */
    uint32_t rev = system_get_device_id();

    rev &= DSU_DID_REVISION_Msk;
    rev = rev >> DSU_DID_REVISION_Pos;

    if (rev < _SYSTEM_MCU_REVISION_D) {
        _system_clock_inst.dfll.val =
            _SYSTEM_OLD_DFLLVAL_COARSE(config->coarse_value) |
            _SYSTEM_OLD_DFLLVAL_FINE(config->fine_value);
    } else {
        _system_clock_inst.dfll.val =
            _SYSTEM_NEW_DFLLVAL_COARSE(config->coarse_value) |
            _SYSTEM_NEW_DFLLVAL_FINE(config->fine_value);
    }

    _system_clock_inst.dfll.control =
        (uint32_t)config->wakeup_lock | (uint32_t)config->stable_tracking |
        (uint32_t)config->quick_lock | (uint32_t)config->chill_cycle |
        ((uint32_t)config->on_demand << SYSCTRL_DFLLCTRL_ONDEMAND_Pos);

    if (config->loop_mode == SYSTEM_CLOCK_DFLL_LOOP_MODE_CLOSED) {

        if (rev < _SYSTEM_MCU_REVISION_D) {
            _system_clock_inst.dfll.mul =
                _SYSTEM_OLD_DFLLMUL_CSTEP(config->coarse_max_step) |
                _SYSTEM_OLD_DFLLMUL_FSTEP(config->fine_max_step) |
                SYSCTRL_DFLLMUL_MUL(config->multiply_factor);
        } else {
            _system_clock_inst.dfll.mul =
                _SYSTEM_NEW_DFLLMUL_CSTEP(config->coarse_max_step) |
                _SYSTEM_NEW_DFLLMUL_FSTEP(config->fine_max_step) |
                SYSCTRL_DFLLMUL_MUL(config->multiply_factor);
        }

        /* Enable the closed loop mode */
        _system_clock_inst.dfll.control |= config->loop_mode;
    }
}

/**
 * \brief Writes the calibration values for a given oscillator clock source.
 *
 * Writes an oscillator calibration value to the given oscillator control
 * registers. The acceptable ranges are:
 *
 * For OSC32K:
 *  - 7 bits (max value 128)
 * For OSC8MHZ:
 *  - 8 bits (Max value 255)
 * For OSCULP:
 *  - 5 bits (Max value 32)
 *
 * \note The frequency range parameter applies only when configuring the 8MHz
 *       oscillator and will be ignored for the other oscillators.
 *
 * \param[in] clock_source       Clock source to calibrate
 * \param[in] calibration_value  Calibration value to write
 * \param[in] freq_range         Frequency range (8MHz oscillator only)
 *
 * \retval STATUS_OK               The calibration value was written
 *                                 successfully.
 * \retval STATUS_ERR_INVALID_ARG  The setting is not valid for selected clock
 *                                 source.
 */
enum status_code system_clock_source_write_calibration(
    const enum system_clock_source clock_source,
    const uint16_t calibration_value,
    const uint8_t freq_range) {
    switch (clock_source) {
    case SYSTEM_CLOCK_SOURCE_OSC8M:

        if (calibration_value > 0xfff || freq_range > 4) {
            return STATUS_ERR_INVALID_ARG;
        }

        SYSCTRL->OSC8M.bit.CALIB  = calibration_value;
        SYSCTRL->OSC8M.bit.FRANGE = freq_range;
        break;

    case SYSTEM_CLOCK_SOURCE_OSC32K:

        if (calibration_value > 128) {
            return STATUS_ERR_INVALID_ARG;
        }

        _system_osc32k_wait_for_sync();
        SYSCTRL->OSC32K.bit.CALIB = calibration_value;
        break;

    case SYSTEM_CLOCK_SOURCE_ULP32K:

        if (calibration_value > 32) {
            return STATUS_ERR_INVALID_ARG;
        }

        SYSCTRL->OSCULP32K.bit.CALIB = calibration_value;
        break;

    default:
        Assert(false);
        return STATUS_ERR_INVALID_ARG;
        break;
    }

    return STATUS_OK;
}

/**
 * \brief Enables a clock source.
 *
 * Enables a clock source which has been previously configured.
 *
 * \param[in] clock_source       Clock source to enable
 *
 * \retval STATUS_OK               Clock source was enabled successfully and
 *                                 is ready
 * \retval STATUS_ERR_INVALID_ARG  The clock source is not available on this
 *                                 device
 */
enum status_code
system_clock_source_enable(const enum system_clock_source clock_source) {
    switch (clock_source) {
    case SYSTEM_CLOCK_SOURCE_OSC8M:
        SYSCTRL->OSC8M.reg |= SYSCTRL_OSC8M_ENABLE;
        return STATUS_OK;

    case SYSTEM_CLOCK_SOURCE_OSC32K:
        SYSCTRL->OSC32K.reg |= SYSCTRL_OSC32K_ENABLE;
        break;

    case SYSTEM_CLOCK_SOURCE_XOSC:
        SYSCTRL->XOSC.reg |= SYSCTRL_XOSC_ENABLE;
        break;

    case SYSTEM_CLOCK_SOURCE_XOSC32K:
        SYSCTRL->XOSC32K.reg |= SYSCTRL_XOSC32K_ENABLE;
        break;

    case SYSTEM_CLOCK_SOURCE_DFLL:
        _system_clock_inst.dfll.control |= SYSCTRL_DFLLCTRL_ENABLE;
        _system_clock_source_dfll_set_config_errata_9905();
        break;

    case SYSTEM_CLOCK_SOURCE_ULP32K:
        /* Always enabled */
        return STATUS_OK;

    default:
        Assert(false);
        return STATUS_ERR_INVALID_ARG;
    }

    return STATUS_OK;
}

/**
 * \brief Disables a clock source.
 *
 * Disables a clock source that was previously enabled.
 *
 * \param[in] clock_source  Clock source to disable
 *
 * \retval STATUS_OK               Clock source was disabled successfully
 * \retval STATUS_ERR_INVALID_ARG  An invalid or unavailable clock source was
 *                                 given
 */
enum status_code
system_clock_source_disable(const enum system_clock_source clock_source) {
    switch (clock_source) {
    case SYSTEM_CLOCK_SOURCE_OSC8M:
        SYSCTRL->OSC8M.reg &= ~SYSCTRL_OSC8M_ENABLE;
        break;

    case SYSTEM_CLOCK_SOURCE_OSC32K:
        SYSCTRL->OSC32K.reg &= ~SYSCTRL_OSC32K_ENABLE;
        break;

    case SYSTEM_CLOCK_SOURCE_XOSC:
        SYSCTRL->XOSC.reg &= ~SYSCTRL_XOSC_ENABLE;
        break;

    case SYSTEM_CLOCK_SOURCE_XOSC32K:
        SYSCTRL->XOSC32K.reg &= ~SYSCTRL_XOSC32K_ENABLE;
        break;

    case SYSTEM_CLOCK_SOURCE_DFLL:
        _system_clock_inst.dfll.control &= ~SYSCTRL_DFLLCTRL_ENABLE;
        SYSCTRL->DFLLCTRL.reg = _system_clock_inst.dfll.control;
        break;

    case SYSTEM_CLOCK_SOURCE_ULP32K:
        /* Not possible to disable */

    default:
        Assert(false);
        return STATUS_ERR_INVALID_ARG;
    }

    return STATUS_OK;
}

/**
 * \brief Checks if a clock source is ready.
 *
 * Checks if a given clock source is ready to be used.
 *
 * \param[in] clock_source  Clock source to check if ready
 *
 * \returns Ready state of the given clock source.
 *
 * \retval true   Clock source is enabled and ready
 * \retval false  Clock source is disabled or not yet ready
 */
bool system_clock_source_is_ready(const enum system_clock_source clock_source) {
    uint32_t mask = 0;

    switch (clock_source) {
    case SYSTEM_CLOCK_SOURCE_OSC8M:
        mask = SYSCTRL_PCLKSR_OSC8MRDY;
        break;

    case SYSTEM_CLOCK_SOURCE_OSC32K:
        mask = SYSCTRL_PCLKSR_OSC32KRDY;
        break;

    case SYSTEM_CLOCK_SOURCE_XOSC:
        mask = SYSCTRL_PCLKSR_XOSCRDY;
        break;

    case SYSTEM_CLOCK_SOURCE_XOSC32K:
        mask = SYSCTRL_PCLKSR_XOSC32KRDY;
        break;

    case SYSTEM_CLOCK_SOURCE_DFLL:
        if (CONF_CLOCK_DFLL_LOOP_MODE == SYSTEM_CLOCK_DFLL_LOOP_MODE_CLOSED) {
            mask = (SYSCTRL_PCLKSR_DFLLRDY | SYSCTRL_PCLKSR_DFLLLCKF |
                    SYSCTRL_PCLKSR_DFLLLCKC);
        } else {
            mask = SYSCTRL_PCLKSR_DFLLRDY;
        }
        break;

    case SYSTEM_CLOCK_SOURCE_ULP32K:
        /* Not possible to disable */
        return true;

    default:
        return false;
    }

    return ((SYSCTRL->PCLKSR.reg & mask) == mask);
}

/* Include some checks for conf_clocks.h validation */
#include "clock_config_check.h"

#if !defined(__DOXYGEN__)
/** \internal
 *
 * Configures a Generic Clock Generator with the configuration from \c
 * conf_clocks.h.
 */
#define _CONF_CLOCK_GCLK_CONFIG(n, unused)                                \
    if (CONF_CLOCK_GCLK_##n##_ENABLE == true) {                           \
        struct system_gclk_gen_config gclk_conf;                          \
        system_gclk_gen_get_config_defaults(&gclk_conf);                  \
        gclk_conf.source_clock    = CONF_CLOCK_GCLK_##n##_CLOCK_SOURCE;   \
        gclk_conf.division_factor = CONF_CLOCK_GCLK_##n##_PRESCALER;      \
        gclk_conf.run_in_standby  = CONF_CLOCK_GCLK_##n##_RUN_IN_STANDBY; \
        gclk_conf.output_enable   = CONF_CLOCK_GCLK_##n##_OUTPUT_ENABLE;  \
        system_gclk_gen_set_config(GCLK_GENERATOR_##n, &gclk_conf);       \
        system_gclk_gen_enable(GCLK_GENERATOR_##n);                       \
    }

/** \internal
 *
 * Configures a Generic Clock Generator with the configuration from \c
 * conf_clocks.h, provided that it is not the main Generic Clock Generator
 * channel.
 */
#define _CONF_CLOCK_GCLK_CONFIG_NONMAIN(n, unused) \
    if (n > 0) {                                   \
        _CONF_CLOCK_GCLK_CONFIG(n, unused);        \
    }
#endif

/** \internal
 *
 * Switch all peripheral clock to a not enabled general clock
 * to save power.
 */
static void _switch_peripheral_gclk(void) {
    uint32_t gclk_id;
    struct system_gclk_chan_config gclk_conf;

#if CONF_CLOCK_GCLK_1_ENABLE == false
    gclk_conf.source_generator = GCLK_GENERATOR_1;
#elif CONF_CLOCK_GCLK_2_ENABLE == false
    gclk_conf.source_generator = GCLK_GENERATOR_2;
#elif CONF_CLOCK_GCLK_3_ENABLE == false
    gclk_conf.source_generator = GCLK_GENERATOR_3;
#elif CONF_CLOCK_GCLK_4_ENABLE == false
    gclk_conf.source_generator = GCLK_GENERATOR_4;
#elif CONF_CLOCK_GCLK_5_ENABLE == false
    gclk_conf.source_generator = GCLK_GENERATOR_5;
#elif CONF_CLOCK_GCLK_6_ENABLE == false
    gclk_conf.source_generator = GCLK_GENERATOR_6;
#elif CONF_CLOCK_GCLK_7_ENABLE == false
    gclk_conf.source_generator = GCLK_GENERATOR_7;
#else
    gclk_conf.source_generator = GCLK_GENERATOR_7;
#endif

    for (gclk_id = 0; gclk_id < GCLK_NUM; gclk_id++) {
        system_gclk_chan_set_config(gclk_id, &gclk_conf);
    }
}

/**
 * \brief Initialize clock system based on the configuration in conf_clocks.h.
 *
 * This function will apply the settings in conf_clocks.h when run from the user
 * application. All clock sources and GCLK generators are running when this
 * function returns.
 *
 * \note OSC8M is always enabled and if user selects other clocks for GCLK
 * generators, the OSC8M default enable can be disabled after system_clock_init.
 * Make sure the clock switch successfully before disabling OSC8M.
 */
void system_clock_init(void) {
    /* Various bits in the INTFLAG register can be set to one at startup.
       This will ensure that these bits are cleared */
    SYSCTRL->INTFLAG.reg = SYSCTRL_INTFLAG_BOD33RDY | SYSCTRL_INTFLAG_BOD33DET |
                           SYSCTRL_INTFLAG_DFLLRDY;

    system_flash_set_waitstates(CONF_CLOCK_FLASH_WAIT_STATES);

    /* Switch all peripheral clock to a not enabled general clock to save power.
     */
    _switch_peripheral_gclk();

    /* XOSC */
#if CONF_CLOCK_XOSC_ENABLE == true
    struct system_clock_source_xosc_config xosc_conf;
    system_clock_source_xosc_get_config_defaults(&xosc_conf);

    xosc_conf.external_clock = CONF_CLOCK_XOSC_EXTERNAL_CRYSTAL;
    xosc_conf.startup_time   = CONF_CLOCK_XOSC_STARTUP_TIME;
    xosc_conf.frequency      = CONF_CLOCK_XOSC_EXTERNAL_FREQUENCY;
    xosc_conf.run_in_standby = CONF_CLOCK_XOSC_RUN_IN_STANDBY;

    system_clock_source_xosc_set_config(&xosc_conf);
    system_clock_source_enable(SYSTEM_CLOCK_SOURCE_XOSC);
    while (!system_clock_source_is_ready(SYSTEM_CLOCK_SOURCE_XOSC))
        ;
    if (CONF_CLOCK_XOSC_ON_DEMAND || CONF_CLOCK_XOSC_AUTO_GAIN_CONTROL) {
        SYSCTRL->XOSC.reg |=
            (CONF_CLOCK_XOSC_ON_DEMAND << SYSCTRL_XOSC_ONDEMAND_Pos) |
            (CONF_CLOCK_XOSC_AUTO_GAIN_CONTROL << SYSCTRL_XOSC_AMPGC_Pos);
    }
#endif

    /* XOSC32K */
#if CONF_CLOCK_XOSC32K_ENABLE == true
    struct system_clock_source_xosc32k_config xosc32k_conf;
    system_clock_source_xosc32k_get_config_defaults(&xosc32k_conf);

    xosc32k_conf.frequency          = 32768UL;
    xosc32k_conf.external_clock     = CONF_CLOCK_XOSC32K_EXTERNAL_CRYSTAL;
    xosc32k_conf.startup_time       = CONF_CLOCK_XOSC32K_STARTUP_TIME;
    xosc32k_conf.auto_gain_control  = CONF_CLOCK_XOSC32K_AUTO_AMPLITUDE_CONTROL;
    xosc32k_conf.enable_1khz_output = CONF_CLOCK_XOSC32K_ENABLE_1KHZ_OUPUT;
    xosc32k_conf.enable_32khz_output = CONF_CLOCK_XOSC32K_ENABLE_32KHZ_OUTPUT;
    xosc32k_conf.on_demand           = false;
    xosc32k_conf.run_in_standby      = CONF_CLOCK_XOSC32K_RUN_IN_STANDBY;

    system_clock_source_xosc32k_set_config(&xosc32k_conf);
    system_clock_source_enable(SYSTEM_CLOCK_SOURCE_XOSC32K);
    while (!system_clock_source_is_ready(SYSTEM_CLOCK_SOURCE_XOSC32K))
        ;
    if (CONF_CLOCK_XOSC32K_ON_DEMAND) {
        SYSCTRL->XOSC32K.bit.ONDEMAND = 1;
    }
#endif

    /* OSCK32K */
#if CONF_CLOCK_OSC32K_ENABLE == true
    SYSCTRL->OSC32K.bit.CALIB = (*(uint32_t*)FUSES_OSC32KCAL_ADDR >>
                                 FUSES_OSC32KCAL_Pos);

    struct system_clock_source_osc32k_config osc32k_conf;
    system_clock_source_osc32k_get_config_defaults(&osc32k_conf);

    osc32k_conf.startup_time        = CONF_CLOCK_OSC32K_STARTUP_TIME;
    osc32k_conf.enable_1khz_output  = CONF_CLOCK_OSC32K_ENABLE_1KHZ_OUTPUT;
    osc32k_conf.enable_32khz_output = CONF_CLOCK_OSC32K_ENABLE_32KHZ_OUTPUT;
    osc32k_conf.on_demand           = CONF_CLOCK_OSC32K_ON_DEMAND;
    osc32k_conf.run_in_standby      = CONF_CLOCK_OSC32K_RUN_IN_STANDBY;

    system_clock_source_osc32k_set_config(&osc32k_conf);
    system_clock_source_enable(SYSTEM_CLOCK_SOURCE_OSC32K);
#endif

    /* DFLL Config (Open and Closed Loop) */
#if CONF_CLOCK_DFLL_ENABLE == true
    struct system_clock_source_dfll_config dfll_conf;
    system_clock_source_dfll_get_config_defaults(&dfll_conf);

    dfll_conf.loop_mode = CONF_CLOCK_DFLL_LOOP_MODE;
    dfll_conf.on_demand = false;

    /* Using DFLL48M COARSE CAL value from NVM Software Calibration Area Mapping
 in DFLL.COARSE helps to output a frequency close to 48 MHz.
       Not applicable for silicon rev C and previous*/

    /* Get MCU revision */
    uint32_t rev = system_get_device_id();

    rev &= DSU_DID_REVISION_Msk;
    rev = rev >> DSU_DID_REVISION_Pos;

    if (rev >= _SYSTEM_MCU_REVISION_D) {
#define NVM_DFLL_COARSE_POS \
    58 /* DFLL48M Coarse calibration value bit position.*/
#define NVM_DFLL_COARSE_SIZE 6 /* DFLL48M Coarse calibration value bit size.*/

        uint32_t coarse =
            (*((uint32_t*)(NVMCTRL_OTP4) + (NVM_DFLL_COARSE_POS / 32)) >>
             (NVM_DFLL_COARSE_POS % 32)) &
            ((1 << NVM_DFLL_COARSE_SIZE) - 1);
        /* In some revision chip, the coarse calibration value is not correct.
         */
        if (coarse == 0x3f) {
            coarse = 0x1f;
        }
        dfll_conf.coarse_value = coarse;
    }

    if (CONF_CLOCK_DFLL_LOOP_MODE == SYSTEM_CLOCK_DFLL_LOOP_MODE_OPEN) {
        dfll_conf.fine_value = CONF_CLOCK_DFLL_FINE_VALUE;
    }

#if CONF_CLOCK_DFLL_QUICK_LOCK == true
    dfll_conf.quick_lock = SYSTEM_CLOCK_DFLL_QUICK_LOCK_ENABLE;
#else
    dfll_conf.quick_lock = SYSTEM_CLOCK_DFLL_QUICK_LOCK_DISABLE;
#endif

#if CONF_CLOCK_DFLL_TRACK_AFTER_FINE_LOCK == true
    dfll_conf.stable_tracking =
        SYSTEM_CLOCK_DFLL_STABLE_TRACKING_TRACK_AFTER_LOCK;
#else
    dfll_conf.stable_tracking =
        SYSTEM_CLOCK_DFLL_STABLE_TRACKING_FIX_AFTER_LOCK;
#endif

#if CONF_CLOCK_DFLL_KEEP_LOCK_ON_WAKEUP == true
    dfll_conf.wakeup_lock = SYSTEM_CLOCK_DFLL_WAKEUP_LOCK_KEEP;
#else
    dfll_conf.wakeup_lock = SYSTEM_CLOCK_DFLL_WAKEUP_LOCK_LOSE;
#endif

#if CONF_CLOCK_DFLL_ENABLE_CHILL_CYCLE == true
    dfll_conf.chill_cycle = SYSTEM_CLOCK_DFLL_CHILL_CYCLE_ENABLE;
#else
    dfll_conf.chill_cycle = SYSTEM_CLOCK_DFLL_CHILL_CYCLE_DISABLE;
#endif

    if (CONF_CLOCK_DFLL_LOOP_MODE == SYSTEM_CLOCK_DFLL_LOOP_MODE_CLOSED) {
        dfll_conf.multiply_factor = CONF_CLOCK_DFLL_MULTIPLY_FACTOR;
    }

    dfll_conf.coarse_max_step = CONF_CLOCK_DFLL_MAX_COARSE_STEP_SIZE;
    dfll_conf.fine_max_step   = CONF_CLOCK_DFLL_MAX_FINE_STEP_SIZE;

    system_clock_source_dfll_set_config(&dfll_conf);
#endif

    /* OSC8M */
    struct system_clock_source_osc8m_config osc8m_conf;
    system_clock_source_osc8m_get_config_defaults(&osc8m_conf);

    osc8m_conf.prescaler      = CONF_CLOCK_OSC8M_PRESCALER;
    osc8m_conf.on_demand      = CONF_CLOCK_OSC8M_ON_DEMAND;
    osc8m_conf.run_in_standby = CONF_CLOCK_OSC8M_RUN_IN_STANDBY;

    system_clock_source_osc8m_set_config(&osc8m_conf);
    system_clock_source_enable(SYSTEM_CLOCK_SOURCE_OSC8M);

    /* GCLK */
#if CONF_CLOCK_CONFIGURE_GCLK == true
    system_gclk_init();

    /* Configure all GCLK generators except for the main generator, which
     * is configured later after all other clock systems are set up */
    MREPEAT(GCLK_GEN_NUM, _CONF_CLOCK_GCLK_CONFIG_NONMAIN, ~);

#if CONF_CLOCK_DFLL_ENABLE == true
    /* Enable DFLL reference clock if in closed loop mode */
    if (CONF_CLOCK_DFLL_LOOP_MODE == SYSTEM_CLOCK_DFLL_LOOP_MODE_CLOSED) {
        struct system_gclk_chan_config dfll_gclk_chan_conf;

        system_gclk_chan_get_config_defaults(&dfll_gclk_chan_conf);
        dfll_gclk_chan_conf.source_generator =
            CONF_CLOCK_DFLL_SOURCE_GCLK_GENERATOR;
        system_gclk_chan_set_config(SYSCTRL_GCLK_ID_DFLL48,
                                    &dfll_gclk_chan_conf);
        system_gclk_chan_enable(SYSCTRL_GCLK_ID_DFLL48);
    }
#endif
#endif

    /* DFLL Enable (Open and Closed Loop) */
#if CONF_CLOCK_DFLL_ENABLE == true
    system_clock_source_enable(SYSTEM_CLOCK_SOURCE_DFLL);
    while (!system_clock_source_is_ready(SYSTEM_CLOCK_SOURCE_DFLL))
        ;
    if (CONF_CLOCK_DFLL_ON_DEMAND) {
        SYSCTRL->DFLLCTRL.bit.ONDEMAND = 1;
    }
#endif

    /* CPU and BUS clocks */
    system_cpu_clock_set_divider(CONF_CLOCK_CPU_DIVIDER);

    system_apb_clock_set_divider(SYSTEM_CLOCK_APB_APBA,
                                 CONF_CLOCK_APBA_DIVIDER);
    system_apb_clock_set_divider(SYSTEM_CLOCK_APB_APBB,
                                 CONF_CLOCK_APBB_DIVIDER);
    system_apb_clock_set_divider(SYSTEM_CLOCK_APB_APBC,
                                 CONF_CLOCK_APBC_DIVIDER);

    /* GCLK 0 */
#if CONF_CLOCK_CONFIGURE_GCLK == true
    /* Configure the main GCLK last as it might depend on other generators */
    _CONF_CLOCK_GCLK_CONFIG(0, ~);
#endif
}
