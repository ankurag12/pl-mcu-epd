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
 * app/slideshow.c -- Basic slideshow app
 *
 * Authors:
 *   Nick Terry <nick.terry@plasticlogic.com>
 *   Guillaume Tucker <guillaume.tucker@plasticlogic.com>
 *
 */

#include "app.h"
#include <pl/platform.h>
#include <pl/epdc.h>
#include <pl/epdpsu.h>
#include <stdio.h>
#include <string.h>
#include "assert.h"

#define LOG_TAG "slideshow"
#include "utils.h"

/* -- private functions -- */

static int show_image(struct pl_platform *plat, const char *dir,
		      const char *file);

/* -- public entry point -- */

int app_slideshow(struct pl_platform *plat, const char *path)
{
	DIR dir;
	FILINFO f;
	int dir_open = 0;

	assert(plat != NULL);
	assert(path != NULL);

	LOG("Running slideshow");

	while (!app_stop) {
		if (!dir_open) {
			/* (re-)open the directory */
			if (f_opendir(&dir, path) != FR_OK) {
				LOG("Failed to open directory [%s]", path);
				return -1;
			}

			dir_open = 1;
		}

		/* read next entry in the directory */
		if (f_readdir(&dir, &f) != FR_OK) {
			LOG("Failed to read directory entry");
			return -1;
		}

		/* end of the directory reached */
		if (f.fname[0] == '\0') {
			dir_open = 0;
			continue;
		}

		/* skip directories */
		if ((f.fname[0] == '.') || (f.fattrib & AM_DIR))
			continue;

		/* only show PGM files */
		if (!strstr(f.fname, ".PGM"))
			continue;

		if (show_image(plat, path, f.fname)) {
			LOG("Failed to show image");
			return -1;
		}
	}

	return 0;
}

static int show_image(struct pl_platform *plat, const char *dir,
		      const char *file)
{
	struct pl_epdc *epdc = &plat->epdc;
	struct pl_epdpsu *psu = &plat->psu;
	char path[MAX_PATH_LEN];
	int wfid;

	wfid = 2;

//	if (wfid < 0)
//		return -1;

	join_path(path, sizeof(path), dir, file);

	epdc->wait_update_end(epdc);

	epdc->load_image(epdc, path, NULL, 0, 0);


	epdc->update_temp(epdc);


//	if (psu->on(psu))
//		return -1;

	epdc->update(epdc, 2, UPDATE_FULL, NULL);


	epdc->wait_update_end(epdc);


//	if (psu->off(psu))
//		return -1;

	return 0;
}
