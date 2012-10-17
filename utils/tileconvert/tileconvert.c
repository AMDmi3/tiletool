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

#include <err.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <getopt.h>
#include <sys/time.h>

#include <ttip.h>

/* simple tile gc */
ttip_image_t tiles_to_gc[10];
int ntiles_to_gc = 0;

void gc_tile(ttip_image_t tile) {
	tiles_to_gc[ntiles_to_gc++] = tile;
	if (ntiles_to_gc == sizeof(tiles_to_gc)/sizeof(tiles_to_gc[0]))
		errx(1, "tile garbage collector overflow");
}

void run_gc() {
	int i;
	for (i = 0; i < ntiles_to_gc; ++i)
		ttip_destroy(&tiles_to_gc[i]);
}

/* benchmarking */
void start_benchmark(struct timeval* start) {
	gettimeofday(start, NULL);
}

void end_benchmark(struct timeval* begin, const char* function, int iterations) {
	if (iterations == 1)
		return;

	struct timeval end;
	gettimeofday(&end, NULL);

	float time = (float)(end.tv_sec - begin->tv_sec) + (float)(end.tv_usec - begin->tv_usec)/1000000.0f;
	fprintf(stderr, "%s: %d iterations, %.2f seconds, %.2f iters/sec, %.2f msecs/iter\n",
		function, iterations, time, iterations/time, 1000000.0*time/iterations);
}

/* usage information */
void usage(const char* progname) {
	fprintf(stderr, "Usage: %s [-h] [-b iterations] <command> <arguments>\n\n", progname);

	/* 75 charwidth  ===========================================================================\n */
	fprintf(stderr, "Commands:\n");
	fprintf(stderr, "    loadpng <source> <destination>\n");
	fprintf(stderr, "    savepng <source> <destination>\n");
	fprintf(stderr, "    clone <source> <destination>\n");
	fprintf(stderr, "        just repack an image (but benchmarks different functions)\n\n");
	fprintf(stderr, "    desaturate <source> <destination>\n");
	fprintf(stderr, "        convert RGB image to grayscale\n\n");
	fprintf(stderr, "    maskblend <background> <overlay> <destination>\n");
	fprintf(stderr, "        blend overlay image with alpha channel with background\n\n");
	fprintf(stderr, "    downsample2x2 <topleft> <topright> <bottomleft> <bottomright> <dest>\n");
	fprintf(stderr, "        downsample 4 tiles 2x and fit them on a single tile\n\n");
	fprintf(stderr, "    threshold <value> <source> <destination>\n");
	fprintf(stderr, "        convert to monochrome comparing pixel intensity with threshold\n\n");
}

