/*
 * Node.h
 *
 *  Created on: 14 jan. 2013
 *      Author: kaj
 */

#ifndef NODE_H_
#define NODE_H_

#include <list>
#include <vector>

/* the position of a node in the + field */
#define LEFT_UP    0
#define RIGHT_UP   1
#define LEFT_DOWN  2
#define RIGHT_DOWN 3

struct point2d {
	float x;
	float y;
};

struct tile {
	point2d pos;
	bool occupied;
	/* used in pathfinding */
	int g, h;
	int list;
	tile * prev;
};

class MapNode {
	MapNode * childs[4];
	point2d centre;
	float resolution;
	tile t;
	int depth;

	/* pre: point2d isWithin
	 * returns in which child the point is
	 */
	int orientation(const point2d *);
	/* calculates the centre of the child at orientation */
	void childCentre(int, point2d *);
	/* calculates the centre of the parent when the child (this) is at orientation */
	void parentCentre(const MapNode *, int, point2d *);
public:
	MapNode (point2d centre, float resolution, int depth);
	MapNode (MapNode * child, int orientation);

	int getDepth();
	float getResolution();
	const point2d * getCentre();
	tile * getTile();

	/* appoint all the tiles with point2d that lie within the borders of Node as free */
	void free(std::list<point2d> *);
	/* appoint tile with point2d as occupied */
	void occupied(const point2d *);
	/* returns the tile at given depth and point2d, returns NULL if it is an unknown tile */
	tile * get(const point2d *, int depth = 0);
	/* returns wether or not point2d is within the boarders of the Node */
	bool isWithin(const point2d *);

	void toText(std::vector<std::vector<char> > *, int);
};



#endif /* NODE_H_ */
