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

#ifdef HAVE_FORK

#include <err.h>
#include <errno.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <unistd.h>

#include "process.h"

static int forked_childs = 0;

int get_nchilds() {
	return forked_childs;
}

int wait_child() {
	int retcode;
	int status;

	/* definitely wait for one child to be raclaimed, ignoring signal interrupts */
	while ((retcode = wait(&status)) == -1 && errno == EINTR) { /* empty */ }

	if (retcode == -1)
		err(1, "wait");

	forked_childs--;

	return status != 0;
}

int wait_all_childs() {
	int failures = 0;
	while (forked_childs > 0)
		failures += wait_child();
	return failures;
}

int fork_child() {
	pid_t pid = fork();
	if (pid == -1)
		err(1, "fork");
	else if (pid == 0)
		return 1;

	forked_childs++;
	return 0;
}

#endif
