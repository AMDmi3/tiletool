/*
 * Copyright (C) 2012 Dmitry Marakasov
 *
 * This file is part of tiletool.
 *
 * tiletool is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * tiletool is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with tiletool.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <stddef.h>
#include <err.h>

#include <ttip.h>

#include "emptytile.h"

static ttip_image_t g_empty_tile = NULL;

void init_empty_tile(const char* path) {
	cleanup_empty_tile();

	ttip_result_t ret;
	if ((ret = ttip_loadpng(&g_empty_tile, path)) != TTIP_OK)
		errx(1, "Cannot load empty tile: %s", ttip_strerror(ret));
}

void cleanup_empty_tile() {
	if (g_empty_tile)
		ttip_destroy(&g_empty_tile);
}

ttip_image_t spawn_empty_tile() {
	ttip_image_t out = NULL;
	ttip_result_t res;

	if (!g_empty_tile)
		errx(1, "No empty tile specified");

	if ((res = ttip_clone(&out, g_empty_tile)) != TTIP_OK)
		errx(1, "Cannot clone empty tile: %s", ttip_strerror(res));

	return out;
}

int has_empty_tile() {
	return g_empty_tile != NULL;
}
