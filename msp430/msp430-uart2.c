/*

 */

#include <pl/gpio.h>
#include <stdint.h>
#include <file.h>
#include "utils.h"
#include "msp430.h"
#include "msp430-defs.h"
#include "msp430-uart2.h"
#include "msp430-gpio.h"
#include "config.h"

#define USCI_UNIT	A
#define	USCI_CHAN	2
// Pin definitions for this unit.
#define	UART_TX                 MSP430_GPIO(9,4)
#define	UART_RX                 MSP430_GPIO(9,5)

#define LOG_TAG "msp430-uart2"

// protect from calls before initialization is complete.
static uint8_t init_done = 0;

// Buffer to store data
static unsigned char g_buff[BUFFER_SIZE];
static unsigned int g_buff_ptr = 0;

unsigned char* get_buffer(void)
{
    return &g_buff[0];
}

int buffer_available_for_read(void)
{
    return (g_buff_ptr == 0);
}

int msp430_uart2_getc(void)
{
    if (!init_done)
    {
        return -1;
    }

    while (!(UCxnIFG & UCRXIFG))
        ;

    return (UCxnRXBUF & 0x00ff);

}

int msp430_uart2_putc(int c)
{
    if (init_done)
    {
        while (!(UCxnIFG & UCTXIFG))
            ;
        UCxnTXBUF = c;
    }

    return c;
}

int msp430_uart2_puts(const char *s)
{
    unsigned int i;

    if (!init_done)
        return 1;

    for (i = 0; s[i]; i++)
        msp430_uart2_putc(s[i]);

    return i;
}

#pragma vector = USCI_xn_VECTOR
__interrupt void USCI_ISR(void)
{
    switch (__even_in_range(UCxnIV, 4))
    {
    case 0:
        break;                             // Vector 0 - no interrupt
    case 2:                                   // Vector 2 - RXIFG
        g_buff[g_buff_ptr++] = UCxnRXBUF;                  //  -> RXed character
        if (g_buff_ptr == BUFFER_SIZE)
        {
            g_buff_ptr = 0;
            // Wake-up device to process data. Doesn't work over LPM1
            __bic_SR_register_on_exit(LPM1_bits);
        }
        break;
    case 4:
        break;                             // Vector 4 - TXIFG
    default:
        break;
    }
}

//int msp430_uart2_register_files()
//{
//	add_device("msp430UART", _MSA,
//		msp430_uart_open, msp430_uart_close,
//		msp430_uart_read, msp430_uart_write,
//		msp430_uart_lseek, msp430_uart_unlink, msp430_uart_rename);
//
//	if (!freopen("msp430UART:stderrfile", "w", stderr)) {
//		LOG("Failed to freopen stderr");
//		return -1;
//	}
//
//	/* Don't buffer as we want to see error immediately*/
//	if (setvbuf(stderr, NULL, _IONBF, 0)) {
//		LOG("Failed to setvbuf stderr");
//		return -1;
//	}
//
//	if (!freopen("msp430UART:stdoutfile", "w", stdout)) {
//		LOG("Failed to freopen stdout");
//		return -1;
//	}
//
//	/* Don't buffer (saves calling fflush) */
//	if (setvbuf(stdout, NULL, _IONBF, 0)) {
//		LOG("Failed to setvbuf stdout");
//		return -1;
//	}
//
//	return 0;
//}

