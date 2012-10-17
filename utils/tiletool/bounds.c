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

#include <string.h>
#include <stdlib.h>

#include "bounds.h"
#include "parsing.h"

/* ~1cm on equator */
#define BOUNDS_EPS 2.5e-10

/* parses bounds in format z/x-X/y-Y */
static int parse_zxy_bounds(const char* string, Bounds* bounds) {
	const char* slashes[2];
	if (parse_split(string, '/', slashes, 2) != 2)
		return 0;

	int zoom, minx, maxx, miny, maxy;

	if (!parse_unsigned(string, slashes[0], &zoom))
		return 0;
	if (!parse_unsigned_range(slashes[0] + 1, slashes[1], &minx, &maxx))
		return 0;
	if (!parse_unsigned_range(slashes[1] + 1, string + strlen(string), &miny, &maxy))
		return 0;

	double span = (double)(1 << zoom);
	bounds->left = minx / span;
	bounds->right = (maxx + 1) / span;
	bounds->top = miny / span;
	bounds->bottom = (maxy + 1) / span;

	return 1;
}

static int parse_bbox_bounds(const char* string, Bounds* bounds) {
	const char *end = string + strlen(string);
	const char *comma1, *comma2, *comma3;

	if ((comma1 = strchr(string, ',')) == NULL)
		return 0;

	if ((comma2 = strchr(comma1 + 1, ',')) == NULL)
		return 0;

	if ((comma3 = strchr(comma2 + 1, ',')) == NULL)
		return 0;

	double left, bottom, right, top;

	if (!parse_double(string, comma1, &left))
		return 0;
	if (!parse_double(comma1 + 1, comma2, &bottom))
		return 0;
	if (!parse_double(comma2 + 1, comma3, &right))
		return 0;
	if (!parse_double(comma3 + 1, end, &top))
		return 0;

	if (left > right) {
		double tmp = left;
		left = right;
		right = tmp;
	}

	if (bottom > top) {
		double tmp = top;
		top = bottom;
		bottom = tmp;
	}

	if (left < -180.0 || right > 180.0)
		return 0;
	if (bottom < -90.0 || top > 90.0)
		return 0;

	bounds->left = 0.5 + left / 360.0;
	bounds->right = 0.5 + right / 360.0;
	bounds->top = 0.5 - top / 180.0;
	bounds->bottom = 0.5 - bottom / 180.0;

	return 1;
}

int parse_bounds(const char* string, Bounds* bounds) {
	if (strchr(string, '/') != NULL)
		return parse_zxy_bounds(string, bounds);

	return parse_bbox_bounds(string, bounds);
}

int is_tile_in_bounds(int x, int y, int zoom, Bounds* bounds) {
	double span = 1 << zoom;

	return !(
				(x / span > bounds->right - BOUNDS_EPS) ||
				((x + 1) / span < bounds->left + BOUNDS_EPS) ||
				(y / span > bounds->bottom - BOUNDS_EPS) ||
				((y + 1) / span < bounds->top + BOUNDS_EPS)
			);
}
