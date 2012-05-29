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

ttip_result_t ttip_desaturate_inplace(ttip_image_t target, ttip_image_t source) {
	int srcstep, dststep;

	switch (source->format) {
	case TTIP_GRAY:
	case TTIP_GRAY_ALPHA:
		if (!ttip_issameformat(target, source))
			return TTIP_IMAGE_FORMAT_MISMATCH;
		return ttip_copy(target, source);
	case TTIP_RGB:
		if (!ttip_issamedimension(target, source) || target->format != TTIP_GRAY)
			return TTIP_IMAGE_FORMAT_MISMATCH;
		srcstep = 3;
		dststep = 1;
		break;
	case TTIP_RGB_ALPHA:
		if (!ttip_issamedimension(target, source) || target->format != TTIP_GRAY_ALPHA)
			return TTIP_IMAGE_FORMAT_MISMATCH;
		srcstep = 4;
		dststep = 2;
		break;
	default:
		return TTIP_BAD_PIXEL_FORMAT;
	}

	/* process */
	unsigned char *srcrow, *dstrow, *src, *dst;
	for (srcrow = source->data, dstrow = target->data;
			srcrow < source->data + source->height * source->stride;
			srcrow += source->stride, dstrow += target->stride) {
		for (src = srcrow, dst = dstrow; src < srcrow + source->width * srcstep; src += srcstep, dst += dststep) {
			*dst = desaturate(src[0], src[1], src[2]);
		}
	}

	return TTIP_OK;
}

ttip_result_t ttip_desaturate(ttip_image_t* output, ttip_image_t source) {
	ttip_format_t dstformat;

	switch (source->format) {
	case TTIP_GRAY:
	case TTIP_GRAY_ALPHA:
		dstformat = source->format;
		break;
	case TTIP_RGB:
		dstformat = TTIP_GRAY;
		break;
	case TTIP_RGB_ALPHA:
		dstformat = TTIP_GRAY_ALPHA;
		break;
	default:
		return TTIP_BAD_PIXEL_FORMAT;
	}

	int ret;
	struct ttip_image* destination;
	if ((ret = ttip_create(&destination, source->width, source->height, dstformat)) != TTIP_OK)
		return ret;

	if ((ret = ttip_desaturate_inplace(destination, source)) != TTIP_OK) {
		ttip_destroy(&destination);
		return ret;
	}

	*output = destination;

	return TTIP_OK;
}
