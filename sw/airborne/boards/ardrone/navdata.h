/*
 * navdata.h
 *
 *  Created on: Oct 17, 2012
 *      Author: dhensen
 */

#ifndef NAVDATA_H_
#define NAVDATA_H_

#include <stdint.h>

typedef struct {
	uint8_t buffer[120];
	int bufferSize;
	uint8_t block[60];
	int blockIndex;
} raw_navdata;

typedef struct
{
  unsigned short taille;
  unsigned short nu_trame;

  unsigned short ax;
  unsigned short ay;
  unsigned short az;

  unsigned short vx;
  unsigned short vy;
  unsigned short vz;
  unsigned short temperature_acc;
  unsigned short temperature_gyro;

  unsigned short ultrasound;

  unsigned short us_debut_echo;
  unsigned short us_fin_echo;
  unsigned short us_association_echo;
  unsigned short us_distance_echo;

  unsigned short us_curve_time;
  unsigned short us_curve_value;
  unsigned short us_curve_ref;

  unsigned short nb_echo;

  unsigned long  sum_echo;
  short gradient;

  unsigned short flag_echo_ini;

  long pressure;
  short temperature_pressure;

  short mx;
  short my;
  short mz;

  unsigned short chksum;

} __attribute__ ((packed)) measures_t;

measures_t* navdata;
uint16_t navdata_check;
uint16_t navdata_cks;

int navdata_init(void);
void navdata_close(void);
void navdata_appendBuffer(raw_navdata* ptr);
void navdata_readFromBuffer(raw_navdata* ptr);
void navdata_read_once(void);
void navdata_setMeasurements(void);
uint16_t navdata_checksum(void);
void navdata_fill_block(raw_navdata* nfb_ptr);

void navdata_event(void (* _navdata_handler)(void));

#define NavdataEvent(_navdata_handler) { \
  navdata_event(_navdata_handler); \
}

#endif /* NAVDATA_H_ */
