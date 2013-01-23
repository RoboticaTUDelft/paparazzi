/*
 * MapPos.cpp
 *
 *  Created on: 18 jan. 2013
 *      Author: kaj
 */


#include "subsystems/mapping/MapPos.h"

extern "C" {
#include "mcu_periph/sys_time_arch.h"
#include "subsystems/ins/ins_int.h"
}

MapPos::MapPos() {
	this->pos.x = 0;
	this->pos.y = 0;
	prevSpeedx = 0;
	prevSpeedy = 0;
	heading = 0;
}

void MapPos::startMoving() {
	prevSpeedx = ins_ltp_speed.x;
	prevSpeedy = ins_ltp_speed.y;
	SysTimeTimerStart(timer);
}

void MapPos::updatePos() {
	int speedx;
	int speedy;
#if USE_GPS
	speedx = ins_ltp_speed.x;
	speedy = ins_ltp_speed.y;
#else
	/* accelerometer shit */
#endif
	SysTimeTimerStop(timer);
	pos.x += prevSpeedx * timer + 0.5 * (speedx - prevSpeedx) * timer;
	pos.y += prevSpeedy * timer + 0.5 * (speedy - prevSpeedy) * timer;
	SysTimeTimerStart(timer);
}
