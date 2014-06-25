/*
 *  prtd: process real-time data for hf2_display
 */

#define _GNU_SOURCE

/*
 *	IMAGE_WIDTH must match the width of Ca_Canvas defined in whatever
 *	version of the real time display GUI we're working with.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <unistd.h>
#include <fftw3.h>
#include <sys/types.h>
#include <sys/stat.h>

#include "dynprtd.h"

#define IMAGE_WIDTH 670

unsigned short *samples;
double fft_samples1[1024], fft_samples2[1024], fft_samples3[1024], fft_samples4[1024];
double out1[1024], out2[1024], out3[1024], out4[1024];
double hann[1024];
int total_samples;
fftw_plan plan_forward1, plan_forward2, plan_forward3, plan_forward4;

float a, b;
float v1, v2, v3, v4;
float r1, r2, r3, r4;
float x[512];
float df = 5e3 / 512.;

unsigned char im1[512][IMAGE_WIDTH];
unsigned char im2[512][IMAGE_WIDTH];
unsigned char im3[512][IMAGE_WIDTH];
unsigned char im4[512][IMAGE_WIDTH];
char tmp_dir[200], instring[200], config_filename[200], data_dir[200];
float im1o[512][IMAGE_WIDTH], im2o[512][IMAGE_WIDTH], im3o[512][IMAGE_WIDTH], im4o[512][IMAGE_WIDTH];
int gray_min, gray_max, old_gray_min = 0, old_gray_max = 0;
int write_data = 0;
unsigned long old_sec = 0, old_usec = 0;

void read_new_samples(struct parsed_options *);
void fft_new_samples(void);
void rescale_images(void);

int main(int argc, char **argv) {
	int i, j;
	long int procid;
	char outstring1[100];
	FILE *in, *image1, *image2, *image3, *image4, *out;
	struct parsed_options options;

	umask(000);

	init_parsed_options(&options);
	parse_options(&options, argc, argv);

	nice(10);

	sprintf(instring,"%s/process_rt_data_running",options.tmpdir);
	out = fopen(instring, "r");
	if (out != NULL) {
		fprintf(stderr, "\nrprocess_rt_data found a lock file ... ");
		fscanf(out, "%lu", &procid);
		fprintf(stderr, "\n  PID: %lu", procid);
		fclose(out);
		sprintf(outstring1,"/proc/%lu/cmdline",procid);
		out = fopen(outstring1, "r");
		if (out != NULL) {
			fscanf(out, "%s", outstring1);
			sprintf(instring,"process_real_time_data");
			//if( strncmp(outstring1,"/home/radio",11)==0 )
			if (strstr(outstring1, instring) != NULL) {
				fprintf(stderr, "\n  Process Exists. Exiting ...\n\n");
				fclose(out);
				exit(0);
			}
			fclose(out);
		}
		sprintf(instring,"%s/process_rt_data_running",options.tmpdir);
		remove(instring);
		fprintf(stderr, "\n  Process Does Not Exist. Lock File Removed\n");
	}

	sprintf(instring,"%s/process_rt_data_running",options.tmpdir);

	out = fopen(instring, "w");

	if (out != NULL) {
		fprintf(out, "%lu", (long unsigned int) getpid());
		fclose(out);
	} else {
		fprintf(stderr, "Couldn't write lock file %s?!", instring);
		exit(-1);
	}

	/* initialize the images */
	for (i = 0; i < 512; i++)
		for (j = 0; j < IMAGE_WIDTH; j++) {
			im1o[i][j] = 20.;
			im2o[i][j] = 20.;
			im3o[i][j] = 20.;
			im4o[i][j] = 20.;
		}

	/* initialize Hann window */
	for (i = 0; i < 1024; i++) {
		hann[i] = 0.5 * (1 - cos(2*M_PI*i/1023));
	}

	/* initialize the fftw3 plans */
	plan_forward1 = fftw_plan_r2r_1d(1024, fft_samples1, out1, FFTW_R2HC, FFTW_MEASURE);
	plan_forward2 = fftw_plan_r2r_1d(1024, fft_samples2, out2, FFTW_R2HC, FFTW_MEASURE);
	plan_forward3 = fftw_plan_r2r_1d(1024, fft_samples3, out3, FFTW_R2HC, FFTW_MEASURE);
	plan_forward4 = fftw_plan_r2r_1d(1024, fft_samples4, out4, FFTW_R2HC, FFTW_MEASURE);


	while (1) {
		/* read in the new data */
		read_new_samples();
		/* fft the new data */
		fft_new_samples();

		sprintf(instring,"%s/hf2_display_running",options.tmpdir);
		in = fopen(instring, "r");
		if (in != NULL) {
			write_data = 1;
			fclose(in);
		} else
			write_data = 0;

		sprintf(instring,"%s/levels.grayscale",options.tmpdir);
		in = fopen(instring, "r");
		if (in != NULL) {
			fscanf(in, "%d %d", &gray_min, &gray_max);
			fclose(in);
		} else {
			gray_min = 0;
			gray_max = 60.;
		}
		if (old_gray_min != gray_min || old_gray_max != gray_max)
			rescale_images();
		if (write_data) {
			sprintf(instring,"%s/test.data",options.tmpdir);
			out = fopen(instring, "w");
		} else {
			printf("hf2_display not running?\n");
		}

		for (i = 0; i < 512; i++) {
			x[i] = i * df;
			memcpy(*(im1+i),&im1[i][1],IMAGE_WIDTH-1);
			memcpy(*(im2+i),&im2[i][1],IMAGE_WIDTH-1);
			memcpy(*(im3+i),&im3[i][1],IMAGE_WIDTH-1);
			memcpy(*(im4+i),&im4[i][1],IMAGE_WIDTH-1);
			memcpy((im1o+i),&im1o[i][1],(IMAGE_WIDTH-1)*sizeof(float));
			memcpy((im2o+i),&im2o[i][1],(IMAGE_WIDTH-1)*sizeof(float));
			memcpy((im3o+i),&im3o[i][1],(IMAGE_WIDTH-1)*sizeof(float));
			memcpy((im4o+i),&im4o[i][1],(IMAGE_WIDTH-1)*sizeof(float));
			r1 = out1[i];
			r2 = out2[i];
			r3 = out3[i];
			r4 = out4[i];
			im1o[512 - i][IMAGE_WIDTH-1] = r1;
			im2o[512 - i][IMAGE_WIDTH-1] = r2;
			im3o[512 - i][IMAGE_WIDTH-1] = r3;
			im4o[512 - i][IMAGE_WIDTH-1] = r4;
			v1 = a + r1 * b + 0.5;
			v2 = a + r2 * b + 0.5;
			v3 = a + r3 * b + 0.5;
			v4 = a + r4 * b + 0.5;
			if (v1 < 0)
				v1 = 0;
			if (v2 < 0)
				v2 = 0;
			if (v3 < 0)
				v3 = 0;
			if (v4 < 0)
				v4 = 0;
			if (v1 > 255)
				v1 = 255;
			if (v2 > 255)
				v2 = 255;
			if (v3 > 255)
				v3 = 255;
			if (v4 > 255)
				v4 = 255;

			im1[512 - i][IMAGE_WIDTH-1] = 255 - (unsigned char) v1;
			im2[512 - i][IMAGE_WIDTH-1] = 255 - (unsigned char) v2;
			im3[512 - i][IMAGE_WIDTH-1] = 255 - (unsigned char) v3;
			im4[512 - i][IMAGE_WIDTH-1] = 255 - (unsigned char) v4;

			if (write_data)
				fprintf(out, "%.0f %.2f %.2f %.2f\n", x[i], r1, r2, r3);
		}

		if (write_data) {
			fclose(out);

			sprintf(instring,"%s/test.image1",options.tmpdir);
			image1 = fopen(instring, "w");
			fprintf(image1, "P5\n%i 512\n255\n",IMAGE_WIDTH);
			fwrite(im1, sizeof(im1), 1, image1);
			fclose(image1);

			sprintf(instring,"%s/test.image2",options.tmpdir);
			image2 = fopen(instring, "w");
			fprintf(image2, "P5\n%i 512\n255\n",IMAGE_WIDTH);
			fwrite(im2, sizeof(im2), 1, image2);
			fclose(image2);

			sprintf(instring,"%s/test.image3",options.tmpdir);
			image3 = fopen(instring, "w");
			fprintf(image3, "P5\n%i 512\n255\n",IMAGE_WIDTH);
			fwrite(im3, sizeof(im3), 1, image3);
			fclose(image3);

			sprintf(instring,"%s/test.image4",options.tmpdir);
			image4 = fopen(instring, "w");
			fprintf(image4, "P5\n%i 512\n255\n",IMAGE_WIDTH);
			fwrite(im4, sizeof(im4), 1, image4);
			fclose(image4);
		}
		usleep(1e4);
	}
	return (0);
}

