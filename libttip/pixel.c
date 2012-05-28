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

#include <ttip_int.h>

void ttip_setpixel(ttip_image_t tile, unsigned int x, unsigned int y, ttip_color_t color) {
	if (x >= tile->width || y >= tile->height)
		return;

	ttip_writepixel(tile->data + tile->stride * y + ttip_getbpp(tile->format) * x, color, tile->format);
}

ttip_color_t ttip_getpixel(ttip_image_t tile, unsigned int x, unsigned int y) {
	if (x >= tile->width || y >= tile->height)
		return 0;

	return ttip_readpixel(tile->data + tile->stride * y + ttip_getbpp(tile->format) * x, tile->format);
}
