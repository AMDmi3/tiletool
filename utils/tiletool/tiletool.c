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
#include <getopt.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

#include <ttip.h>

#include "bounds.h"
#include "parsing.h"
#include "emptytile.h"
#include "process.h"
#include "paths.h"

#define MAX_INPUTS 128
#define MAX_OVERLAYS 128

/* properties based on options */
int g_min_input_zoom = -1;
int g_max_input_zoom = -1;
int g_min_output_zoom = -1;
int g_max_output_zoom = -1;

int g_num_jobs = 0;

Bounds g_input_bounds = BOUNDS_FULL_INITIALIZER;
Bounds g_output_bounds = BOUNDS_FULL_INITIALIZER;

const char* g_inputs[MAX_INPUTS];
unsigned int g_ninputs;
const char* g_output = NULL;

const char* g_overlays[MAX_OVERLAYS];
unsigned int g_noverlays = 0;

unsigned int g_pngcompression = 2;

const char* g_postcmd = NULL;

int g_verbose = 0;

/* other global data */
static struct option longopts[] = {
	{ "intput-bounds", required_argument, NULL, 'b' },
	{ "output-bounds", required_argument, NULL, 'B' },
	{ "input-zoom",    required_argument, NULL, 'z' },
	{ "output-zoom",   required_argument, NULL, 'Z' },
	{ "empty-tile",    required_argument, NULL, 'e' },
	{ "jobs",          required_argument, NULL, 'j' },
	{ "overlay",       required_argument, NULL, 'l' },
	{ "postcmd",       required_argument, NULL, 'c' },
	{ "input",         required_argument, NULL, 'i' },
	{ "output",        required_argument, NULL, 'o' },
	{ "help",          no_argument,       NULL, 'h' },
	{ "verbose",       no_argument,       NULL, 'v' },
	{ NULL,            0,                 NULL, 0 },
};

const char* g_progname;

int g_totaltiles = 0;
int g_errortiles = 0;

/* main code */
int process_output(int x, int y, int zoom, ttip_image_t tile) {
	/* ensure we have a tile */
	if (tile == NULL)
		tile = spawn_empty_tile();

	/* main processing */
	int had_error = 0;
	ttip_result_t res;

	/* process overlays */
	for (unsigned int i = 0; i < g_noverlays; ++i) {
		char* overlay_path = get_tile_path(g_overlays[i], x, y, zoom, ".png");
		ttip_image_t overlay = NULL;
		if ((res = ttip_loadpng(&overlay, overlay_path)) == TTIP_OK) {
			ttip_image_t temp;
			if ((res = ttip_maskblend(&temp, tile, overlay)) == TTIP_OK) {
				ttip_destroy(&tile);
				tile = temp;
			} else {
				warnx("Could not blend overlay %s: %s", overlay_path, ttip_strerror(res));
				had_error = 1;
			}
			ttip_destroy(&overlay);
		} else if (res != ENOENT) {
			warnx("Could not open overlay tile %s: %s", overlay_path, ttip_strerror(res));
			had_error = 1;
		}
		/* else -> overlay tile didn't exist */
	}

	/* save result */
	char* output_path = get_tile_path(g_output, x, y, zoom, ".png");

	create_directories(output_path);

	if ((res = ttip_savepng(tile, output_path, g_pngcompression)) != TTIP_OK) {
		warnx("Could not save output tile %s: %s", output_path, ttip_strerror(res));
		had_error = 1;
	}

	/* cleanup */
	ttip_destroy(&tile);

	if (g_postcmd) {
		char buffer[strlen(g_postcmd) + strlen(output_path) + 2];
		strcpy(buffer, g_postcmd);
		strcat(buffer, " ");
		strcat(buffer, output_path);
		if (system(buffer) != 0) {
			warnx("Postcmd `%s` failed", buffer);
			had_error = 1;
		}
	}

	return !had_error;
}