int main(int argc, char **argv) {
	const char* progname = argv[0];

	/* arguments */
	int ch;
	int iterations = 1; /* >1 means benchmarking mode */
	while ((ch = getopt(argc, argv, "hb:")) != -1) {
		switch (ch) {
		case 'b':
			iterations = (int)strtoul(optarg, NULL, 10);
			if (iterations < 1)
				iterations = 1;
			break;
		case 'h':
			usage(progname);
			return 0;
		default:
			usage(progname);
			return 1;
		}
	}

	argc -= optind;
	argv += optind;

	if (argc < 1) {
		usage(progname);
		return 1;
	}

	atexit(run_gc);

	/* process commands */
	int ret, pass;
	struct timeval begin;
	if (strcmp(argv[0], "loadpng") == 0 && argc == 3) {
		ttip_image_t source;

		start_benchmark(&begin);
		for (pass = 1; pass <= iterations; ++pass) {
			if ((ret = ttip_loadpng(&source, argv[1])) != TTIP_OK)
				errx(1, "ttip_loadpng: %s", ttip_strerror(ret));
			if (pass != iterations)
				ttip_destroy(&source);
		}
		end_benchmark(&begin, "ttip_loadpng", iterations);
		gc_tile(source);

		if ((ret = ttip_savepng(source, argv[2], 6)) != TTIP_OK)
			errx(1, "ttip_savepng: %s", ttip_strerror(ret));
	} else if (strcmp(argv[0], "savepng") == 0 && argc == 3) {
		ttip_image_t source;

		if ((ret = ttip_loadpng(&source, argv[1])) != TTIP_OK)
			errx(1, "ttip_loadpng: %s", ttip_strerror(ret));
		gc_tile(source);

		start_benchmark(&begin);
		for (pass = 1; pass <= iterations; ++pass) {
			if ((ret = ttip_savepng(source, argv[2], 6)) != TTIP_OK)
				errx(1, "ttip_savepng: %s", ttip_strerror(ret));
		}
		end_benchmark(&begin, "ttip_savepng", iterations);
	} else if (strcmp(argv[0], "clone") == 0 && argc == 3) {
		ttip_image_t source, destination;

		if ((ret = ttip_loadpng(&source, argv[1])) != TTIP_OK)
			errx(1, "ttip_loadpng: %s", ttip_strerror(ret));
		gc_tile(source);

		start_benchmark(&begin);
		for (pass = 1; pass <= iterations; ++pass) {
			if ((ret = ttip_clone(&destination, source)) != TTIP_OK)
				errx(1, "ttip_clone: %s", ttip_strerror(ret));
			if (pass != iterations)
				ttip_destroy(&destination);
		}
		end_benchmark(&begin, "ttip_clone", iterations);
		gc_tile(destination);

		if ((ret = ttip_savepng(destination, argv[2], 6)) != TTIP_OK)
			errx(1, "ttip_savepng: %s", ttip_strerror(ret));
	} else if (strcmp(argv[0], "desaturate") == 0 && argc == 3) {
		ttip_image_t source, destination;

		if ((ret = ttip_loadpng(&source, argv[1])) != TTIP_OK)
			errx(1, "ttip_loadpng: %s", ttip_strerror(ret));
		gc_tile(source);

		start_benchmark(&begin);
		for (pass = 1; pass <= iterations; ++pass) {
			if ((ret = ttip_desaturate(&destination, source)) != TTIP_OK)
				errx(1, "ttip_desaturate: %s", ttip_strerror(ret));
			if (pass != iterations)
				ttip_destroy(&destination);
		}
		end_benchmark(&begin, "ttip_desaturate", iterations);
		gc_tile(destination);

		if ((ret = ttip_savepng(destination, argv[2], 6)) != TTIP_OK)
			errx(1, "ttip_savepng: %s", ttip_strerror(ret));
	} else if (strcmp(argv[0], "maskblend") == 0 && argc == 4) {
		ttip_image_t background, overlay, destination;

		if ((ret = ttip_loadpng(&background, argv[1])) != TTIP_OK)
			errx(1, "ttip_loadpng: %s", ttip_strerror(ret));
		gc_tile(background);

		if ((ret = ttip_loadpng(&overlay, argv[2])) != TTIP_OK)
			errx(1, "ttip_loadpng: %s", ttip_strerror(ret));
		gc_tile(overlay);

		start_benchmark(&begin);
		for (pass = 1; pass <= iterations; ++pass) {
			if ((ret = ttip_maskblend(&destination, background, overlay)) != TTIP_OK)
				errx(1, "ttip_maskblend: %s", ttip_strerror(ret));
			if (pass != iterations)
				ttip_destroy(&destination);
		}
		end_benchmark(&begin, "ttip_maskblend", iterations);
		gc_tile(destination);

		if ((ret = ttip_savepng(destination, argv[3], 6)) != TTIP_OK)
			errx(1, "ttip_savepng: %s", ttip_strerror(ret));
	} else if (strcmp(argv[0], "downsample2x2") == 0 && argc == 6) {
		ttip_image_t tl, tr, bl, br, destination;

		if ((ret = ttip_loadpng(&tl, argv[1])) != TTIP_OK)
			errx(1, "ttip_loadpng: %s", ttip_strerror(ret));
		gc_tile(tl);

		if ((ret = ttip_loadpng(&tr, argv[2])) != TTIP_OK)
			errx(1, "ttip_loadpng: %s", ttip_strerror(ret));
		gc_tile(tr);

		if ((ret = ttip_loadpng(&bl, argv[3])) != TTIP_OK)
			errx(1, "ttip_loadpng: %s", ttip_strerror(ret));
		gc_tile(bl);

		if ((ret = ttip_loadpng(&br, argv[4])) != TTIP_OK)
			errx(1, "ttip_loadpng: %s", ttip_strerror(ret));
		gc_tile(br);

		start_benchmark(&begin);
		for (pass = 1; pass <= iterations; ++pass) {
			if ((ret = ttip_downsample2x2(&destination, tl, tr, bl, br)) != TTIP_OK)
				errx(1, "ttip_downsample2x2: %s", ttip_strerror(ret));
			if (pass != iterations)
				ttip_destroy(&destination);
		}
		end_benchmark(&begin, "ttip_downsample2x2", iterations);
		gc_tile(destination);

		if ((ret = ttip_savepng(destination, argv[5], 6)) != TTIP_OK)
			errx(1, "ttip_savepng: %s", ttip_strerror(ret));
	} else if (strcmp(argv[0], "threshold") == 0 && argc == 4) {
		int th = strtoul(argv[1], NULL, 10);
		if (th < 0)
			th = 0;
		if (th > 255)
			th = 255;

		ttip_image_t source, destination;

		if ((ret = ttip_loadpng(&source, argv[2])) != TTIP_OK)
			errx(1, "ttip_loadpng: %s", ttip_strerror(ret));
		gc_tile(source);

		start_benchmark(&begin);
		for (pass = 1; pass <= iterations; ++pass) {
			if ((ret = ttip_threshold(&destination, source, th)) != TTIP_OK)
				errx(1, "ttip_threshold: %s", ttip_strerror(ret));
			if (pass != iterations)
				ttip_destroy(&destination);
		}
		end_benchmark(&begin, "ttip_threshold", iterations);
		gc_tile(destination);

		if ((ret = ttip_savepng(destination, argv[3], 6)) != TTIP_OK)
			errx(1, "ttip_savepng: %s", ttip_strerror(ret));
	} else {
		usage(progname);
		return 1;
	}

	return 0;
}
