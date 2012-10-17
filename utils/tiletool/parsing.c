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

#include <string.h>
#include <stdlib.h>

#include "parsing.h"

int parse_double(const char* start, const char* end, double* out) {
	char *endptr;
	double val = strtod(start, &endptr);

	if (endptr != end)
		return 0;

	*out = val;
	return 1;
}

int parse_unsigned(const char* start, const char* end, int* out) {
	char *endptr;
	unsigned long val = strtoul(start, &endptr, 10);

	if (endptr != end)
		return 0;

	*out = val;
	return 1;
}

int parse_unsigned_range(const char* start, const char* end, int* out1, int* out2) {
	char *endptr;
	char *dash = strchr(start, '-');

	if (dash > end)
		dash = NULL;

	if (dash == NULL) {
		int ret = parse_unsigned(start, end, out1);
		if (ret)
			*out2 = *out1;
		return ret;
	}

	if (dash == end - 1)
		return 0;

	unsigned long val1 = strtoul(start, &endptr, 10);
	if (endptr != dash)
		return 0;

	unsigned long val2 = strtoul(dash + 1, &endptr, 10);
	if (endptr != end)
		return 0;

	*out1 = val1;
	*out2 = val2;

	return 1;
}

int parse_split(const char* start, char ch, const char** dividers, int parts) {
	int i = 0;
	const char* curr = start - 1;

	while (curr && i < parts) {
		if ((curr = dividers[i] = strchr(curr + 1, ch)))
			i++;
	}

	return i;
}
