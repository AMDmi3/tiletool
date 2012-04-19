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

#include <math.h>
#include <string.h>

#include "parsing.h"

#include "testing.h"

#define EPS 0.00000001

BEGIN_TEST()
	{
		int val;
		const char* str = "1,2,3";

		EXPECT_TRUE(parse_unsigned(str, str + 1, &val) && val == 1);
		EXPECT_TRUE(parse_unsigned(str + 2, str + 3, &val) && val == 2);
		EXPECT_TRUE(parse_unsigned(str + 4, str + 5, &val) && val == 3);
	}

	{
		int val = 0;
		const char* str = "4abc";

		EXPECT_FALSE(parse_unsigned(str, str + 4, &val));
		EXPECT_TRUE(val == 0);
	}

	{
		double val;
		const char* str = "1.2345abc";

		EXPECT_TRUE(parse_double(str, str + 6, &val));
	   	EXPECT_TRUE(fabs(val - 1.2345) < EPS);
	}

	{
		int a, b;
		const char* str1 = "123-234";
		const char* str2 = "123";
		const char* str3 = "123-";
		const char* str4 = "-123";

		EXPECT_TRUE(parse_unsigned_range(str1, str1 + strlen(str1), &a, &b));
		EXPECT_TRUE(a == 123);
		EXPECT_TRUE(b == 234);

		EXPECT_TRUE(parse_unsigned_range(str2, str2 + strlen(str2), &a, &b));
		EXPECT_TRUE(a == 123);
		EXPECT_TRUE(b == 123);

		EXPECT_FALSE(parse_unsigned_range(str3, str3 + strlen(str3), &a, &b));
		EXPECT_FALSE(parse_unsigned_range(str4, str4 + strlen(str4), &a, &b));
	}

	{
		const char* str = "aaa/bbb/ccc";
		const char* arr[2];

		EXPECT_TRUE(parse_split(str, '/', arr, 1) == 1);
		EXPECT_TRUE(parse_split(str, '/', arr, 2) == 2);
		EXPECT_TRUE(parse_split(str, '/', arr, 3) == 2);
		EXPECT_TRUE(arr[0] == str + 3);
		EXPECT_TRUE(arr[1] == str + 7);
	}

END_TEST()
