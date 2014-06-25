#include <stdbool.h>
#include <time.h>

#define DEF_MONFILE "/tmp/rtd/rtd.data"
#define DEF_TMPDIR "/tmp/rtd/"
#define DEF_HEIGHT 512
#define DEF_WIDTH 670
#define DEF_AVG false

struct parsed_options {
	char *monfile;
	char *tmpdir;
	int height;
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
	
	test
};

int parse_options(struct parsed_options *, int, char **);
void init_parsed_options(struct parsed_options *);

