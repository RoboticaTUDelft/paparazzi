/*
 * Copyright (C) 2012 TU Delft Quatrotor Group 1
 */

#include "subsystems/imu.h"
#include "navdata.h"

void imu_impl_init(void) {
  imu_data_available = FALSE;
}

void imu_periodic(void) {
  //checks if the navboard has a new dataset ready
//FIXME: obtaining data from navdata
//	QUAT_ASSIGN(imu.gyro_unscaled, 1, navboard.vx, navboard.vy, navboard.vz);
//	VECT3_ASSIGN(imu.accel_unscaled, navboard.ax, navboard.ay, navboard.az);
//	VECT3_ASSIGN(imu.mag_unscaled, navboard.mz, -(navboard.mx), navboard.my);
	imu_data_available = TRUE;

}

