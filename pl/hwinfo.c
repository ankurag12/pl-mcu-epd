/*
  Plastic Logic EPD project on MSP430

  Copyright (C) 2013, 2014 Plastic Logic Limited

  This program is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/
/*
 * pl/hwinfo.c -- Read the hardware info EEPROM
 *
 * Authors:
 *   Nick Terry <nick.terry@plasticlogic.com>
 *   Guillaume Tucker <guillaume.tucker@plasticlogic.com>
 *   Andrew Cox <andrew.cox@plasticlogic.com>
 *
 */

#include <pl/endian.h>
#include <pl/i2c.h>
#include <pl/hwinfo.h>
#include <stdint.h>
#include "assert.h"
#include "i2c-eeprom.h"
#include "crc16.h"

#define LOG_TAG "hwinfo"
#include "utils.h"

int pl_hw_info_init(struct pl_hw_info *info, struct i2c_eeprom *eeprom)
{
	struct pl_hw_vcom_info *vcom;
	struct pl_hw_board_info *board;
	uint16_t crc;

	assert(info != NULL);
	assert(eeprom != NULL);

	if (eeprom_read(eeprom, 0, sizeof(*info), (uint8_t *)info)) {
		LOG("Failed to read EEPROM contents");
		return -1;
	}

	if (info->version != PL_HW_INFO_VERSION) {
		LOG("Unsupported version number: %d, required version is %d",
		    info->version, PL_HW_INFO_VERSION);
		return -1;
	}

	crc = crc16_run(crc16_init, (const uint8_t *)info,
			(sizeof(info->version) + sizeof(info->vcom) +
			 sizeof(info->board)));
	info->crc = be16toh(info->crc);

	if (crc != info->crc) {
		LOG("CRC mismatch: %04X instead of %04X", crc, info->crc);
		return -1;
	}

	vcom = &info->vcom;
	vcom->dac_x1 = be16toh(vcom->dac_x1);
	vcom->dac_y1 = be16toh(vcom->dac_y1);
	vcom->dac_x2 = be16toh(vcom->dac_x2);
	vcom->dac_y2 = be16toh(vcom->dac_y2);
	swap32(vcom->vgpos_mv);
	swap32(vcom->vgneg_mv);
	swap32(vcom->swing_ideal);

	board = &info->board;
	board->board_type[8] = '\0';
	board->adc_scale_1 = be16toh(board->adc_scale_1);
	board->adc_scale_2 = be16toh(board->adc_scale_2);

	LOG("VCOM DAC info: dac[%d]=%d, dac[%d]=%d",
	    vcom->dac_x1, vcom->dac_y1, vcom->dac_x2, vcom->dac_y2);
	LOG("Gate PSU info: VGPOS=%ld, VGNEG=%ld, swing=%ld",
	    vcom->vgpos_mv, vcom->vgneg_mv, vcom->swing_ideal);
	LOG("Board type: %s, version: %d.%d",
	    board->board_type, board->board_ver_maj, board->board_ver_min);
	LOG("vcom_mode=%d, hv_pmic=%d, vcom_dac=%d, vcom_adc=%d",
	    board->vcom_mode, board->hv_pmic, board->vcom_dac, board->vcom_adc);
	LOG("io_config=%d, i2c_mode=%d, temp_sensor=%d, frame_buffer=%d",
	    board->io_config, board->i2c_mode, board->temp_sensor,
	    board->frame_buffer);
	LOG("epdc_ref=%d, adc_scale_1=%d, adc_scale_2=%d",
	    board->epdc_ref, board->adc_scale_1, board->adc_scale_2);
	LOG("CRC16: %04X", crc);

	return 0;
}