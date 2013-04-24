/*
 * electrical_arch.c
 *
 *  Created on: Nov 23, 2012
 *      Author: dhensen
 */

#include "subsystems/electrical/electrical_arch.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include <fcntl.h>
#include <sys/time.h>
#include <unistd.h>
#include "i2c-dev.h"

struct Electrical electrical;
vbat_struct vbat;

#define AUXILIARY 0x4a;

int fd;

void electrical_init(void) {

  fd = open( "/dev/i2c-1", O_RDWR );

  if( ioctl( fd, I2C_SLAVE, AUXILIARY ) < 0 )
  {
    fprintf( stderr, "Failed to set slave address: %m\n" );
    return 1;
  }

  // Turn on MADC in CTRL1
  if( i2c_smbus_write_byte_data( fd, 0x00, 0x01))   {
    fprintf( stderr, "Failed to write to I2C device 2\n" );
  }
  // Select ADCIN0 for conversion in SW1SELECT_LSB
  if( i2c_smbus_write_byte_data( fd, 0x06, 0x01)){
    fprintf( stderr, "Failed to write to I2C device 3\n" );
  }
  // Select ADCIN12 for conversion in SW1SELECT_MSB
  if( i2c_smbus_write_byte_data( fd, 0x07, 0x10))  {
    fprintf( stderr, "Failed to write to I2C device 4\n" );
  }
  // Start all channel conversion by setting bit 5 to one in CTRL_SW1
  if( i2c_smbus_write_byte_data( fd, 0x12, 0x20))  {
    fprintf( stderr, "Failed to write to I2C device 5\n" );
  }

  //	printf("Status returned on vbat_init: %d", status);

}

void electrical_periodic(void) {
  unsigned char lsb, msb;
  lsb = i2c_smbus_read_byte_data(fd, 0x37);
  msb = i2c_smbus_read_byte_data(fd, 0x38);

  lsb >>= 6;
  msb <<= 2;



  //electrical.vsupply = 120;
}
