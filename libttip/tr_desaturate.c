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

ttip_result_t ttip_desaturate(ttip_image_t* output, ttip_image_t source) {
	ttip_format_t dstformat;
	int srcstep, dststep;

	switch (source->format) {
	case TTIP_GRAY:
	case TTIP_GRAY_ALPHA:
		return ttip_clone(output, source);
	case TTIP_RGB:
		dstformat = TTIP_GRAY;
		srcstep = 3;
		dststep = 1;
		break;
	case TTIP_RGB_ALPHA:
		dstformat = TTIP_GRAY_ALPHA;
		srcstep = 4;
		dststep = 2;
		break;
	default:
		return EINVAL;
	}

	/* allocate tile */
	int ret;
	struct ttip_image* destination;
	if ((ret = ttip_create(&destination, source->width, source->height, dstformat)) != TTIP_OK)
		return ret;

	/* process */
	unsigned char *srcrow, *dstrow, *src, *dst;
	for (srcrow = source->data, dstrow = destination->data;
			srcrow < source->data + source->height * source->stride;
			srcrow += source->stride, dstrow += destination->stride) {
		for (src = srcrow, dst = dstrow; src < srcrow + source->width * srcstep; src += srcstep, dst += dststep) {
			*dst = desaturate(src[0], src[1], src[2]);
		}
	}

	*output = destination;

	return TTIP_OK;
}
