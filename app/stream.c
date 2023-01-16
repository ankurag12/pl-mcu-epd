/*
 * stream.c
 *
 *  Created on: Jan 15, 2023
 *      Author: ankuragrawal
 */

#include <msp430/msp430-uart2.h>
#include <pl/platform.h>
#include <pl/epdc.h>
#include <pl/epdpsu.h>

#define LOG_TAG "stream"
#include "utils.h"

enum data_packet_cmd
{
    DO_NOTHING,
    CLEAR_DISPLAY,
    INIT_FILE_TRANSFER,
    READ_DATA_PACKET,
    END_FILE_TRANSFER
};

enum data_packet_response
{
    UNKNOWN, ACK_SEND_NEXT, ERROR_SEND_PACKET_AGAIN, ERROR_RESTART_COMM
};

int process_data_packet(struct pl_platform *plat);

int process_data_packet(struct pl_platform *plat)
{
    unsigned char *buff;
    unsigned i;
    unsigned cmd;
    struct pl_epdc *epdc = &plat->epdc;
    struct pl_epdpsu *psu = &plat->psu;
    static int wfid;

    buff = get_buffer();

    cmd = *(buff + 0);

    switch (cmd)
    {
    case DO_NOTHING:
        break;

    case CLEAR_DISPLAY:
        break;

    case INIT_FILE_TRANSFER:
        LOG("Initing file transfer");
//        wfid = pl_epdc_get_wfid(epdc, wf_refresh);
//        if (wfid < 0)
//            return -1;

        break;

    case READ_DATA_PACKET:
        for (i = 1; i < BUFFER_SIZE - 1; ++i)
        {
            LOG("Received = %c", *(buff + i));
        }
        break;

    case END_FILE_TRANSFER:
        LOG("Finishing file transfer");
//        if (epdc->update_temp(epdc))
//            return -1;
//
//        if (psu->on(psu))
//            return -1;
//
//        if (epdc->update(epdc, wfid, NULL))
//            return -1;
//
//        if (epdc->wait_update_end(epdc))
//            return -1;
//
//        if (psu->off(psu))
//            return -1;

        break;

    default:
        break;

    }

    return 0;
}

int process_and_respond(struct pl_platform *plat)
{
    int ret = process_data_packet(plat);

    // TODO: Respond in more detail
    switch (ret)
    {
    case 0:
        msp430_uart2_putc(ACK_SEND_NEXT);
        break;
    case -1:
        msp430_uart2_putc(ERROR_SEND_PACKET_AGAIN);
        break;

    default:
        break;
    }

    return 0;

}
//
//static int show_image(struct pl_platform *plat)
//{
//
//    if (epdc->load_image(epdc, path, NULL, 0, 0))
//        return -1;
//
//    return 0;
//}
