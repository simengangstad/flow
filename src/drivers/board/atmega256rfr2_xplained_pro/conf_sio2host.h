#ifndef CONF_SIO2HOST_H_INCLUDED
#define CONF_SIO2HOST_H_INCLUDED

#define USART_HOST             (&USARTA1)
#define USART_HOST_BAUDRATE    38400
#define USART_HOST_CHAR_LENGTH USART_CHSIZE_8BIT_gc
#define USART_HOST_PARITY      USART_PMODE_DISABLED_gc
#define USART_HOST_STOP_BITS   1

#define SERIAL_RX_BUF_SIZE_HOST 156
#define USART_HOST_RX_ISR_ENABLE() \
    usart_rx_complete_interrupt_enable(USART_HOST)
#define USART_HOST_ISR_VECT() ISR(USART1_RX_vect)

#endif /* CONF_SIO2HOST_H_INCLUDED */
