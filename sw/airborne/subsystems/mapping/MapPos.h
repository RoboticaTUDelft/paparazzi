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
	int prevSpeedx;
	int prevSpeedy;

public:
	MapPos();

	void startMoving();
	void updatePos();
	void stopMoving();
};




#endif /* MAPPOS_H_ */
