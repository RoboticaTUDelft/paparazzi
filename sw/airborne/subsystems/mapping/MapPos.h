/*
 * MapPos.h
 *
 *  Created on: 17 jan. 2013
 *      Author: kaj
 */

#ifndef MAPPOS_H_
#define MAPPOS_H_

#include "subsystems/mapping/Node.h"

class MapPos {
	point2d pos;
	int timer;
	float heading;
	float prevSpeedx;
	float prevSpeedy;

public:
	MapPos();

	void startMoving();
	void updatePos();
	void updateHeading();

	point2d * getPos() { return &pos; }
	float getHeading() { return heading; }
};




#endif /* MAPPOS_H_ */
