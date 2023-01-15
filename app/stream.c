/*
 * stream.c
 *
 *  Created on: Jan 15, 2023
 *      Author: ankuragrawal
 */

#include <msp430/msp430-uart2.h>
#define LOG_TAG "stream"
#include "utils.h"

int process_data_packet(void)
{
    unsigned char *buff;
    unsigned i;

    if (buffer_available_for_read())
    {
        buff = get_buffer();

        for (i = 0; i < BUFFER_SIZE; ++i)
        {
            LOG("Received = %c", *(buff + i));
        }
        msp430_uart2_putc("A");
    }
}
