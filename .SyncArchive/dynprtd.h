struct parsed_options {
	char *filename;
	char *conffile;
	char *mondir;
	char *tmpdir;
	char *agc;
	double value;
};

int read_input_file(void);
void cmd_init_parsed_options(struct parsed_options *options);
int cmd_parse_options(struct parsed_options *options, int argc, char *argv[]);

static struct header_info {
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
} header;