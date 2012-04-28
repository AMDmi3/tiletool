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

#include <stdlib.h>

#include "process.h"

#include "testing.h"

BEGIN_TEST()
#ifdef HAVE_FORK
	EXPECT_TRUE(get_nchilds() == 0);

	if (fork_child())
		exit(0);

	EXPECT_TRUE(get_nchilds() == 1);

	EXPECT_FALSE(wait_child());

	EXPECT_TRUE(get_nchilds() == 0);

	if (fork_child())
		exit(1);

	EXPECT_TRUE(wait_child());

	if (fork_child())
		exit(1);
	if (fork_child())
		exit(1);
	if (fork_child())
		exit(1);

	EXPECT_TRUE(get_nchilds() == 3);

	EXPECT_TRUE(wait_child());
	EXPECT_TRUE(wait_all_childs() == 2);
#endif
END_TEST()