ttip_image_t process_tile(int x, int y, int zoom) {
	if (g_verbose)
		fprintf(stderr, "Entering %d/%d/%d...\n", zoom, x, y);

	ttip_result_t res;

	/* higher-level childs; { topleft, topright, bottomleft, bottomright } */
	ttip_image_t childs[4] = { NULL, NULL, NULL, NULL };

	/* current tile */
	ttip_image_t current = NULL;

	/* bounds */
	if (!is_tile_in_bounds(x, y, zoom, &g_input_bounds))
		return NULL;

	/* load current tile, if needed and available */
	if (g_min_input_zoom <= zoom && zoom <= g_max_input_zoom) {
		for (unsigned int i = 0; i < g_ninputs; ++i) {
			char* input_path = get_tile_path(g_inputs[i], x, y, zoom, ".png");
			if ((res = ttip_loadpng(&current, input_path)) != TTIP_OK && res != ENOENT)
				errx(1, "Could not load source tile %s: %s", input_path, ttip_strerror(res));
			if (res == TTIP_OK)
				break; /* input found */
		}
	}

	/* descend to childs only if we need to do input or output on them */
	int have_childs = 0;
	if ((current == NULL && zoom < g_max_input_zoom) || zoom < g_max_output_zoom)
		for (int i = 0; i < 4; ++i) {
			childs[i] = process_tile(x * 2 + (i & 1), y * 2 + !!(i & 2), zoom + 1);
			have_childs += childs[i] != NULL;
		}

	if (current == NULL && have_childs > 0) {
		/* if we have partial child data, fill missing tiles */
		for (int i = 0; i < 4; ++i)
			if (!childs[i])
				childs[i] = spawn_empty_tile();

		/* and combine current tile */
		if ((res = ttip_downsample2x2(&current, childs[0], childs[1], childs[2], childs[3])) != TTIP_OK)
			errx(1, "Error downsampling tile: %s", ttip_strerror(res));
	}

	for (int i = 0; i < 4; ++i)
		ttip_destroy(&childs[i]);

	/* if output not needed, just return the tile */
	if (zoom < g_min_output_zoom || zoom > g_max_output_zoom || !is_tile_in_bounds(x, y, zoom, &g_output_bounds))
		return current;

	/* output processing */
	g_totaltiles++;
	if (g_verbose)
		fprintf(stderr, "Processing %d/%d/%d...\n", zoom, x, y);
#ifdef HAVE_FORK
	if (g_num_jobs == 0) {
#endif
		/* single process case; since we need unmodified tile here later, clone it */
		ttip_image_t tmp = NULL;
		if (current)
			if ((res = ttip_clone(&tmp, current)) != TTIP_OK)
				errx(1, "Could not clone tile: %s", ttip_strerror(res));
		if (!process_output(x, y, zoom, tmp))
			g_errortiles++;
#ifdef HAVE_FORK
	} else {
		/* if there are enough jobs running, wait for one to finish */
		while (get_nchilds() >= g_num_jobs)
			g_errortiles += wait_child();

		/* and run a new job */
		if (fork_child()) {
			exit(process_output(x, y, zoom, current) ? 0 : 1);
		}
	}
#endif

	/* if output is not needed already, destroy data here and pass NULL up */
	if (zoom <= g_min_output_zoom) {
		ttip_destroy(&current);
		return NULL;
	}

	return current;
}

void usage(int ecode) {
	/*               [ 75 chars ===============================================================] */
	fprintf(stderr, "usage: %s [options] [-z or -Z] [-i input tile tree] [-o output tile tree]\n\n", g_progname);
	fprintf(stderr, "    -0..-9               set png compression level\n");
	fprintf(stderr, "    -b, --input-bounds   limit processing with bounding box.\n");
	fprintf(stderr, "                         Possible formats:\n");
	fprintf(stderr, "                         zoom/x/y or zoom/minx-maxx/miny-maxy (tile ids)\n");
	fprintf(stderr, "                         left,bottom,right,top (bbox with geo coords)\n");
	fprintf(stderr, "    -B, --output-bounds  limit output with bounding box\n");
	fprintf(stderr, "    -z, --input-zoom     zoom range of input tiles\n");
	fprintf(stderr, "    -Z, --output-zoom    zoom range of output tiles\n");
	fprintf(stderr, "    -e, --empty-tile     empty tile filler\n");
#ifdef HAVE_FORK
	fprintf(stderr, "    -j, --jobs           number of processes to spawn for tile processing\n");
#endif
	fprintf(stderr, "    -i, --input          specify input tileset\n");
	fprintf(stderr, "    -o, --output         specify place for output tileset\n");
	fprintf(stderr, "    -l, --overlay        add overlay tileset\n");
	fprintf(stderr, "    -c, --postcmd        add command to postprocess each generated tile\n");
	fprintf(stderr, "    -v, --verbose        increase verbosity\n");
	fprintf(stderr, "    -h, --help           display this help\n");
	exit(ecode);
}

