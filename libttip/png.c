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

#include <assert.h>
#include <errno.h>
#include <png.h>
#include <stdlib.h>
#include <unistd.h>

#include <ttip_int.h>

ttip_result_t ttip_savepng(ttip_image_t source, const char* filename, int level) {
#if defined(WITH_PNG)
	FILE* f;
	png_structp png_ptr;
	png_infop info_ptr;

	if ((f = fopen(filename, "wb")) == NULL)
		return errno;

	if ((png_ptr = png_create_write_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL)) == NULL)
		return TTIP_LIBPNG_INIT_FAILED;

	if ((info_ptr = png_create_info_struct(png_ptr)) == NULL) {
		png_destroy_write_struct(&png_ptr, NULL);
		return TTIP_LIBPNG_INIT_FAILED;
	}

	if (setjmp(png_jmpbuf(png_ptr))) {
		png_destroy_write_struct(&png_ptr, &info_ptr);
		fclose(f);
		unlink(filename);
		return TTIP_LIBPNG_ERROR;
	}

	png_init_io(png_ptr, f);
	png_set_compression_level(png_ptr, level);

	int png_color_type = 0;
	switch (source->format) {
	case TTIP_GRAY: png_color_type = PNG_COLOR_TYPE_GRAY; break;
	case TTIP_GRAY_ALPHA: png_color_type = PNG_COLOR_TYPE_GRAY_ALPHA; break;
	case TTIP_RGB: png_color_type = PNG_COLOR_TYPE_RGB; break;
	case TTIP_RGB_ALPHA: png_color_type = PNG_COLOR_TYPE_RGB_ALPHA; break;
	}

	png_set_IHDR(png_ptr, info_ptr, source->width, source->height, 8, png_color_type, PNG_INTERLACE_NONE, PNG_COMPRESSION_TYPE_BASE, PNG_FILTER_TYPE_BASE);

	png_write_info(png_ptr, info_ptr);

	png_bytep row;
	for (row = source->data; row < source->data + source->stride * source->height; row += source->stride)
		png_write_row(png_ptr, row);

	png_write_end(png_ptr, NULL);

	png_destroy_write_struct(&png_ptr, &info_ptr);
	fclose(f);

	return TTIP_OK;
#else
	return TTIP_NOT_COMPILED_IN;
#endif
}

ttip_result_t ttip_loadpng(ttip_image_t* output, const char* filename) {
#if defined(WITH_PNG)
	FILE* f;
	png_structp png_ptr;
	png_infop info_ptr;
	struct ttip_image* destination = NULL;

	if ((f = fopen(filename, "rb")) == NULL)
		return errno;

	if ((png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL)) == NULL)
		return TTIP_LIBPNG_INIT_FAILED;

	if ((info_ptr = png_create_info_struct(png_ptr)) == NULL) {
		png_destroy_read_struct(&png_ptr, NULL, NULL);
		return TTIP_LIBPNG_INIT_FAILED;
	}

	if (setjmp(png_jmpbuf(png_ptr))) {
		png_destroy_read_struct(&png_ptr, &info_ptr, NULL);
		fclose(f);
		if (destination != NULL)
			ttip_destroy(&destination);
		return TTIP_LIBPNG_ERROR;
	}

	png_init_io(png_ptr, f);

	png_read_info(png_ptr, info_ptr);

	png_uint_32 width, height;
	int bit_depth, color_type, interlace_method;

	png_get_IHDR(png_ptr, info_ptr, &width, &height, &bit_depth, &color_type, &interlace_method, 0, 0);

	if (color_type == PNG_COLOR_TYPE_PALETTE)
		png_set_palette_to_rgb(png_ptr);

	png_read_update_info(png_ptr, info_ptr);
	png_get_IHDR(png_ptr, info_ptr, &width, &height, &bit_depth, &color_type, &interlace_method, 0, 0);

	ttip_format_t format = 0;
	switch (color_type) {
	case PNG_COLOR_TYPE_GRAY: format = TTIP_GRAY; break;
	case PNG_COLOR_TYPE_GRAY_ALPHA: format = TTIP_GRAY_ALPHA; break;
	case PNG_COLOR_TYPE_RGB: format = TTIP_RGB; break;
	case PNG_COLOR_TYPE_RGB_ALPHA: format = TTIP_RGB_ALPHA; break;
	}

	if (format == 0 || interlace_method != 0 || ttip_create(&destination, width, height, format) != TTIP_OK) {
		png_destroy_read_struct(&png_ptr, &info_ptr, NULL);
		fclose(f);
		return TTIP_IMAGE_FORMAT_NOT_SUPPORTED;
	}

	assert(destination->stride >= (int)png_get_rowbytes(png_ptr, info_ptr));

	png_bytep row;
	for (row = destination->data; row < destination->data + destination->stride * destination->height; row += destination->stride)
		png_read_row(png_ptr, row, NULL);

    png_read_end(png_ptr, NULL);
	png_destroy_read_struct(&png_ptr, &info_ptr, NULL);
	fclose(f);

	*output = destination;

	return TTIP_OK;
#else
	return TTIP_NOT_COMPILED_IN;
#endif
}
