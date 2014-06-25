#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "dynprtd.h"

/* Command-line option parsing */

int parse_options(struct parsed_options *options, int argc, char *argv[]) {
	int c;

	while (-1 != (c = getopt(argc, argv, "c:m:t:a:h"))) {
		switch (c) {
		case 'm':
			options->monfile = optarg;
			break;
		case 't':
			options->tmpdir = optarg;
			break;
		case 'n':
			options->height = (int) strtol(optarg, NULL, 0);
			break;
		case 'w':
			options->width = (int) strtol(optarg, NULL, 0);
			break;
		case 'a':
			options->avg = true;
			break;
		case 'h':
		default:
			fprintf(stderr, "cmd Options:\n");
			fprintf(stderr, "\t-m <file>\tMonitor file [%s]\n", DEF_MONFILE);
			fprintf(stderr, "\t-t <dir>\tTemporary directory [%s]\n", DEF_TMPDIR);
			fprintf(stderr, "\n-n <#>\tImage height/FFT length [%i]\n", DEF_HEIGHT);
			fprintf(stderr, "\n-w <#>\tImage width [%i]\n", DEF_WIDTH);
			fprintf(stderr, "\n-a \tEnable averaging [%s]\n", DEF_AVG ? "enabled" : "disabled");
			fprintf(stderr, "\n");
			fprintf(stderr, "When -a is specified, as much data from the monitor"); 
			fprintf(stderr, "file as possible will be used to build averages.\n");
			exit(1);
		}
	}
	
	// Validate numeric options
	if (options->height <= 0) {
		fprintf(stderr, "Image height must be greater than zero!\n");
		exit(1);
	}
	if (options->width <=0) {
		fprintf(stderr, "Image width must be greater than zero!\n");
		exit(1);
	}

	return argc;
}

void init_parsed_options(struct parsed_options *options) {
	memset(options, 0, sizeof(struct parsed_options));
	options->monfile = DEF_MONFILE;
	options->tmpdir = DEF_TMPDIR;
	options->height = DEF_HEIGHT;
	options->width = DEF_WIDTH;
	options->avg = DEF_AVG;
}

