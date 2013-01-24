#include "subsystems/mapping/Mapping.h"
#include <climits>
#include <cmath>

//TODO: Travel
//TODO: Scan

/* exploration states */
#define INIT                       0
#define SCANNING                   1 
#define DELETE_FRONTIERS           2
#define FIND_FRONTIER_NODES        3
#define BUILD_FRONTIERGRID         4
#define CLUSTER_FRONTIERS          5
#define FIND_PATHS                 6
#define FIND_BEST_FRONTIER         7
#define TURN_TO_WAYPOINT           8
#define TRAVEL_TO_WAYPOINT         9

using namespace std;

void Mapping::mappingStep () {
	std::list<point2d>::iterator i;
	switch(state) {
	case INIT: {
		index = 0;
		startHeading = pos.getHeading();
		goalHeading = startHeading + M_PI_4;	
		break;
	}
	case SCANNING: {
		if(turn()) {
			state++;
		}
		break;
	}
	case DELETE_FRONTIERS: {
		if(!frontierPoints.empty()) {
			for(i = frontierPoints.begin(); i != frontierPoints.end(); i++) {
				if(!isFrontier(&(*i)))
					frontierPoints.erase(i);
				int s = map->getSize();
				point2d sc = scale(&(*i));
				delete frontiergrid[sc.y * s + sc.x];
			}
		}
		state++;
		break;
	}
	case FIND_FRONTIER_NODES: {
#ifdef SCAN_RANGE
		float scan_range = SCAN_RANGE;
#else 		
		float scan_range = 1;
#endif	
		float j, k;
		for(j = -scan_range; j < scan_range; j += map->getResolution()) {
			for(k = -scan_range; k < scan_range; k += map->getResolution()) {
				point2d p;
				p.x = pos.getPos()->x + k;
				p.y = pos.getPos()->y + j;
				if(isFrontier(&p)) {
					frontierPoints.push_back(p);
				}
			}
		}
		state++;
		break;
	}
	case BUILD_FRONTIERGRID: {
		frontiergrid.clear();
		int size = map->getSize();
		frontiergrid.resize(size * size);
		for(i = frontierPoints.begin(); i != frontierPoints.end(); i++) {
			point2d scaled = scale(&(*i));
			frontiergrid[scaled.y * size + scaled.x] = new Frontier (&(*i));
		}
		state++;
		break;
	}
	case CLUSTER_FRONTIERS: {
		clusterFrontiers(&frontierPoints, &frontiergrid, 2, 3, 5 /* TODO: goeie threshold verzinnen */);
		state++;
		index = 0;
		ifrontiers = frontiers.begin();
		break;
	}
	case FIND_PATHS: {
		if(index < frontiers.size()) {
			tile goal;
			goal.pos = (*ifrontiers)->centre;
			Astar pathfinder (&goal/*TODO: curret position */,&goal, map);
			int msg = pathfinder.findPath();
			if(msg == 1) {
				paths.push_back(pathfinder.getPath());
				index++;
				ifrontiers++;
			} else if(msg == -1) {
				index++;
				ifrontiers++;
			}
		} else state++;
		break;
	}
	case FIND_BEST_FRONTIER:
		index = bestFrontier();
		if(&(paths[index]->back()->pos) != pos.getPos()) {
			int j;
			list<Frontier *>::iterator it;
			for(j = 0, it = frontiers.begin(); (unsigned) j != index; j++) {
				it++;
			}
			frontiers.erase(it);
		} else {		
			setGoalHeading(pos.getPos(), &(paths[index]->front()->pos));
			state++;
		}
		break;
	case TURN_TO_WAYPOINT: {
		if(turn()) {
			startPos = pos.getPos();
			state++;
		}
		break;
	}
	case TRAVEL_TO_WAYPOINT: {
		point2d goal = paths[index]->front()->pos;
		if(passedGoal(startPos, &goal)) {
			mapping_navigation_stop();
			paths[index]->pop_front();
			if(paths[index]->empty()) {
				state = INIT;
			} else {
				setGoalHeading(pos.getPos(), &(paths[index]->front()->pos));
				state--;
			}
		} else {
			mapping_navigation_forward();
		}
		break;
	}
	}
}

int Mapping::isFrontier (const point2d * point) {
	int i, j;
	tile * result = map->get(point);
	/*the supposed frontier has to be a free node */
	if(result && !result->occupied) {
		/*check if there are any unknown nodes around */
		for(i = -1; i < 2 && result; i++) {
			for(j = -1; j < 2 && result; j++) {
				if(i != 0 || j != 0) {
					point2d p;
					p.x = point->x + i * map->getResolution();
					p.y = point->y + j * map->getResolution();
					result = map->get(&p);
				}
			}
		}
		return !result;
	}
	return 0;
}

