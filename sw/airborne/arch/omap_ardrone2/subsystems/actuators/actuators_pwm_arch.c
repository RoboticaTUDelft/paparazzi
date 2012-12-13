/*
 * Copyright (C) 2012 TU Delft: Minor Robotics - Quadrotor team 1
 *
 * This file is part of paparazzi.
 *
 * paparazzi is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2, or (at your option)
 * any later version.
 *
 * paparazzi is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with paparazzi; see the file COPYING.  If not, write to
 * the Free Software Foundation, 59 Temple Place - Suite 330,
 * Boston, MA 02111-1307, USA.
 */

#include "subsystems/actuators/actuators_pwm_arch.h"
#include <stdio.h>		// for standard input output
#include <stdlib.h>
#include <fcntl.h> 		// for O_RDWR, O_NOCTTY, O_NONBLOCK
#include <termios.h> 	// for baud rates and options
#include <unistd.h>

int actuator_fd;

/* GPIO_Select
 * This function selects/deselects a gpio device, or sets it as input
 *
 * (opt_value < 0) == set as input
 * (opt_value > 0) == select
 * (opt_value = 0) == deselect
 */
int gpio_select (int port, int opt_value){
	char cmdline[100];
	if(opt_value < 0)
		sprintf(cmdline, "/usr/sbin/gpio %d -d i", port);
	else if (opt_value > 0)
		sprintf(cmdline, "/usr/sbin/gpio %d -d ho 1", port);
	else
		sprintf(cmdline, "/usr/sbin/gpio %d -d ho 0", port);
	return system(cmdline);
}

int actuators_rdwr_cmd(uint8_t cmd, uint8_t *reply, int size) {
	write(actuator_fd,&cmd,1);
	return read(actuator_fd,reply,size);
}

void actuators_pwm_arch_init(void) {
	actuator_fd = open("/dev/ttyO0", O_RDWR | O_NOCTTY | O_NONBLOCK);
	if (actuator_fd == -1)
	{
		perror("actuator_init: Unable to open /dev/ttyO0 - ");
	}

	//read calls are non-blocking
	fcntl(actuator_fd, F_SETFL, 0);

	//set port options
	struct termios options;
	//Get the current options for the port
	tcgetattr(actuator_fd, &options);
	//Set the baud rates to 115200
	cfsetispeed(&options, B115200);
	cfsetospeed(&options, B115200);

  //Control Options
	//Enable the receiver and set local mode
	options.c_cflag |= (CLOCAL | CREAD);
	//Clear input options
	options.c_iflag = 0;
	//clear local options
	options.c_lflag=0;

	//clear output options (raw output)
	options.c_oflag &= ~OPOST;

	//Set bew options for the port
	tcsetattr(actuator_fd, TCSANOW, &options);

  //reset IRQ flipflop - on error 106 read 1, this code resets 106 to 0
	//IRQ active
	gpio_select(106,-1);
	//Toggle 107 to reset IRQ
	gpio_select(107,0);
	gpio_select(107,1);

  //configure motors
	uint8_t reply[8];
	for (int m = 0; m < ACTUATORS_PWM_NB; m++){
		gpio_select(68+m,-1);
		actuators_rdwr_cmd(0xe0, reply, 2);
		if(reply[0] != 0xe0 || reply[1] != 0x00){
			printf("motor%d cmd=0x%02x reply=0x%02x\n",m+1,(int)reply[0],(int)reply[1]);
		}
		actuators_rdwr_cmd((m+1), reply, 1);
	}

  //start multicast
	actuators_rdwr_cmd(0xa0,reply,1);
	actuators_rdwr_cmd(0xa0,reply,1);
	actuators_rdwr_cmd(0xa0,reply,1);
	actuators_rdwr_cmd(0xa0,reply,1);
	actuators_rdwr_cmd(0xa0,reply,1);
}

void actuator_pwm_arch_close(void){
	close(actuator_fd);
}

void actuator_pwm_commit(void){
	uint8_t pwm_signal[5];

	pwm_signal[0] = 0x20 | ((actuators_pwm_values[0]&0x1ff)>>4);
	pwm_signal[1] = ((actuators_pwm_values[0]&0x1ff)<<4) | ((actuators_pwm_values[1]&0x1ff)>>5);
	pwm_signal[2] = ((actuators_pwm_values[1]&0x1ff)<<3) | ((actuators_pwm_values[2]&0x1ff)>>6);
	pwm_signal[3] = ((actuators_pwm_values[2]&0x1ff)<<2) | ((actuators_pwm_values[3]&0x1ff)>>7);
	pwm_signal[4] = ((actuators_pwm_values[3]&0x1ff)<<1)| 0x00;

	write(actuator_fd, pwm_signal, 5);
}
