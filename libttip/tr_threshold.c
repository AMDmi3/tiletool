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

#include <errno.h>

#include <ttip_int.h>

ttip_result_t ttip_threshold(ttip_image_t* output, ttip_image_t source, int value) {
	if (source->format != TTIP_GRAY && source->format != TTIP_RGB)
		return TTIP_BAD_PIXEL_FORMAT;

	/* allocate tile */
	int ret;
	struct ttip_image* destination;
   	if ((ret = ttip_create(&destination, source->width, source->height, TTIP_GRAY)) != TTIP_OK)
		return ret;

	/* process */
	unsigned char *srcrow, *dstrow, *src, *dst;

	if (source->format == TTIP_GRAY) {
		for (srcrow = source->data, dstrow = destination->data;
				srcrow < source->data + source->height * source->stride;
				srcrow += source->stride, dstrow += destination->stride) {
			for (src = srcrow, dst = dstrow; src < srcrow + source->width; src++, dst++) {
				dst[0] = (src[0] > value) ? 255 : 0;
			}
		}
	} else if (source->format == TTIP_RGB) {
		for (srcrow = source->data, dstrow = destination->data;
				srcrow < source->data + source->height * source->stride;
				srcrow += source->stride, dstrow += destination->stride) {
			for (src = srcrow, dst = dstrow; src < srcrow + source->width * 3; src += 3, dst++) {
				dst[0] = (desaturate(src[0], src[1], src[2]) > value) ? 255 : 0;
			}
		}
	}

	*output = destination;

	return TTIP_OK;
}
