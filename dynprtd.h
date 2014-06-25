#include <stdbool.h>
#include <time.h>

#define DEF_MONFILE "/tmp/rtd/rtd.data"
#define DEF_TMPDIR "/tmp/rtd/"
#define DEF_HEIGHT 512
#define DEF_WIDTH 670
#define DEF_AVG false

/*
 *	The following defines set the maximum number of channels to
 * 	process, and the maximum samples each channel can hold.
 *	This is done to allow for fixed-location data arrays and
 * 	thus fixed FFTW plans, which makes the whole thing easier
 * 	to deal with, and faster to boot.  4 x 2^17 doubles = 4 MB
 */
#define MAX_CHAN 4
#define MAX_FFTN 131072

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
};

int parse_options(struct parsed_options *, int, char **);
void init_parsed_options(struct parsed_options *);
void read_new_samples(unsigned short *, struct parsed_options *);
void fft_new_samples(unsigned short *);
void rescale_images(void);
