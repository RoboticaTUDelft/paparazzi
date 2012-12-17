/*
 * Copyright (C) 2012 TU Delft Quatrotor Group 1
 */

#include "subsystems/imu.h"
#include "navdata.h"

//temp
#include <stdio.h>

void imu_impl_init(void) {
  imu_data_available = FALSE;
}

void imu_periodic(void) {
  //checks if the navboard has a new dataset ready
	if (navdata_check == 0){
		RATES_ASSIGN(imu.gyro_unscaled, navdata->vx, navdata->vy, navdata->vz);
		VECT3_ASSIGN(imu.accel_unscaled, navdata->ax, navdata->ay, navdata->az);
		VECT3_ASSIGN(imu.mag_unscaled, navdata->mz, -(navdata->mx), navdata->my);
		imu_data_available = TRUE;
	}
	else
		imu_data_available = FALSE;

}

