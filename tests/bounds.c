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

#include "math.h"

#include "bounds.h"

#include "testing.h"

#define EPS 0.00000001

BEGIN_TEST()
	Bounds bounds;

	const char* full_variants[] = {
		"-180,-90,180,90",
		"-180.0,-90.0,180.0,90.0",
		"180.0,-90.0,-180.0,90.0",
		"180.0,90.0,-180.0,-90.0",
		"-180.0,90.0,180.0,-90.0",
		"0/0/0",
		"1/0-1/0-1",
		"2/0-3/0-3",
		"3/0-7/0-7",
	};

	/* check whether all variants of whole-earth bounds work */
	unsigned int i, j;
	for (i = 0; i < sizeof(full_variants)/sizeof(full_variants[0]); ++i) {
		fprintf(stderr, "Checking %s:\n", full_variants[i]);
		bounds.left = bounds.right = bounds.bottom = bounds.top = 0.5;
		EXPECT_TRUE(parse_bounds(full_variants[i], &bounds));

		EXPECT_TRUE(fabs(bounds.left) < EPS);
		EXPECT_TRUE(fabs(bounds.right - 1.0) < EPS);
		EXPECT_TRUE(fabs(bounds.top) < EPS);
		EXPECT_TRUE(fabs(bounds.bottom - 1.0) < EPS);

		EXPECT_TRUE(is_tile_in_bounds(0, 0, 0, &bounds));

		/* these are actually UB, but check to improve coverage */
		EXPECT_FALSE(is_tile_in_bounds(1, 0, 0, &bounds));
		EXPECT_FALSE(is_tile_in_bounds(0, 1, 0, &bounds));
	}

	const char* quarter_bounds[] = {
		"-180,90,0,0",
		"180,90,0,0",
		"-180,-90,0,0",
		"180,-90,0,0",
	};

	int zoom;
	for (zoom = 1; zoom < 31; ++zoom) {
		fprintf(stderr, "Checking zoom %d:\n", zoom);
		unsigned long midtile = ((unsigned long)1 << zoom) / 2 - 1;
		for (i = 0; i < 4; ++i) {
			EXPECT_TRUE(parse_bounds(quarter_bounds[i], &bounds));

			for (j = 0; j < 4; ++j) {
				if (i == j) {
					EXPECT_TRUE(is_tile_in_bounds(midtile + (j & 1), midtile + (j & 2) / 2, zoom, &bounds));
				} else {
					EXPECT_FALSE(is_tile_in_bounds(midtile + (j & 1), midtile + (j & 2) / 2, zoom, &bounds));
				}
			}
		}
	}

END_TEST()
