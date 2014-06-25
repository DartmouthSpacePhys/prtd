#include <stdbool.h>

struct parsed_options {
	char *monfile;
	char *tmpdir;
	int fftn;
	int width;
	bool avg;
};

struct header_info {
	char site_id[12];
	int num_channels;
	char channel_flags;
	unsigned int num_samples;
	unsigned int num_read;
	float sample_frequency;
	float time_between_acquisitions;
	int byte_packing;
	time_t start_time;
	struct timeval start_timeval;
	float code_version;
};