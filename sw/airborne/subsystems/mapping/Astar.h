/*
 * Astar.h
 *
 *  Created on: 7 jan. 2013
 *      Author: kaj
 */

#ifndef ASTAR_H_
#define ASTAR_H_

#include "subsystems/mapping/Map2D.h"
#include "subsystems/mapping/BinaryHeap.h"

class Astar {
	tile * start;
	tile * goal;
	Map2D * map;
	BinaryHeap<tile> openList;
	std::list<tile *> path;

	/* returns true if the tile should be checked */
	bool shouldCheck(const tile *, const tile *);
public:
	Astar(tile * start, tile * goal, Map2D * map);

	/* Executes a step to find the path,  *
	 * returns 0 if path is not yet found *
	 * returns 1 if path is found         *
	 * returns -1 if no path is possible  */
	int findPath();

	/* Checks the tile's surrounding tiles:     *
	 * adds them to the openList if neccessary  *
	 * sets/resets their score                  *
	 * adds current to the closed list                */
	void checkTile(tile * current);

	/* saves the path in a list (only selects waypoints)*/
	void savePath();

	/* returns the path that was found (will be an empty list if Astar is not done yet) */
	std::list<tile *> * getPath();
};

#endif /* ASTAR_H_ */
