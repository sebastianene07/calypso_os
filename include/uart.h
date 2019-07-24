#ifndef __UART_H
#define __UART_H

int uart_low_init(void);

int uart_low_send(char *msg);

char uart_low_receive(void);

#endif /* __UART_H */
