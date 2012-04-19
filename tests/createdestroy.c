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
	ttip_image_t tile = NULL;

	EXPECT_TRUE(ttip_create(&tile, 256, 128, TTIP_RGB) == TTIP_OK);
	EXPECT_TRUE(tile != NULL);
	EXPECT_TRUE(ttip_getwidth(tile) == 256);
	EXPECT_TRUE(ttip_getheight(tile) == 128);
	EXPECT_TRUE(ttip_getformat(tile) == TTIP_RGB);

	ttip_destroy(&tile);

	EXPECT_TRUE(tile == NULL);
END_TEST()
