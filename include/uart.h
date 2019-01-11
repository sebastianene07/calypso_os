#ifndef __UART_H
#define __UART_H

int uart_init(void);

int uart_send(char *msg, int msg_len);

char uart_receive(void);

#endif /* __UART_H */