int main(int argc, char** argv) {
	g_progname = argv[0];

	/* parse arguments */
	int ch;
	while ((ch = getopt_long(argc, argv, "b:B:z:Z:e:j:i:o:l:c:0123456789hv", longopts, NULL)) != -1) {
		switch (ch) {
		case 'b':
			if (!parse_bounds(optarg, &g_input_bounds)) {
				warnx("Cannot parse input bounds\n");
				usage(1);
			}
			break;
		case 'B':
			if (!parse_bounds(optarg, &g_output_bounds)) {
				warnx("Cannot parse output bounds\n");
				usage(1);
			}
			break;
		case 'z':
			if (!parse_unsigned_range(optarg, optarg + strlen(optarg), &g_min_input_zoom, &g_max_input_zoom)) {
				warnx("Cannot parse input zoom\n");
				usage(1);
			}
			break;
		case 'Z':
			if (!parse_unsigned_range(optarg, optarg + strlen(optarg), &g_min_output_zoom, &g_max_output_zoom)) {
				warnx("Cannot parse output zoom\n");
				usage(1);
			}
			break;
		case 'e':
			init_empty_tile(optarg);
			atexit(cleanup_empty_tile);
			break;
#ifdef HAVE_FORK
		case 'j':
			if (!parse_unsigned(optarg, optarg + strlen(optarg), &g_num_jobs)) {
				warnx("Cannot parse number of jobs\n");
				usage(1);
			}
			break;
#endif
		case 'i':
			if (g_ninputs < MAX_INPUTS) {
				g_inputs[g_ninputs++] = optarg;
			} else {
				warnx("Too many inputs specified\n");
				usage(1);
			}
			break;
		case 'o':
			g_output = optarg;
			break;
		case 'l':
			if (g_noverlays < MAX_OVERLAYS) {
				g_overlays[g_noverlays++] = optarg;
			} else {
				warnx("Too many overlays specified\n");
				usage(1);
			}
			break;
		case 'c':
			g_postcmd = optarg;
			break;
		case '0': case '1': case '2': case '3': case '4':
		case '5': case '6': case '7': case '8': case '9':
			g_pngcompression = ch - '0';
			break;
		case 'h':
			usage(0);
			break;
		case 'v':
			g_verbose = 1;
			break;
		default:
			usage(1);
			break;
		}
	}

	/* defaults, checks */
	if (g_output == NULL) {
		warnx("No output specified\n");
		usage(1);
	}

	if (g_ninputs == 0) {
		warnx("No input(s) specified\n");
		usage(1);
	}

	if (g_max_output_zoom < 0 && g_min_input_zoom > 0) {
		g_max_output_zoom = g_min_input_zoom - 1;
		g_min_output_zoom = 0;
	}

	if (g_min_input_zoom < 0 && g_max_output_zoom >= 0)
		g_min_input_zoom = g_max_input_zoom = g_max_output_zoom + 1;

	int bad_options = 0;
	if (g_min_input_zoom < 0 || g_max_input_zoom < 0) {
		warnx("Unspecified input zoom range\n");
		bad_options++;
	}

	if (g_min_output_zoom < 0 || g_max_output_zoom < 0) {
		warnx("Unspecified output zoom range\n");
		bad_options++;
	}

	if (bad_options)
		usage(1);

	if (!has_empty_tile())
		fprintf(stderr, "Warning: empty tile not specified, process will fail if input tileset is incomplete\n");

	/* run processing */
	if (g_verbose) {
		for (unsigned int i = 0; i < g_ninputs; ++i)
			fprintf(stderr, "  Input: %s, zooms %d-%d\n", g_inputs[i], g_min_input_zoom, g_max_input_zoom);
		fprintf(stderr, " Output: %s, zooms %d-%d\n", g_output, g_min_output_zoom, g_max_output_zoom);
		for (unsigned int i = 0; i < g_noverlays; ++i)
			fprintf(stderr, "Overlay: %s\n", g_overlays[i]);
	}

	/* run processing */
	process_tile(0, 0, 0);

#ifdef HAVE_FORK
	if (g_num_jobs > 0)
		g_errortiles += wait_all_childs();
#endif

	if (g_verbose)
		fprintf(stderr, "Tiles processed: %d, errors: %d\n", g_totaltiles, g_errortiles);

	return g_errortiles != 0;
}