void Mapping::clusterFrontiers (const std::list<point2d> * frontierpoints,
		std::vector<Frontier *> * frontiergrid, int rows, int columns, int threshold) {
	std::list<Frontier *> clusters;

	std::list<point2d>::const_iterator ifrontierpoints;
	std::list<Frontier *>::iterator iclusters;
	/* addToClusters is a bool that is maintained to see whether thisFrontier is already in the list of clusters or not */
	bool addToClusters;
	for(ifrontierpoints = frontierpoints->begin(); ifrontierpoints != frontierpoints->end(); ifrontierpoints++) {
		/* doubleListed tells us how many times thisFrontier and a ThatFrontier were already in clusters */
		int doubleListed = 0;
		/* get current frontier */
		point2d pos = scale(&(*ifrontierpoints));
		Frontier * thisFrontier = (*frontiergrid) [pos.y * columns + pos.x];
		/* a Frontier will already be in clusters if its size is above threshold */
		bool thisInClusters = addToClusters = rootSize(thisFrontier) <= threshold;

		/* check surrounding nodes for other frontiers and put these in the same cluster */
		int i, j;
		for(i = -1; i < 2; i++) {
			for(j = -1; j < 2; j++) {
				if(pos.x + i >= 0 && pos.x + i < columns && pos.y + j >= 0 && pos.y + j < rows) {
					Frontier * thatFrontier = (*frontiergrid) [(pos.y + j) * columns + (pos.x + i)];
					if((i != 0 || j != 0) && thatFrontier) {
						/* if thatFrontier size is above threshold it's already in clusters */
						bool thatInClusters = rootSize(thatFrontier) > threshold;

						if(funion(thisFrontier, thatFrontier) && thatInClusters && thisInClusters)
							doubleListed++;

						addToClusters = addToClusters && !thatInClusters;
					}
				}
			}
		}
		/* thisFrontier should be added if it should be added so far and size is above threshold */
		if(addToClusters && rootSize(thisFrontier) > threshold) clusters.push_back (thisFrontier);
		/* remove all redundant Frontiers */
		Frontier * thisRoot = find(thisFrontier);
		for (iclusters = clusters.begin(); doubleListed; iclusters++) {
			if(thisRoot == find(*iclusters)) {
				clusters.erase(iclusters);
				doubleListed--;
			}
		}
	}

	frontiers = clusters;
}

int Mapping::bestFrontier () {
	int maxAmount = -1;
	int minDistance = INT_MAX;
	unsigned int i = 0;
	std::list<Frontier *>::const_iterator ifrontiers;
	/*find max amount and minimum distance, to be used for normalisation */
	for(ifrontiers = frontiers.begin(); i < frontiers.size(); ifrontiers++, i++) {
		maxAmount = (maxAmount >= rootSize(*ifrontiers)) ? maxAmount : rootSize(*ifrontiers);;
		minDistance = (minDistance <= paths.at(i)->back()->g) ? minDistance : paths.at(i)->back()->g;
	}

	int maxValue = -1;
	int maxIndex = -1;
	/*finding the best frontier */
	for(i = 0; i < frontiers.size(); i++) {
		/*Normalize the amounts and distances and add them (smaller distance is better, thus 1/x) */
		float temp = rootSize(*ifrontiers) / (maxAmount * 1.0f) + minDistance / paths.at(i)->back()->g;
		if(maxValue < temp) {
			maxValue = temp;
			maxIndex = i;
		}
	}
	/* return the frontier with the best value */
	return maxIndex;
}
std::list<Frontier *> * Mapping::getFrontiers() {
	return &frontiers;
}

point2d Mapping::scale(const point2d * p) {
	point2d scale;
	point2d min = map->getMin();
	scale.x = (p->x - min.x) / map->getResolution();
	scale.y = (p->y - min.y) / map->getResolution();
	return scale;
}

void Mapping::setGoalHeading(const point2d * start, const point2d * destination) {
	if(start->y != destination->y) {
		float xdif = destination->x - start->x;
		float ydif = destination->y - start->y;
		goalHeading = atan(xdif / ydif);
	} else if(start->x < destination->x) {
		goalHeading = M_PI_2;
	} else goalHeading = M_PI_2 * 3;
}

bool_t Mapping::turn() {
	if(goalHeading - startHeading <= M_PI_2) {
		/* pos.getHeading() will be smaller than startHeading when it has passed the 0 clockwise */
		float corr = 0;
		if(pos.getHeading() < startHeading) { 
			float corr = 2 * M_PI;
		}
		/* turn right while haven passed goalHeading */
		if(pos.getHeading() + corr < goalHeading) {			
			mapping_navigation_yaw_right();		
			return false;
		} else return true;
	} else {
		/* pos.getHeading() will be larger than startHeading when it has passed the 0 counter clockwise */
		float corr = 0;
		if(pos.getHeading() > startHeading) {
			float corr = 2 * M_PI;
		}
		/* turn right while havent passed goalHeading */
		if(pos.getHeading() - corr > goalHeading) {
			mapping_navigation_yaw_left();
			return false;
		} else return true;
	}
}	

bool_t Mapping::passedGoal(const point2d * start, const point2d * goal) {
	if(start->x < goal->x) {
		return pos.getPos()->x >= goal->x;
	} else if (start->x > goal->x) {
		return pos.getPos()->x <= goal->x;
	} else if (start->y < goal->y) {
		return pos.getPos()->y >= goal->y;
	} else /* if (start->y > goal->y) */ { 
		return pos.getPos()->y <= goal->y;
	}
}

