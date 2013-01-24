/*
 * Astar.cpp
 *
 *  Created on: 7 jan. 2013
 *      Author: kaj
 */

#include "Astar.h"

using namespace std;

/* type of list a tile can be in */
#define NONE   0
#define OPEN   1
#define CLOSED 2

int calcG(const tile * from, const tile * to) {
	int g = from->g + 10;
	/* add extra cost for diagonal tiles */
	if(from->pos.x != to->pos.x && from->pos.y != to->pos.y) {
		g += 4;
	}
	return g;
	//TODO: add cost for turning
}

bool expectedPrev(const tile * from, const tile * to) {
	int xDiff = to->pos.x - from->pos.x;
	int yDiff = to->pos.y - from->pos.y;

	return from->prev->pos.x == from->pos.x - xDiff &&
			from->prev->pos.y == from->pos.y - yDiff;
}

Astar::Astar(tile * start, tile * goal, Map2D * map) {
	this->start = start;
	this->goal = goal;
	this->map = map;
	openList.add(start, 0);
	start->list = OPEN;
}

int Astar::findPath() {
	if (goal->list != CLOSED && !openList.empty()) {
		tile * current = openList.get();
		openList.remove();

		checkTile(current);

		/* current now has been checked and can be added to ClosedList */
		current->list = CLOSED;
		return 0;
	} else if(goal->list == CLOSED) {
		savePath();
		return 1;
	} else return -1;
}

bool Astar::shouldCheck(const tile * current, const tile * t) {
	if(t && !(t->occupied || t->list == CLOSED)) {
		point2d p1, p2;
		p1.y = t->pos.y;
		p1.x = current->pos.x;
		p2.y = current->pos.y;
		p2.x = t->pos.x;
		tile * corner1 = map->get(&p1, 3);
		tile * corner2 = map->get(&p2, 3);
		/* cutting corners isn allowed */
		return (corner1 && !corner1->occupied) && (corner2 && !corner2->occupied);
	}
	return false;
}

void Astar::checkTile(tile * current) {
	int x, y;
	/* check every surrounding tile */
	for(y = -1; y < 2; y++) {
		for(x = -1; x < 2; x++) {
			point2d cur;
			cur.x = x + current->pos.x;
			cur.y = y + current->pos.y;
			/* not the middle tile */
			if(x != 0 || y != 0) {
				tile * t = map->get(&cur, 3);
				/* don't pick tiles that are occupied or in the closed list */
				if(shouldCheck(current, t)) {
					/* if not yet checked, set the h and g  */
					if(t->list == NONE) {
						float hx = ((cur.x - goal->pos.x < 0) ? -1 : 1) * (cur.x - goal->pos.x);
						float hy = ((cur.y - goal->pos.y < 0) ? -1 : 1) * (cur.y - goal->pos.y);
						t->h = 10 * (int)((hx + hy) / map->getResolution());
						t->g = calcG(current, t);
						t->list = OPEN;
						t->prev = current;
						openList.add(t, t->h + t->g);
					} else if(t->list == OPEN) {
						/* reset g and prev if g is cheaper */
						int g = calcG(current, t);
						if(g < t->g) {
							t->g = g;
							t->prev = current;
							openList.resort(t, t->g + t->h);
						}
					}
				}
			}
		}
	}
}

void Astar::savePath() {
	tile * current2 = goal;
	/* revert from goal to start (excluding start) */
	while(current2 != start) {
		path.push_front(current2);
		tile * temp = current2->prev;
		/* Only select waypoints (not all tiles) */
		while (temp != start && expectedPrev(current2->prev, current2)){
			current2 = current2->prev;
			temp = temp->prev;
		}
		current2 = temp;
	}
}

std::list<tile *> * Astar::getPath() {
	return &path;
}


