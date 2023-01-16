/*

 */

#ifndef MSP430_UART2_H
#define MSP430_UART2_H 1

#define	BR_9600		1
#define	BR_19200	2
#define	BR_38400	3
#define	BR_57600	4
#define	BR_115200	5
#define	BR_230400	6

#define BUFFER_SIZE 7

struct pl_gpio;

//int    msp430_uart_open(const char *path, unsigned flags, int llv_fd);
//int    msp430_uart_close(int dev_fd);
//int    msp430_uart_read(int dev_fd, char *buf, unsigned count);
//int    msp430_uart_write(int dev_fd, const char *buf, unsigned count);
//fpos_t msp430_uart_lseek(int dev_fd, fpos_t offset, int origin);
//int    msp430_uart_unlink(const char *path);
//int    msp430_uart_rename(const char *old_name, const char *new_name);

int msp430_uart2_init(struct pl_gpio *gpio, int baud_rate_id, char parity,
                      int data_bits, int stop_bits);
int msp430_uart2_getc(void);
int msp430_uart2_putc(int c);
unsigned char *get_buffer(void);
int buffer_available_for_read(void);

#endif /* MSP430_UART2_H */