/**************** read_new_samples() *************************/
void read_new_samples(void) {
	struct header_info header;
	FILE *in;

	in = fopen(options.monfile, "r");
	fread(&header, sizeof(header), 1, in);

	if (samples != NULL) free(samples);
	samples = malloc(2 * header.num_read);
	//fprintf(stderr,"\nReading Samples (%lu bytes) ... ",2*header.num_read);
	fread(samples, 2 * header.num_read, 1, in);
	//fprintf(stderr,"Done.\n");

	fclose(in);

	if (old_sec == header.start_timeval.tv_sec && old_usec
			== header.start_timeval.tv_usec) {
		usleep(1e4);
		read_new_samples();
	} else {
//		printf("\nReading new samples");
		/*
		 printf("\nHeader Info: (%d bytes)",sizeof(header));
		 printf("\n\tSite ID: %s",header.site_id);
		 printf("\n\tNum Channels: %d",header.num_channels);
		 printf("\n\tNum Samples: %d",header.num_samples);
		 printf("\n\tNum Read: %d",header.num_read);
		 if( header.num_read==header.num_channels*header.num_samples )
		 printf(" OK");
		 else
		 printf(" Missing Samples!");
		 printf("\n\tSample Frequency: %.3f MHz",header.sample_frequency/1e6);
		 printf("\n\tTime Between Acquisitions: %.3fs",header.time_between_acquisitions);
		 printf("\n\tAcquisition Time: %lu --> (UT) %s",header.start_time,asctime(gmtime(&header.start_time)));
		 printf("\tAcquistition Time: %lu and %luus",header.start_timeval.tv_sec,header.start_timeval.tv_usec);
		 printf("\n\tByte Packing: %d",header.byte_packing);
		 printf("\n\tData Collection Code Version: %.1f",header.code_version);
		 */
		total_samples = header.num_channels * header.num_samples;

		old_sec = header.start_timeval.tv_sec;
		old_usec = header.start_timeval.tv_usec;
		return;
	}
	return;
}

