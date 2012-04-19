/*
 * Copyright (c) 2011-2012 Dmitry Marakasov <amdmi3@amdmi3.ru>
 * All rights reserved.
 *
 * See https://github.com/AMDmi3/testing.h for updates, bug reports,
 * forks etc.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE AUTHOR OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 */

#ifndef TESTING_H_INCLUDED
#define TESTING_H_INCLUDED

#include <stdio.h>

// Define NO_TEST_COLOR before including testing.h to disable colors
#ifndef TEST_COLOR
#	define TEST_COLOR
#endif

#ifdef TEST_COLOR
#	define PASSED "\e[0;32mPASSED:\e[0m "
#	define FAILED "\e[1;31mFAILED:\e[0m "
#else
#	define PASSED "PASSED: "
#	define FAILED "FAILED: "
#endif

// Test begin/end
#define BEGIN_TEST() int main() { int num_failing_tests_ = 0;

#define END_TEST() if (num_failing_tests_ > 0) fprintf(stderr, "%d failures\n", num_failing_tests_); return num_failing_tests_; }

#define EXPECT_TRUE(expr) { \
		if (!(expr)) { \
			fprintf(stderr, FAILED "\"" #expr "\" returned false while expected true\n"); \
			++num_failing_tests_; \
		} else { \
			fprintf(stderr, PASSED "\"" #expr "\" returned true, as expected\n"); \
		} \
	}

#define EXPECT_FALSE(expr) { \
		if ((expr)) { \
			fprintf(stderr, FAILED "\"" #expr "\" returned true while expected false\n"); \
			++num_failing_tests_; \
		} else { \
			fprintf(stderr, PASSED "\"" #expr "\" returned false, as expected\n"); \
		} \
	}

#define EXPECT_INT(expr, expected) { \
		int result = (expr); \
		if (result != expected) { \
			fprintf(stderr, FAILED #expr " returned %d, while expected %d\n", result, expected); \
			++num_failing_tests_; \
		} else { \
			fprintf(stderr, PASSED #expr " == %d\n", expected); \
		} \
	}

#define EXPECT_EQUAL(expr, expected) { \
		if ((expr) != (expected)) { \
			fprintf(stderr, FAILED #expr " != " #expected "\n"); \
			++num_failing_tests_; \
		} else { \
			fprintf(stderr, PASSED #expr " == " #expected "\n"); \
		} \
	}

#endif // TESTING_H_INCLUDED
