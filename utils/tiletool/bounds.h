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

#ifndef BOUNDS_H
#define BOUNDS_H

typedef struct {
	double left;
	double right;
	double top;
	double bottom;
} Bounds;

#define BOUNDS_FULL_INITIALIZER { 0.0, 1.0, 0.0, 1.0 }

int parse_bounds(const char* string, Bounds* bounds);
int is_tile_in_bounds(int x, int y, int zoom, Bounds* bounds);

#endif
