/*
 * MapPos.cpp
 *
 *  Created on: 18 jan. 2013
 *      Author: kaj
 */


#include "subsystems/mapping/MapPos.h"

extern "C" {
#include "mcu_periph/sys_time_arch.h"
#include "state.h"
}

MapPos::MapPos() {
	this->pos.x = 0;
	this->pos.y = 0;
	prevSpeedx = 0;
	prevSpeedy = 0;
	heading = 0;
}

void MapPos::startMoving() {
	NedCoor_f* speed = stateGetSpeedNed_f();

	prevSpeedx = speed->x;
	prevSpeedy = speed->y;
	SysTimeTimerStart(timer);
}

void MapPos::updatePos() {
	SysTimeTimerStop(timer);

	NedCoor_f* speed = stateGetSpeedNed_f();

	/* integrate speed to estimate position */
	pos.x += prevSpeedx * timer + 0.5 * (speed->x - prevSpeedx) * timer;
	pos.y += prevSpeedy * timer + 0.5 * (speed->y - prevSpeedy) * timer;
	/* reset timer */
	SysTimeTimerStart(timer);
	/* set prevSpeed for next integration */
	prevSpeedx = speed->x;
	prevSpeedy = speed->y;
}

void MapPos::updateHeading() {
	heading = stateGetNedToBodyEulers_f()->psi;
}
