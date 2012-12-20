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

#define NAVDATA_MAX_READSIZE 120

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

	for (int i = 0; i < NAVDATA_MAX_READSIZE; i++){
		rn.buffer[i] = 0;
	}

	rn.blockIndex = 0;
	rn.bufferSize = 0;

	rn_ptr = &rn;
	nextReadSize = NAVDATA_MAX_READSIZE;

	navdata = (measures_t*) &(rn.block);
	navdata_check = 0;

	debug = 0;

	navdata_cks = 0;

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
			if (ptr->buffer[i] == 0x3a && (ptr->bufferSize - i) > 1) {
					if (ptr->buffer[i+1] == 0x0) {
						ptr->blockIndex = i;
						navdata_writeToBlock = 1;
						navdata_fill_block(ptr);
						break;
					}
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
		nextReadSize = NAVDATA_MAX_READSIZE;
		rn_ptr->blockIndex = 0;
	}

	else {
		// when a composed block is fully read, reset the nextReadSize
		nextReadSize = NAVDATA_MAX_READSIZE;
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
	navdata_cks = 0;
	navdata_cks += navdata->nu_trame;
	navdata_cks += navdata->ax;
	navdata_cks += navdata->ay;
	navdata_cks += navdata->az;
	navdata_cks += navdata->vx;
	navdata_cks += navdata->vy;
	navdata_cks += navdata->vz;
	navdata_cks += navdata->temperature_acc;
	navdata_cks += navdata->temperature_gyro;
	navdata_cks += navdata->ultrasound;
	navdata_cks += navdata->us_debut_echo;
	navdata_cks += navdata->us_fin_echo;
	navdata_cks += navdata->us_association_echo;
	navdata_cks += navdata->us_distance_echo;
	navdata_cks += navdata->us_curve_time;
	navdata_cks += navdata->us_curve_value;
	navdata_cks += navdata->us_curve_ref;
	navdata_cks += navdata->nb_echo;
	navdata_cks += navdata->sum_echo;
	navdata_cks += navdata->gradient;
	navdata_cks += navdata->flag_echo_ini;
	navdata_cks += navdata->pressure;
	navdata_cks += navdata->temperature_pressure;
	navdata_cks += navdata->mx;
	navdata_cks += navdata->my;
	navdata_cks += navdata->mz;

	return 0;

	return abs(navdata->chksum - navdata_cks);
}
