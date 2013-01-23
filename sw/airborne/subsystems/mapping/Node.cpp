/*
 * Node.cpp
 *
 *  Created on: 14 jan. 2013
 *      Author: kaj
 */

#include <stddef.h>
#include "subsystems/mapping/Node.h"

using namespace std;

tile emptyTile(const point2d * p) {
	tile t;
	t.pos.x = p->x;
	t.pos.y = p->y;
	t.occupied = 0;
	t.list = 0;
	t.prev = NULL;
	return t;
}

MapNode::MapNode(MapNode * child , int orientation) {
	int i;
	point2d pCentre;
	parentCentre(child, orientation, &pCentre);
	this->centre = pCentre;
	this->resolution = child->getResolution() * 2;
	this->depth = child->getDepth() + 1;
	for(i = 0; i < 4; i++) {
		if(i == orientation) {
			childs[i] = child;
		} else {
			/* calculate centre of the child */
			point2d cCentre;
			childCentre(i, &cCentre);

			childs[i] = new MapNode (cCentre, child->getResolution(), child->getDepth());
		}
	}
	this->t = emptyTile(&centre);
}

MapNode::MapNode(point2d centre, float resolution, int depth) {
	this->centre = centre;
	this->resolution = resolution;
	this->depth = depth;
	this->t = emptyTile(&centre);
	int i;
	for(i = 0; i < 4; i++) {
		childs[i] = NULL;
	}
}

int MapNode::getDepth() { return this->depth;}
float MapNode::getResolution() { return this->resolution; }
const point2d * MapNode::getCentre() { return &this->centre; }
tile * MapNode::getTile() {return &this->t;}

void MapNode::free(list<point2d> * ray) {
	if(this->depth != 0) {
		while(!ray->empty() && isWithin(&ray->front())) {
			int o = orientation(&ray->front());
			if(!childs[o]) {
				float resolution = this->resolution / 2;
				point2d centre;
				childCentre(o, &centre);
				childs[o] = new MapNode (centre, resolution, this->depth - 1);
			}
			childs[o]->free(ray);
		}
	} else {
		/* occupied tiles stay occupied and when created, tiles are unoccupied
		 * this means we don't have to do anything at depth = 0
		 */
		ray->pop_front();
	}
}

void MapNode::occupied(const point2d * p) {
	if(this->depth != 0) {
		int o = orientation(p);
		if(!childs[o]) {
			float resolution = this->resolution / 2;
			point2d centre;
			childCentre(o, &centre);
			childs[o] = new MapNode (centre, resolution, this->depth - 1);
		}
		childs[o]->occupied(p);
	}
	/* parents with occupied childs are occupied as well */
	this->t.occupied = true;
}

tile * MapNode::get(const point2d * p, int depth) {
	if(isWithin(p)) {
		if(this->depth != depth) {
			int o = orientation(p);
			if(!childs[o]) {
				return NULL;
			} else {
				return childs[o]->get(p, depth);
			}
		} else {
			return &this->t;
		}
	} else return NULL;
}

bool MapNode::isWithin(const point2d * p) {
	bool within = true;
	if(p->x < this->centre.x) {
		within = this->centre.x - (this->resolution / 2) <= p->x;
	} else if(p->x > this->centre.x) {
		within = this->centre.x + (this->resolution / 2) > p->x;
	}

	if(p->y < this->centre.y) {
		within = within && this->centre.y - (this->resolution / 2) <= p->y;
	} else if(p->y > this->centre.y) {
		within = within && this->centre.y + (this->resolution / 2) > p->y;
	}

	return within;
}


void MapNode::toText(vector<vector<char> > * vv, int height) {
	if(depth == 0) {
		if((unsigned) height < vv->size()) { /* no need to add a vector */
			if(t.occupied) {
				(*vv)[height].push_back('x');
			} else {
				(*vv)[height].push_back('-');
			}
			/* add a space to maintain square like shape in txt file */
			(*vv)[height].push_back(' ');
		} else { /* need to add a vector */
			vector<char> v;
			if(t.occupied) {
				v.push_back('x');
			} else {
				v.push_back('-');
			}
			v.push_back(' ');
			/* add a space to maintain square like shape in txt file */
			vv->push_back(v);
		}
	} else {
		int k;
		/* check every child */
		for(k = 0; k < 4; k++) {
			/*calculate index of the vector of chars */
			int index = height * 2;
			if(k > 1)
				index++;

			/* if child exists */
			if(childs[k]) {
				childs[k]->toText(vv, index);
			} else {
				/* fill vv with a square of spaces (unknown space) */

				/* calc the width of the child (s) and the index (start)*/
				int i, m, n;
				int s = 2;
				int start = index;
				for(i = 0; i < depth - 1; i++) {
					s = s * 2;
					start = start * 2;
				}
				s = s / 2;

				if((unsigned) start < vv->size()) {
					for(m = 0; m < s; m++) {
						for(n = 0; n < s; n++) {
							(*vv)[start + m].push_back('0');
							(*vv)[start + m].push_back(' ');
						}
					}
				} else {
					vector<char> v;
					for(m = 0; m < s; m++) {
						vv->push_back(v);
						for(n = 0; n < s; n++) {
							(*vv)[start + m].push_back('0');
							(*vv)[start + m].push_back(' ');
						}
					}
				}
			}
		}
	}

}

int MapNode::orientation(const point2d * p) {
	if(p->x < this->centre.x) {
		if(p->y < this->centre.y) {
			return LEFT_DOWN;
		} else return LEFT_UP;
	} else {
		if(p->y < this->centre.y) {
			return RIGHT_DOWN;
		} else return RIGHT_UP;
	}
}

void MapNode::childCentre(int orientation, point2d * childCentre) {
	float diff = this->resolution / 4;
	switch(orientation) {
	case LEFT_UP:
		childCentre->x = this->centre.x - diff;
		childCentre->y = this->centre.y + diff;
		break;
	case RIGHT_UP:
		childCentre->x = this->centre.x + diff;
		childCentre->y = this->centre.y + diff;
		break;
	case LEFT_DOWN:
		childCentre->x = this->centre.x - diff;
		childCentre->y = this->centre.y - diff;
		break;
	case RIGHT_DOWN:
		childCentre->x = this->centre.x + diff;
		childCentre->y = this->centre.y - diff;
		break;
	}
}

void MapNode::parentCentre(const MapNode * child, int orientation, point2d * parentCentre) {
	float diff = child->resolution / 2;
		switch(orientation) {
		case LEFT_UP:
			parentCentre->x = child->centre.x + diff;
			parentCentre->y = child->centre.y - diff;
			break;
		case RIGHT_UP:
			parentCentre->x = child->centre.x - diff;
			parentCentre->y = child->centre.y - diff;
			break;
		case LEFT_DOWN:
			parentCentre->x = child->centre.x + diff;
			parentCentre->y = child->centre.y + diff;
			break;
		case RIGHT_DOWN:
			parentCentre->x = child->centre.x - diff;
			parentCentre->y = child->centre.y + diff;
			break;
		}
}
