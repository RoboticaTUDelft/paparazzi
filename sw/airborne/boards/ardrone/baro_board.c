/*
 * Copyright (C) 2012 TU Delft Quatrotor Team 1
 *
 * Paparazzi AR Drone Baro Sensor implementation:
 * These functions are mostly empty because of the calibration and calculations done by the Parrot Navigation board
 */

#include "subsystems/sensors/baro.h"
#include "navdata.h"

//stdio only included for debugging
#include <stdio.h>

struct Baro baro;

void baro_init(void) {
  baro.status = BS_UNINITIALIZED;
  baro.absolute     = 0;
  baro.differential = 0;
  baro_data_available = FALSE;
}

/*
 * Checks periodicly if the navdata countains new sensor values.
 * If it does it sets the absolute pressure of the barometer and
 * tells the INS (indirectly) that the data is available
 */
void baro_periodic(void) {
  baro.status = BS_RUNNING;
  if(navdata_check == 0){
	baro.absolute = navdata->pressure;
	baro_data_available = TRUE;
//	printf("baro   ?:pressure = %d\n", baro.absolute);
  }
  else
	baro_data_available = FALSE;
}
