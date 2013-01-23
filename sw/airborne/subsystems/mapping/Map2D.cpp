/*
 * Map2D.cpp
 *
 *  Created on: 14 jan. 2013
 *      Author: kaj
 */

#include "subsystems/mapping/Map2D.h"
#include <fstream>
#include <iostream>

using namespace std;

Map2D::Map2D(float resolution) {
	point2d centre;
	centre.x = resolution / 2;
	centre.y = resolution / 2;
	root = new MapNode (centre, resolution, 0);
	this->resolution = resolution;
}

void Map2D::insertRay(const point2d * start, const point2d * end) {
	insertFreeRay(start, end);

	root->occupied(end);
}

void Map2D::insertFreeRay(const point2d * start, const point2d * end) {
	/* make sure start and end fit on the map,
	 * map is square and ray is a line so other points will be on the map as well */
	resize (start);
	resize (end);

	list<point2d> ray;
	calculateRay(start, end, &ray);
	root->free(&ray);
}

void Map2D::resize(const point2d * p) {
	while(!root->isWithin(p)) {
		int o;
		if(p->x < root->getCentre()->x) {
			if(p->y < root->getCentre()->y) {
				o = RIGHT_UP;
			} else o = RIGHT_DOWN;
		} else {
			if(p->y < root->getCentre()->y) {
				o = LEFT_UP;
			} else o = LEFT_DOWN;
		}
		MapNode * temp = new MapNode (root, o);
		root = temp;
	}
}

tile * Map2D::get(const point2d * p, int depth) {
	return root->get(p, depth);
}

float Map2D::getResolution() { return this->resolution; }

int Map2D::getSize() {
	int i;
	int size = 1;
	for(i = 0; i < root->getDepth(); i++) {
		size = size * 2;
	}
	return size;
}

point2d Map2D::getMin() {
	point2d min;
	min.x = root->getCentre()->x - (root->getResolution() / 2);
	min.y = root->getCentre()->y - (root->getResolution() / 2);
	return min;
}

point2d Map2D::getMax() {
	point2d max;
	max.x = root->getCentre()->x + (root->getResolution() / 2);
	max.y = root->getCentre()->y + (root->getResolution() / 2);
	return max;
}

void Map2D::add(const point2d * p, bool occupied) {
	resize(p);

	if(!occupied) {
		list<point2d> ray;
		ray.push_back(*p);
		root->free(&ray);
	} else root->occupied(p);
}

void Map2D::toFile() {
	ofstream file;
	file.open("map.txt");
	vector<vector<char> > vv;
	root->toText(&vv, 0);
	int i,j;
	for(i = 0; (unsigned) i < vv.size(); i++) {
		for(j = 0; (unsigned) j < vv[i].size(); j++) {
			file << vv[i][j];
		}
		file << '\n';
	}
	file.close();
}

void Map2D::calculateRay(const point2d * start, const point2d * end, list<point2d> * ray){
	/* set the points in the middle of the tiles */
	int normStartx = ((int)(start->x / this->resolution)) * 2 + 1;
	int normStarty = ((int)(start->y / this->resolution)) * 2 + 1;
	int normEndx = ((int)(end->x / this->resolution)) * 2 + 1;
	int normEndy = ((int)(end->y / this->resolution)) * 2 + 1;
	point2d p;
	p.x = (normStartx - 1) / 2 * this->resolution;
	p.y = (normStarty - 1) / 2 * this->resolution;
	ray->push_back(p);

	int xstep, ystep;
	int x = normStartx;
	int y = normStarty;

	int dx = (normEndx - normStartx) / 2;
	if(dx < 0) {
		dx = -dx;
		xstep = -2;
	} else xstep = 2;

	int dy = (normEndy - normStarty) / 2;
	if(dy < 0) {
		dy = -dy;
		ystep = -2;
	} else ystep = 2;

	int ddx = 2 * dx;
	int ddy = 2 * dy;

	int errorprev, error, i;
	if(ddx >= ddy) {
		int errorprev = error = dx;
		for(i = 0; i < dx; i++) {
			x += xstep;
			error += ddy;
			if(error > ddx) {
				y += ystep;
				error -= ddx;
				if(error + errorprev < ddx) {
					p.x = (x - 1) / 2 * this->resolution;
					p.y = (y - ystep - 1) / 2 * this->resolution;
					ray->push_back(p);
				} else if (error + errorprev > ddx) {
					p.x = (x - xstep - 1) / 2 * this->resolution;
					p.y = (y - 1) / 2 * this->resolution;
					ray->push_back(p);
				} else {
					p.x = (x - 1) / 2 * this->resolution;
					p.y = (y - ystep - 1) / 2 * this->resolution;
					ray->push_back(p);

					p.x = (x - xstep - 1) / 2 * this->resolution;
					p.y = (y - 1) / 2 * this->resolution;
					ray->push_back(p);
				}
			}
			p.x = (x - 1) / 2 * this->resolution;
			p.y = (y - 1) / 2 * this->resolution;
			ray->push_back(p);

			errorprev = error;
		}
	} else {
		int errorprev = error = dy;
		for(i = 0; i < dy; i++) {
			y += ystep;
			error += ddx;
			if(error > ddy) {
				x += xstep;
				error -= ddy;
				if(error + errorprev < ddy) {
					p.x = (x - xstep - 1) / 2 * this->resolution;
					p.y = (y - 1) / 2 * this->resolution;
					ray->push_back(p);
				} else if (error + errorprev > ddy) {
					p.x = (x - 1) / 2 * this->resolution;
					p.y = (y - ystep - 1) / 2 * this->resolution;
					ray->push_back(p);
				} else {
					p.x = (x - 1) / 2 * this->resolution;
					p.y = (y - ystep - 1) / 2 * this->resolution;
					ray->push_back(p);

					p.x = (x - xstep - 1) / 2 * this->resolution;
					p.y = (y - 1) / 2 * this->resolution;
					ray->push_back(p);
				}
			}
			p.x = (x - 1) / 2 * this->resolution;
			p.y = (y - 1) / 2 * this->resolution;
			ray->push_back(p);

			errorprev  = error;
		}
	}
}
