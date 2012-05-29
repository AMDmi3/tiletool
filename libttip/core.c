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
#include <stdlib.h>
#include <string.h>

#include <ttip_int.h>

ttip_result_t ttip_create(ttip_image_t* output, unsigned int width, unsigned int height, ttip_format_t format) {
	if (width <= 0)
		return TTIP_BAD_DIMENSIONS;

	if (height <= 0)
		return TTIP_BAD_DIMENSIONS;

	if (ttip_getbpp(format) == 0)
		return TTIP_BAD_PIXEL_FORMAT;

	size_t stridesize = ttip_alignstride(width * ttip_getbpp(format));

	struct ttip_image* newtile = malloc(sizeof(struct ttip_image));
	if (newtile == NULL)
		return errno;

	unsigned char* data = malloc(stridesize * height);
	if (data == NULL) {
		int saved_errno = errno;
		free(newtile);
		return saved_errno;
	}

	newtile->width = width;
	newtile->height = height;
	newtile->stride = stridesize;
	newtile->format = format;
	newtile->data = data;

	*output = newtile;

	return TTIP_OK;
}

void ttip_destroy(ttip_image_t* tile) {
	if (*tile != NULL) {
		free((*tile)->data);
		free(*tile);
		*tile = NULL;
	}
}

const char* ttip_strerror(ttip_result_t error) {
	switch (error) {
	case TTIP_LIBPNG_INIT_FAILED:
		return "Libpng initialization failed";
	case TTIP_LIBPNG_ERROR:
		return "Error during libpng input/output";
	case TTIP_NOT_IMPLEMENTED:
		return "Functionality not (yet) implemented";
	case TTIP_IMAGE_FORMAT_NOT_SUPPORTED:
		return "Image format not supported";
	case TTIP_NOT_COMPILED_IN:
		return "Feature not compiled in";
	case TTIP_BAD_DIMENSIONS:
		return "Bad image demensions";
	case TTIP_BAD_PIXEL_FORMAT:
		return "Bad pixel format";
	case TTIP_IMAGE_FORMAT_MISMATCH:
		return "Image format mismatch";
	case TTIP_EVEN_DIMENSIONS_REQUIRED:
		return "Even dimensions required";
	default:
		return strerror(error);
	}
}

int ttip_getwidth(ttip_image_t tile) {
	return tile->width;
}

int ttip_getheight(ttip_image_t tile) {
	return tile->height;
}

ttip_format_t ttip_getformat(ttip_image_t tile) {
	return tile->format;
}

int ttip_issameformat(ttip_image_t a, ttip_image_t b) {
	return a->width == b->width &&
		a->height == b->height &&
		a->format == b->format;
}

int ttip_issamedimension(ttip_image_t a, ttip_image_t b) {
	return a->width == b->width &&
		a->height == b->height;
}
