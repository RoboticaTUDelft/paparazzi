/*
 * Map2D.h
 *
 *  Created on: 14 jan. 2013
 *      Author: kaj
 */

#ifndef MAP2D_H_
#define MAP2D_H_

#include "subsystems/mapping/Node.h"

class Map2D {
	MapNode * root;
	float resolution;

	/* resizes map such that point2d is a coord within the borders of the map */
	void resize(const point2d *);
	/* calculates all the point2d that are crossed by the ray excluding the end */
	void calculateRay(const point2d *, const point2d *, std::list<point2d> *);
public:
	Map2D(float);
	/*returns the tile at said x and y coord */
	tile * get(const point2d *, int depth = 0);
	/* fills in the points from start till end (end excluded) as free
	 * , end is set as occupied */
	void insertRay(const point2d *, const point2d *);

	/* fills in the points from start till end as free */
	void insertFreeRay(const point2d *, const point2d *);

	float getResolution();
	int getSize();
	point2d getMin();
	point2d getMax();

	void add(const point2d *, bool);

	void toFile();
};

#endif /* MAP2D_H_ */
