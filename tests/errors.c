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

#include <string.h>

#include <ttip.h>

#include "testing.h"

BEGIN_TEST()
	ttip_result_t from = 0;
	ttip_result_t to = TTIP_LAST_ERROR;

	if (to < from) {
		ttip_result_t tmp = from;
		from = to;
		to = tmp;
	}

	/* check that we have error strings for all ttip errors
	 * for unknown codes, strerror() returns
	 *   Linux: "Unknown error %u"
	 * FreeBSD: "Unknown error: %d"
	 * so we look for intersection of these */
	for (ttip_result_t res = from; res <= to; ++res)
		EXPECT_TRUE(strstr(ttip_strerror(res), "Unknown error") == 0);

	/* just print out all strings */
	printf("libttip errors listing:\n");
	for (ttip_result_t res = from; res <= to; ++res)
		if (res != 0)
			printf("%3d: %s\n", res, ttip_strerror(res));
END_TEST()
