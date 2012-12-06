/*
 * Copyright (C) 2012 TU Delft Quatrotor Team 1
 *
 * Paparazzi AR Drone Baro Sensor implementation:
 * These functions are mostly empty because of the calibration and calculations done by the Parrot Navigation board
 */

#include "subsystems/sensors/baro.h"
#include "navdata.h"

//test
#include <stdio.h>

struct Baro baro;

void baro_init(void) {
  baro.status = BS_UNINITIALIZED;
  baro.absolute     = 0;
  baro.differential = 0;
  baro_data_available = FALSE;
}

void baro_periodic(void) {
	baro.status = BS_RUNNING;
	if(navdata_check == 0){
		baro.absolute = navdata->pressure;
		baro_data_available = TRUE;
//		printf("baro   ?:pressure = %d\n", baro.absolute);
  }
  else
	  baro_data_available = FALSE;
}
