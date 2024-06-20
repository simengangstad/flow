#ifndef _CONF_USB_H_
#define _CONF_USB_H_

#include <compiler.h>

#define USB_DEVICE_VENDOR_ID        USB_VID_ATMEL
#define USB_DEVICE_PRODUCT_ID       USB_PID_ATMEL_ASF_CDC
#define USB_DEVICE_MAJOR_VERSION    1
#define USB_DEVICE_MINOR_VERSION    0
#define USB_DEVICE_POWER            100
#define USB_DEVICE_MANUFACTURE_NAME "ATMEL ASF"
#define USB_DEVICE_PRODUCT_NAME     "CDC"

#define USB_DEVICE_ATTR (USB_CONFIG_ATTR_SELF_POWERED)
/* (USB_CONFIG_ATTR_BUS_POWERED) */
/*	(USB_CONFIG_ATTR_REMOTE_WAKEUP|USB_CONFIG_ATTR_SELF_POWERED) */
/*	(USB_CONFIG_ATTR_REMOTE_WAKEUP|USB_CONFIG_ATTR_BUS_POWERED) */

#define USB_ENABLE

/**
 * USB Device Callbacks definitions
 * @{
 */
#define UDC_VBUS_EVENT(b_vbus_high)
#define UDC_SOF_EVENT()
#define UDC_SUSPEND_EVENT()
#define UDC_RESUME_EVENT()

/**
 * USB Interface Configuration
 */

/**
 * Configuration of CDC interface
 */

/* ! Interface callback definition */
#define UDI_CDC_ENABLE_EXT(port)  main_cdc_enable(port)
#define UDI_CDC_DISABLE_EXT(port) main_cdc_disable(port)
#define UDI_CDC_RX_NOTIFY(port)   usb_rx_notify()
#define UDI_CDC_SET_CODING_EXT(port, cfg)
#define UDI_CDC_SET_DTR_EXT(port, set) dtr_cb(set)
#define UDI_CDC_SET_RTS_EXT(port, set)

bool main_cdc_enable(uint8_t port);

/*! \brief Called by CDC interface
 * Callback running when USB Host disable cdc interface
 */
void main_cdc_disable(uint8_t port);

/* ! Default configuration of communication port */
#define UDI_CDC_DEFAULT_RATE     115200
#define UDI_CDC_DEFAULT_STOPBITS CDC_STOP_BITS_1
#define UDI_CDC_DEFAULT_PARITY   CDC_PAR_NONE
#define UDI_CDC_DEFAULT_DATABITS 8

#include "sio2host.h"
#include <udi_cdc_conf.h>

#endif /* _CONF_USB_H_ */
