/*
 * Copyright (C) 2012 Dmitry Marakasov
 *
 * This file is part of libttip.
 *
 * libttip is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * libttip is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with libttip.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <ttip.h>

#include "testing.h"

BEGIN_TEST()
	int x, y;
	ttip_image_t tile;

	EXPECT_TRUE(ttip_create(&tile, 256, 256, TTIP_RGB) == TTIP_OK);

	for (y = 0; y < 256; y++)
		for (x = 0; x < 256; x++)
			ttip_setpixel(tile, x, y, (x << 8) | y);

	EXPECT_TRUE(ttip_savepng(tile, "test.png", 6) == TTIP_OK);

	ttip_destroy(&tile);

	EXPECT_TRUE(ttip_loadpng(&tile, "test.png") == TTIP_OK);

	int nmismatches = 0;
	for (y = 0; y < 256; y++)
		for (x = 0; x < 256; x++)
			nmismatches += (ttip_getpixel(tile, x, y) != (ttip_color_t)((x << 8) | y));

	EXPECT_TRUE(nmismatches == 0);

	ttip_destroy(&tile);
END_TEST()
