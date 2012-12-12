/*
 * Copyright (C) 2012 TU Delft Quatrotor Group 1
 */

#include "subsystems/imu.h"
#include "navdata.h"

//stdio only included for debugging
#include <stdio.h>

void imu_impl_init(void) {
  imu_data_available = FALSE;
}

/*
 * Checks periodicly if the navdata countains new sensor values.
 * If it does it sets the gyro, acc and mag vectors and
 * tells the AHRS (indirectly) that the data is available
 */
void imu_periodic(void) {
//	printf("imu test 1");
	if (navdata_check == 0){
//		printf("imu test 2");
		RATES_ASSIGN(imu.gyro_unscaled, navdata->vx, navdata->vy, navdata->vz);
		VECT3_ASSIGN(imu.accel_unscaled, navdata->ax, navdata->ay, navdata->az);
		VECT3_ASSIGN(imu.mag_unscaled, navdata->mz, -(navdata->mx), navdata->my);
		printf("mag = (%d,%d,%d)", navdata->mz, -(navdata->mx), navdata->my);
		imu_data_available = TRUE;
	}
	else
		imu_data_available = FALSE;

}

