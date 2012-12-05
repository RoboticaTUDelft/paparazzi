/*
 * navdata.c
 *
 *  Created on: Oct 18, 2012
 *      Author: dhensen
 */
#include <stdio.h>		// for standard input output
#include <stdlib.h>
#include <fcntl.h> 		// for O_RDWR, O_NOCTTY, O_NONBLOCK
#include <termios.h> 	// for baud rates and options
#include <unistd.h>
#include <string.h>
#include "navdata.h"

int nav_fd;
unsigned int nextReadSize;
short int startFound;
short int startOffset;

raw_navdata rn_buffer;
raw_navdata rn;
raw_navdata *rn_ptr;
measures_t *navdata;

int navdata_init()
{
	nav_fd = open("/dev/ttyO1", O_RDWR | O_NOCTTY | O_NONBLOCK);
	if (nav_fd == -1)
	{
		perror("navdata_init: Unable to open /dev/ttyO1 - ");
		return 1;
	}

	fcntl(nav_fd, F_SETFL, 0); //read calls are non blocking
	//set port options
	struct termios options;
	//Get the current options for the port
	tcgetattr(nav_fd, &options);
	//Set the baud rates to 460800
	cfsetispeed(&options, B460800);
	cfsetospeed(&options, B460800);

	options.c_cflag |= (CLOCAL | CREAD); //Enable the receiver and set local mode
	options.c_iflag = 0; //clear input options
	options.c_lflag = 0; //clear local options
	options.c_oflag &= ~OPOST; //clear output options (raw output)

	//Set the new options for the port
	tcsetattr(nav_fd, TCSANOW, &options);

	// stop acquisition
	uint8_t cmd=0x02;
	write(nav_fd, &cmd, 1);

	// start acquisition
	cmd=0x01;
	write(nav_fd, &cmd, 1);

	// initialize buffers
	for (int i = 0; i < 60; i++) {
		rn_buffer.block[i] = 0;
		rn_buffer.buffer[i] = 0;
		rn.block[i] = 0;
		rn.buffer[i] = 0;
	}
	rn_buffer.blockIndex = 0;
	rn_buffer.bufferSize = 0;
	rn.blockIndex = 0;
	rn.bufferSize = 0;

	rn_ptr = &rn_buffer;
	nextReadSize = 60;
	startFound = 0;

	navdata = (measures_t*) &(rn.block);

	return 0;
}

void navdata_close()
{
	close(nav_fd);
}

void navdata_appendBuffer(raw_navdata* ptr, int offset)
{
	if ((ptr->blockIndex + (ptr->bufferSize - offset)) < 60) {
		for(int i = offset; i < ptr->bufferSize; i++)
		{
			printf("%02X ", ptr->buffer[i]);
			ptr->block[ptr->blockIndex + i] = ptr->buffer[i];
			ptr->blockIndex++;	// could this be integrated in the previous line like this: [ptr->blockIndex++ + i]?
		}
		printf("\n");
	}
}

void navdata_read_once()
{
	rn_ptr->bufferSize = read(nav_fd, rn_ptr->buffer, nextReadSize);
	printf("Read %d bytes. Next read size: %d bytes.\n", rn_ptr->bufferSize, nextReadSize);

	// if we read a buffer smaller than or equal to 60 bytes
	if (rn_ptr->bufferSize <= 60) {

		if (startFound == 1) {
			printf("Startfound before...\n");
			navdata_appendBuffer(rn_ptr, 0);
			printf("nextReadSize(b2) = %d\n", nextReadSize);
//			nextReadSize -= rn_ptr->bufferSize;
			printf("nextReadSize(a2) = %d\n", nextReadSize);
		} else {
			printf("Startfound not found...\n");
			for (int i = 0; i < rn_ptr->bufferSize; i++) {
				if (rn_ptr->buffer[i] == 0x3a) {
					printf("Startfound found just now...\n");
//					navdata_appendBuffer(rn_ptr, i);
					printf("nextReadSize(b) = %d\n", nextReadSize);
//					nextReadSize -= (rn_ptr->bufferSize - i);
					printf("nextReadSize(a) = %d\n", nextReadSize);
					startFound = 1;
				}
			}
		}

	}

	if (rn_ptr->blockIndex > 0) {
		nextReadSize = 60 - rn_ptr->blockIndex + 1;
	}

	// if we have gathered a full block
	if (nextReadSize == 0) {

		// the 60 byte block has to start with 0x3a (dec:58)
		if (rn_ptr->block[0] == 0x3a) {
			for (int bi = 0; bi < 60; bi++) {
				printf("%02X ", rn_ptr->block[bi]);
			}

			//				navdata = (measures_t*)rn_ptr->block;
			//				printf("v?(%d, %d, %d)\n", navdata->vx, navdata->vy, navdata->vz);
			//				printf("\n");

			//copy the rn_buffer to rn, this is a full block ready for reading
//			memcpy(&rn, &rn_buffer, sizeof(raw_navdata));
		}
	}

	if (nextReadSize <= 0) {
		// when a composed block is fully read, reset the nextReadSize
		nextReadSize = 60;
		rn_ptr->blockIndex = 0;
		// reset the block array and next read size
//		memset(rn_ptr->block, 0, 60);
		startFound = 0;
		printf("Reset startfound...\n");
	}
}

measures_t* navdata_getMeasurements()
{
//	navdata = (measures_t*) &(rn.block);
	navdata_checksum();
	return navdata;
}

void navdata_checksum()
{
	uint16_t checksum = 0;
	checksum += navdata->nu_trame;
	checksum += navdata->ax;
	checksum += navdata->ay;
	checksum += navdata->az;
	checksum += navdata->vx;
	checksum += navdata->vy;
	checksum += navdata->vz;
	checksum += navdata->temperature_acc;
	checksum += navdata->temperature_gyro;
	checksum += navdata->ultrasound;
	checksum += navdata->us_debut_echo;
	checksum += navdata->us_fin_echo;
	checksum += navdata->us_association_echo;
	checksum += navdata->us_distance_echo;
	checksum += navdata->us_curve_time;
	checksum += navdata->us_curve_value;
	checksum += navdata->us_curve_ref;
	checksum += navdata->nb_echo;
	checksum += navdata->sum_echo;
	checksum += navdata->gradient;
	checksum += navdata->flag_echo_ini;
	checksum += navdata->pressure;
	checksum += navdata->temperature_pressure;
	checksum += navdata->mx;
	checksum += navdata->my;
	checksum += navdata->mz;

	printf("Size of calculated checksum: %d\n", checksum);
	printf("Size of given checksum: %d\n", navdata->chksum);

}
