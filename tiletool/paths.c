/*
 * Copyright (C) 2012 Dmitry Marakasov
 *
 * This file is part of tiletool.
 *
 * tiletool is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * tiletool is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with tiletool.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef PATHS_H
#define PATHS_H

#include <sys/stat.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>

char* get_tile_path_r(char* buffer, size_t size, const char* prefix, int x, int y, int zoom, const char* suffix) {
	if (snprintf(buffer, size, "%s/%d/%d/%d%s", prefix, zoom, x, y, suffix) >= (int)size)
		return NULL;

	return buffer;
}

char* get_tile_path(const char* prefix, int x, int y, int zoom, const char* suffix) {
	static char buffer[FILENAME_MAX];
	return get_tile_path_r(buffer, FILENAME_MAX, prefix, x, y, zoom, suffix);
}

int create_directories(char* path) {
	char* lastslash = strrchr(path, '/');
	if (lastslash == NULL)
		return 0;

	*lastslash = '\0';

	int ret = mkdir(path, 0777);
	if (ret == -1 && errno == ENOENT) {
		if ((ret = create_directories(path)) == 0)
			ret = mkdir(path, 0777);
	}

	*lastslash = '/';

	if (ret == -1 && errno == EEXIST)
		ret = 0;

	return ret;
}

#endif
