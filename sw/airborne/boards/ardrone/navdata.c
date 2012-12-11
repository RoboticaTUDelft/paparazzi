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
int navdata_parced;
unsigned int nextReadSize;
short int navdata_writeToBlock;
int debug;

raw_navdata rn;
raw_navdata* rn_ptr;

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
		rn.block[i] = 0;
	}

	for (int i = 0; i < 120; i++){
		rn.buffer[i] = 0;
	}

	rn.blockIndex = 0;
	rn.bufferSize = 0;

	rn_ptr = &rn;
	nextReadSize = 120;

	navdata = (measures_t*) &(rn.block);
	navdata_check = 0;

	debug = 0;

	return 0;
}

void navdata_close()
{
	close(nav_fd);
}

void navdata_readFromBuffer(raw_navdata* ptr)
{
	if(navdata_writeToBlock == 0){
		for (int i = 0; i < ptr->bufferSize; i++){
			if (ptr->buffer[i] == 0x3a){
				ptr->blockIndex = i;
				navdata_writeToBlock = 1;
				navdata_fill_block(ptr);
				break;
			}
		}
	}
}

void navdata_read_once()
{
	rn_ptr->bufferSize = read(nav_fd, rn_ptr->buffer, nextReadSize);
//	printf("Read %d bytes. Next read size: %d bytes.\n", rn_ptr->bufferSize, (nextReadSize - rn_ptr->bufferSize));

	nextReadSize -= rn_ptr->bufferSize;

	// if we read a buffer smaller than or equal to 60 bytes
	if (nextReadSize > 0) {
//		printf("Buffer not filled completely\n");
	}

	// if we have gathered a full block
	else if (nextReadSize == 0) {
		navdata_readFromBuffer(rn_ptr);

		// the 60 byte block has to start with 0x3a (dec:58)
		if (debug == 1){
			for (int bi = 0; bi < 60; bi++) {
//				printf("%02X ", rn_ptr->block[bi]);
			}
			printf("\n");
		}
		nextReadSize = 120;
		rn_ptr->blockIndex = 0;
	}

	else {
		// when a composed block is fully read, reset the nextReadSize
		nextReadSize = 120;
		rn_ptr->blockIndex = 0;

//		printf("Error: nextReadSize < 0");
	}
}

void navdata_fill_block(raw_navdata* nfb_ptr){
	for (int i = 0; i < 60; i++){
		nfb_ptr->block[i] = nfb_ptr->buffer[nfb_ptr->blockIndex + i];
	}

	navdata_writeToBlock = 0;
}

void navdata_setMeasurements() {
	navdata_check = navdata_checksum();
}

void navdata_event(void (* _navdata_handler)(void)){
	_navdata_handler();
}

uint16_t navdata_checksum()
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

//	return abs(navdata->chksum - checksum);
	return 0;
}
