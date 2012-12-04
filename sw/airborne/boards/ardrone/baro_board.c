/*
 * Copyright (C) 2012 TU Delft Quatrotor Team 1
 *
 * Paparazzi AR Drone Baro Sensor implementation:
 * These functions are mostly empty because of the calibration and calculations done by the Parrot Navigation board
 */

#include "subsystems/sensors/baro.h"
#include "navdata.h"

struct Baro baro;

void baro_init(void) {
  baro.status = BS_UNINITIALIZED;
  baro.absolute     = 0;
  baro.differential = 0;
}

void baro_periodic(void) {
//FIXME: if statement pass condition has to be set according to navdata
  if(navdata_ontvangen){
	baro.status = BS_RUNNING;
    available = TRUE;
  }
}
