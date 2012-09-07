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
#include <assert.h>

#include <ttip_int.h>

ttip_result_t ttip_maskblend(ttip_image_t* output, ttip_image_t background, ttip_image_t overlay) {
	if (background->width != overlay->width)
		return TTIP_IMAGE_DIMENSIONS_MISMATCH;

	if (background->height != overlay->height)
		return TTIP_IMAGE_DIMENSIONS_MISMATCH;

	if (background->format != TTIP_GRAY && background->format != TTIP_RGB)
		return TTIP_BAD_PIXEL_FORMAT;

	if (overlay->format != TTIP_GRAY_ALPHA && overlay->format != TTIP_RGB_ALPHA)
		return TTIP_BAD_PIXEL_FORMAT;

	/* allocate tile */
	int ret;
	struct ttip_image* destination;
	int output_format = (background->format == TTIP_GRAY && overlay->format == TTIP_GRAY_ALPHA) ? TTIP_GRAY : TTIP_RGB;
	if ((ret = ttip_create(&destination, background->width, background->height, output_format)) != TTIP_OK)
		return ret;

	/* process */
	unsigned char *bgrow, *ovrrow, *dstrow, *bg, *ovr, *dst;
	if (background->format == TTIP_GRAY && overlay->format == TTIP_GRAY_ALPHA) {
		for (bgrow = background->data, ovrrow = overlay->data, dstrow = destination->data;
				bgrow < background->data + background->height * background->stride;
				bgrow += background->stride, ovrrow += overlay->stride, dstrow += destination->stride) {
			for (bg = bgrow, ovr = ovrrow, dst = dstrow;
					bg < bgrow + background->width;
					bg++, ovr += 2, dst++) {
				dst[0] = (bg[0] * (255 - ovr[1]) + ovr[0] * ovr[1]) / 255;
			}
		}
	} else if (background->format == TTIP_GRAY && overlay->format == TTIP_RGB_ALPHA) {
		for (bgrow = background->data, ovrrow = overlay->data, dstrow = destination->data;
				bgrow < background->data + background->height * background->stride;
				bgrow += background->stride, ovrrow += overlay->stride, dstrow += destination->stride) {
			for (bg = bgrow, ovr = ovrrow, dst = dstrow;
					bg < bgrow + background->width;
					bg ++, ovr += 4, dst += 3) {
				int background_component = bg[0] * (255 - ovr[3]);
				dst[0] = (background_component + ovr[0] * ovr[3]) / 255;
				dst[1] = (background_component + ovr[1] * ovr[3]) / 255;
				dst[2] = (background_component + ovr[2] * ovr[3]) / 255;
			}
		}
	} else if (background->format == TTIP_RGB && overlay->format == TTIP_GRAY_ALPHA) {
		for (bgrow = background->data, ovrrow = overlay->data, dstrow = destination->data;
				bgrow < background->data + background->height * background->stride;
				bgrow += background->stride, ovrrow += overlay->stride, dstrow += destination->stride) {
			for (bg = bgrow, ovr = ovrrow, dst = dstrow;
					bg < bgrow + background->width * 3;
					bg += 3, ovr += 2, dst += 3) {
				int overlay_component = ovr[0] * ovr[1];
				dst[0] = (bg[0] * (255 - ovr[1]) + overlay_component) / 255;
				dst[1] = (bg[1] * (255 - ovr[1]) + overlay_component) / 255;
				dst[2] = (bg[2] * (255 - ovr[1]) + overlay_component) / 255;
			}
		}
	} else if (background->format == TTIP_RGB && overlay->format == TTIP_RGB_ALPHA) {
		for (bgrow = background->data, ovrrow = overlay->data, dstrow = destination->data;
				bgrow < background->data + background->height * background->stride;
				bgrow += background->stride, ovrrow += overlay->stride, dstrow += destination->stride) {
			for (bg = bgrow, ovr = ovrrow, dst = dstrow;
					bg < bgrow + background->width * 3;
					bg += 3, ovr += 4, dst += 3) {
				dst[0] = (bg[0] * (255 - ovr[3]) + ovr[0] * ovr[3]) / 255;
				dst[1] = (bg[1] * (255 - ovr[3]) + ovr[1] * ovr[3]) / 255;
				dst[2] = (bg[2] * (255 - ovr[3]) + ovr[2] * ovr[3]) / 255;
			}
		}
	} else {
		assert(0);
	}

	*output = destination;

	return TTIP_OK;
}
