#include "subsystems/mapping/Mapping.h"
#include <climits>

//TODO: MapPOs
//TODO: Travel
//TODO: Scan

/* exploration states */
#define SCANNING                   0 
#define DELETE_FRONTIERS           1
#define FIND_FRONTIER_NODES        2
#define BUILD_FRONTIERGRID         3
#define CLUSTER_FRONTIERS          4
#define FIND_PATHS                 5
#define FIND_BEST_FRONTIER         6
#define TRAVELLING                 7

using namespace std;

void Mapping::mappingStep () {
	std::list<point2d>::iterator i;
	switch(state) {
	case SCANNING:
		/* TODO: scan 360 degrees, add all points to octomap and put them in scannedPoints */
		state++;
		break;
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
		int j, k;
		int size = map->getSize();
		for(j = 0; j < size; j++) {
			for(k = 0; k < size; k++) {
				point2d p;
				p.x = k * map->getResolution();
				p.y = j * map->getResolution();
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
		break;
	case TRAVELLING:
		/*if(goal == currentPos)
        state = SCANNING;
        paths.clear();*/
		break;
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