/*********** fft_new_samples() ***********************/
void fft_new_samples(void) {
	int i;
	double mean1, mean2, mean3, mean4;

	mean1 = 0;
	mean2 = 0;
	mean3 = 0;
	mean4 = 0;
	double shift[4];
	// sort from 12341234 order in samples into channel arrays
	for (i = 0; i < 1024* 4 ; i += 4) {
		fft_samples1[i / 4] = samples[i];
		mean1 += fft_samples1[i / 4];
		fft_samples2[i / 4] = samples[i + 1];
		mean2 += fft_samples2[i / 4];
		fft_samples3[i / 4] = samples[i + 2];
		mean3 += fft_samples3[i / 4];
		fft_samples4[i / 4] = samples[i + 3];
		mean4 += fft_samples4[i / 4];
		//fprintf(stderr,"%3d %u %u %u %u\n",i/4,samples[i],samples[i+1],samples[i+2],samples[i+3]);
		//printf("%d %lf\n",i/4,fft_samples[i/4]);
	}
	mean1 /= 1024;
	mean2 /= 1024;
	mean3 /= 1024;
	mean4 /= 1024;
	for (i = 0; i < 1024; i++) {
		fft_samples1[i] -= mean1;
		fft_samples2[i] -= mean2;
		fft_samples3[i] -= mean3;
		fft_samples4[i] -= mean4;
	}

	// Hann window - why doesn't this work?
/*	for (i = 0; i < 1024; i++) {
		fft_samples1[i] *= hann[i];
		fft_samples2[i] *= hann[i];
		fft_samples3[i] *= hann[i];
		fft_samples4[i] *= hann[i];
	}
*/

	fftw_execute(plan_forward1);
	fftw_execute(plan_forward2);
	fftw_execute(plan_forward3);
	fftw_execute(plan_forward4);
	out1[0] = out1[0] * out1[0];
	out2[0] = out2[0] * out2[0];
	out3[0] = out3[0] * out3[0];
	out4[0] = out4[0] * out4[0];
	for (i = 1; i < 512; i++) {
		out1[i] = 10* log10 (sqrt(out1[i] * out1[i] + out1[1024 - i]
				* out1[1024 - i])) + 0.5;
		out2[i] = 10* log10 (sqrt(out2[i] * out2[i] + out2[1024 - i]
				* out2[1024 - i])) + 0.5;
		out3[i] = 10* log10 (sqrt(out3[i] * out3[i] + out3[1024 - i]
				* out3[1024 - i])) + 0.5;
		out4[i] = 10* log10 (sqrt(out4[i] * out4[i] + out4[1024 - i]
				* out4[1024 - i])) + 0.5;
		//printf("%3d   %15.5f\n",i,out[i]);
	}
	shift[0] = 20-(out1[3]+out1[4])/2;
	shift[1] = 20-(out2[3]+out2[4])/2;
	shift[2] = 20-(out3[3]+out3[4])/2;
	shift[3] = 20-(out4[3]+out4[4])/2;
	for (i = 0; i < 512; i++) {
		out1[i] += shift[0];
		out2[i] += shift[1];
		out3[i] += shift[2];
		out4[i] += shift[3];
	}

	//fftw_destroy_plan(plan_forward);

}

/***************************************************************/
void rescale_images() {
	int i, j;

	old_gray_max = gray_max;
	old_gray_min = gray_min;

	b = 255. / (gray_max - gray_min);
	a = -b * gray_min;

	printf("\nRescaling Images ... ");
	for (i = 0; i < 512; i++)
		for (j = 0; j < IMAGE_WIDTH; j++) {
			v1 = a + im1o[i][j] * b + 0.5;
			v2 = a + im2o[i][j] * b + 0.5;
			v3 = a + im3o[i][j] * b + 0.5;
			v4 = a + im4o[i][j] * b + 0.5;
			if (v1 < 0)
				v1 = 0;
			if (v2 < 0)
				v2 = 0;
			if (v3 < 0)
				v3 = 0;
			if (v4 < 0)
				v4 = 0;
			if (v1 > 255)
				v1 = 255;
			if (v2 > 255)
				v2 = 255;
			if (v3 > 255)
				v3 = 255;
			if (v4 > 255)
				v4 = 255;
			im1[i][j] = 255 - (unsigned char) v1;
			im2[i][j] = 255 - (unsigned char) v2;
			im3[i][j] = 255 - (unsigned char) v3;
			im4[i][j] = 255 - (unsigned char) v4;
		}
	printf("Done");
}
