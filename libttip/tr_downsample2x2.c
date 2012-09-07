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

static void ttip_downsample_single(ttip_image_t target, ttip_image_t source, int xoffset, int yoffset) {
	unsigned char *srcrow1, *srcrow2, *dstrow;
	unsigned char *src1, *src2, *dst;

	int step = ttip_getbpp(source->format);

	for (srcrow1 = source->data, dstrow = target->data + target->stride * yoffset;
			srcrow1 < source->data + source->stride * source->height;
			srcrow1 += source->stride * 2, dstrow += target->stride) {
		srcrow2 = srcrow1 + source->stride;
		int pos = 0;
		for (src1 = srcrow1, src2 = srcrow2, dst = dstrow + xoffset * step;
				src1 < srcrow1 + source->width * step;
				src1++, src2++, dst++) {

			/* add 2 to round to nearest */
			dst[0] = ((int)src1[0] + (int)src1[step] + (int)src2[0] + (int)src2[step] + 2)/4;

			if (++pos == step) {
				src1 += step;
				src2 += step;
				pos = 0;
			}
		}
	}
}

ttip_result_t ttip_downsample2x2(ttip_image_t* output, ttip_image_t topleft, ttip_image_t topright, ttip_image_t bottomleft, ttip_image_t bottomright) {
	/* check that all params match */
	ttip_image_t arr[4] = { topleft, topright, bottomleft, bottomright };
	int i;
	for (i = 1; i < 4; ++i) {
		if (arr[1]->width != arr[0]->width)
			return TTIP_IMAGE_DIMENSIONS_MISMATCH;
		if (arr[1]->height != arr[0]->height)
			return TTIP_IMAGE_DIMENSIONS_MISMATCH;
		if (arr[1]->format != arr[0]->format)
			return TTIP_IMAGE_FORMAT_MISMATCH;
	}

	if ((topleft->width & 1) || (topleft->height & 1))
		return TTIP_EVEN_DIMENSIONS_REQUIRED;

	/* allocate tile */
	int ret;
	struct ttip_image* destination;
   	if ((ret = ttip_create(&destination, topleft->width, topleft->height, topleft->format)) != TTIP_OK)
		return ret;

	/* process */
	ttip_downsample_single(destination, topleft, 0, 0);
	ttip_downsample_single(destination, topright, topleft->width/2, 0);
	ttip_downsample_single(destination, bottomleft, 0, topleft->height/2);
	ttip_downsample_single(destination, bottomright, topleft->width/2, topleft->height/2);

	*output = destination;

	return TTIP_OK;
}
