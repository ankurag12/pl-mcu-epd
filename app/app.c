/*
  Plastic Logic EPD project on MSP430

  Copyright (C) 2014 Plastic Logic Limited

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
 * app/app.c -- Application
 *
 * Authors:
 *   Guillaume Tucker <guillaume.tucker@plasticlogic.com>
 *
 */

#include <pl/platform.h>

#define LOG_TAG "app"
#include "utils.h"

/* Interim implementation */
#include <epson/epson-s1d135xx.h>
#include <epson/epson-utils.h>
#include <epson/S1D135xx.h>
#include <epson/S1D13541.h>

int app_demo(struct platform *plat)
{
	if (app_clear(plat))
		return -1;

	LOG("That's all folks.");

	return 0;
}

int app_clear(struct platform *plat)
{
	struct s1d135xx *s1d135xx = plat->epdc.data;
	struct _s1d135xx *epson = s1d135xx->epson;

	/* Interim implementation using direct Epson functions */
	epson_fill_buffer(0x0030, false, epson->yres, epson->xres, 0xff);
	s1d13541_init_display(epson);

	if (plat->psu.on(&plat->psu))
		return -1;

	if (plat->epdc.update(&plat->epdc,
			      pl_epdc_get_wfid(&plat->epdc, wf_init)))
		return -1;

	plat->epdc.wait_update_end(&plat->epdc);

	if (plat->psu.off(&plat->psu))
		return -1;

	return 0;
}