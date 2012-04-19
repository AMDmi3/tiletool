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

ttip_result_t ttip_maskblend(ttip_image_t* output, ttip_image_t background, ttip_image_t overlay) {
	if (background->width != overlay->width) {
		TTIP_VERROR("background and overlay widths don't match");
		return EINVAL;
	}

	if (background->height != overlay->height) {
		TTIP_VERROR("background and overlay heights don't match");
		return EINVAL;
	}

	switch (background->format) {
	case TTIP_GRAY:
		if (overlay->format != TTIP_GRAY_ALPHA) {
			TTIP_VERROR("background and overlay formats don't match (only TTIP_GRAY_ALPHA overlay allowed for TTIP_GRAY background)");
			return EINVAL;
		}
		break;
	case TTIP_RGB:
		if (overlay->format != TTIP_RGB_ALPHA) {
			TTIP_VERROR("background and overlay formats don't match (only TTIP_RGB_ALPHA overlay allowed for TTIP_RGB background)");
			return EINVAL;
		}
		break;
	default:
		TTIP_VERROR("unsupported background format (background with alpha?)");
		return EINVAL;
	}

	/* allocate tile */
	int ret;
	struct ttip_image* destination;
   	if ((ret = ttip_create(&destination, background->width, background->height, background->format)) != TTIP_OK)
		return ret;

	/* process */
	unsigned char *bgrow, *ovrrow, *dstrow, *bg, *ovr, *dst;
	if (background->format == TTIP_GRAY) {
		for (bgrow = background->data, ovrrow = overlay->data, dstrow = destination->data;
				bgrow < background->data + background->height * background->stride;
				bgrow += background->stride, ovrrow += overlay->stride, dstrow += destination->stride) {
			for (bg = bgrow, ovr = ovrrow, dst = dstrow;
					bg < bgrow + background->width;
					bg++, ovr += 2, dst++) {
				dst[0] = (bg[0] * (255 - ovr[1]) + ovr[0] * ovr[1]) / 255;
			}
		}
	} else if (background->format == TTIP_RGB) {
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
	}

	*output = destination;

	return TTIP_OK;
}
