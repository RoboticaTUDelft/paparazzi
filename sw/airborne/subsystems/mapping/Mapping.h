#ifndef MAPPING_H
#define MAPPING_H

#include "subsystems/mapping/Frontier.h"
#include "subsystems/mapping/Astar.h"
#include "subsystems/mapping/MapPos.h"
#include "std.h"
#include <list>
#include <vector>

extern "C" {
#include "subsystems/mapping/mapping_navigation.h"
}

class Mapping {
	Map2D * map;
	MapPos pos;
	float goalHeading;
	float startHeading;

	point2d * startPos;

	int state;
	unsigned int index;
	std::list<point2d> frontierPoints;
	std::list<Frontier *> frontiers;
	std::list<Frontier *>::iterator ifrontiers;
	std::vector<std::list<tile *> *> paths;
	std::vector<Frontier *> frontiergrid;

	/* calculates next heading from starting point to destination */
	void setGoalHeading(const point2d *, const point2d *);
	/* turns the vehicle, returns true if goalHeading has been reached */
	bool_t turn();

	bool_t passedGoal(const point2d *, const point2d *);
public:
	Mapping () {
		map = new Map2D (0.5);
		state = 0;
	}

	Mapping (float resolution) {
		map = new Map2D (resolution);
		state = 0;
	}

	Mapping (Map2D * m) {
		map = m;
		state = 0;
	}

	/* executes the neccessary functions to complete 1 step in the mapping process */
	void mappingStep ();

	/* returns 1 if point is a Frontier else 0 */
	int isFrontier (const point2d *);

	/* returns a list of the Frontiers (clustered together with a minimum size) */
	void clusterFrontiers (const std::list<point2d> *, std::vector<Frontier *> *, int, int, int);

	/* returns the index of the best Frontier to visit next */
	int bestFrontier();

	/*  scale point2d to an integer point for an array presentation of the map */
	point2d scale (const point2d *);

	std::list<Frontier *> * getFrontiers();
};

#endif /* MAPPING_H */
