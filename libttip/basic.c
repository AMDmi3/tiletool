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

#include <memory.h>
#include <string.h>

#include <ttip_int.h>

ttip_result_t ttip_clear(ttip_image_t target) {
	memset(target->data, 0, target->stride * target->height);

	return TTIP_OK;
}

ttip_result_t ttip_copy(ttip_image_t target, ttip_image_t source) {
	if (target->width != source->width || target->height != source->height || target->format != source->format)
		return TTIP_IMAGE_FORMAT_MISMATCH;

	if (source->stride == target->stride) {
		memcpy(target->data, source->data, source->height * source->stride);
	} else {
		/* copy by-row, if strides do not match */
		unsigned char* src = source->data;
		unsigned char* dst = target->data;
		int row;
		for (row = 0; row < source->height; row++) {
			memcpy(dst, src, source->width * ttip_getbpp(source->format));
			src += source->stride;
			dst += target->stride;
		}
	}

	return TTIP_OK;
}

ttip_result_t ttip_clone(ttip_image_t* output, ttip_image_t source) {
	/* allocate tile */
	int ret;
	struct ttip_image* destination;
	if ((ret = ttip_create(&destination, source->width, source->height, source->format)) != TTIP_OK)
		return ret;

	if ((ret != ttip_copy(destination, source)) != TTIP_OK) {
		ttip_destroy(&destination);
		return ret;
	}

	*output = destination;
	return TTIP_OK;
}
