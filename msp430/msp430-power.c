/*
 * msp430-power.c
 *
 *  Created on: Jan 15, 2023
 *      Author: ankuragrawal
 */
#include "msp430.h"

void set_lpm(int mode)
{
    switch (mode)
    {
        case 0:
            __bis_SR_register(LPM0_bits + GIE); // Enter LPM0, interrupts enabled
            break;
        case 1:
            __bis_SR_register(LPM1_bits + GIE); // Enter LPM1, interrupts enabled
            break;
        case 2:
            __bis_SR_register(LPM2_bits + GIE); // Enter LPM2, interrupts enabled
            break;
        case 3:
            __bis_SR_register(LPM3_bits + GIE); // Enter LPM3, interrupts enabled
            break;
        case 4:
            __bis_SR_register(LPM4_bits + GIE); // Enter LPM4, interrupts enabled
            break;
    }

}