int msp430_uart2_init(struct pl_gpio *gpio, int baud_rate_id, char parity,
                      int data_bits, int stop_bits)
{
    static const struct pl_gpio_config gpios[] =
            { { UART_TX, PL_GPIO_SPECIAL | PL_GPIO_OUTPUT | PL_GPIO_INIT_L }, {
                    UART_RX, PL_GPIO_SPECIAL | PL_GPIO_INPUT }, };

    if (pl_gpio_config_list(gpio, gpios, ARRAY_SIZE(gpios)))
        return -1;

    // hold unit in reset while configuring
    UCxnCTL1 |= UCSWRST;

    UCxnCTL0 = UCMODE_0;// Uart Mode (No parity, LSB first, 8 data bits, 1 stop bit)
    UCxnCTL1 |= UCSSEL_2;			// SMCLK

    switch (data_bits)
    {
    case 7:
        UCxnCTL0 |= UC7BIT;
        break;
    case 8:
        UCxnCTL0 &= ~UC7BIT;
        break;
    default:
        return -1;
    }

    switch (stop_bits)
    {
    case 1:
        UCxnCTL0 &= ~UCSPB;
        break;
    case 2:
        UCxnCTL0 |= UCSPB;
        break;
    default:
        return -1;
    }

    switch (parity)
    {
    case 'N':
        UCxnCTL0 &= ~UCPEN;
        break;
    case 'E':
        UCxnCTL0 |= (UCPEN | UCPAR);
        break;
    case 'O':
        UCxnCTL0 |= UCPEN;
        break;
    default:
        return -1;
    }

    /* These registers taken from Table 34-4 and 34-5 of the
     * MSP430 Users guide.
     * They are dependent on a 20MHz clock
     */
#if CPU_CLOCK_SPEED_IN_HZ != 20000000L
#error CPU Clock speed not 20MHz - baud rate calculations not valid
#endif
    switch (baud_rate_id)
    {
    case BR_9600:
        UCxnBR0 = 130;
        UCxnBR1 = 0;
        UCxnMCTL = (UCOS16 | UCBRS_0 | UCBRF_3);
        break;
    case BR_19200:
        UCxnBR0 = 65;
        UCxnBR1 = 0;
        UCxnMCTL = (UCOS16 | UCBRS_0 | UCBRF_2);
        break;
    case BR_38400:
        UCxnBR0 = 32;
        UCxnBR1 = 0;
        UCxnMCTL = (UCOS16 | UCBRS_0 | UCBRF_9);
        break;
    case BR_57600:
        UCxnBR0 = 21;
        UCxnBR1 = 0;
        UCxnMCTL = (UCOS16 | UCBRS_0 | UCBRF_11);
        break;
    case BR_115200:
        UCxnBR0 = 10;
        UCxnBR1 = 0;
        UCxnMCTL = (UCOS16 | UCBRS_0 | UCBRF_14);
        break;
    case BR_230400:
        UCxnBR0 = 5;
        UCxnBR1 = 0;
        UCxnMCTL = (UCOS16 | UCBRS_0 | UCBRF_7);
        break;
    default:
        return -1;
    }

    // release unit from reset
    UCxnCTL1 &= ~UCSWRST;

    UCxnIE |= UCRXIE; // Enable USCI_xn RX interrupt

    init_done = 1;
    return 0;
//	return msp430_uart_register_files();
}

//int msp430_uart2_open(const char *path, unsigned flags, int llv_fd)
//{
//	/* TODO:
//	 * Possibly store some info here, maybe even use *path to specify port options
//	 * Possibly just check init_done?
//	 */
//	return (init_done == 1) ? 0 : -1;
//}
//
//int msp430_uart2_close(int dev_fd)
//{
//	/* TODO: tidy any storage and possibly hold UART in reset? */
//    return 0;
//}
//
//int msp430_uart2_read(int dev_fd, char *buf, unsigned count)
//{
//    /* Reads not allowed */
//	return -1;
//}
//
//int msp430_uart2_write(int dev_fd, const char *buf, unsigned count)
//{
//	unsigned int i;
//
//	if (!init_done)
//		return -1;
//
//	for(i = 0; i < count; i++)
//		msp430_uart_putc(buf[i]);
//
//	return i;
//}

//
//off_t msp430_uart2_lseek(int dev_fd, off_t offset, int origin)
//{
//	/* TODO: not sure this is correct */
//	return 0;
//}
//
//int msp430_uart2_unlink(const char *path)
//{
//     return 0;
//}
//
//int msp430_uart2_rename(const char *old_name, const char *new_name)
//{
//    return 0;
//}

