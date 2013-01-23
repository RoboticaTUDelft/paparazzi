#ifndef FRONTIER_H
#define FRONTIER_H

#include "Node.h"

/* Frontier is a class that's a Union-Find datastructure with the ability  *
 * to store the centre of 2D frontiers and the amount of frontier cells    */

class Frontier {
  public:
    point2d centre;
    int size, rank;
    Frontier * root;
    point2d p;

    Frontier (const point2d * point) {
        centre.x = p.x = point->x;
        centre.y = p.y = point->y;
        size = 1;
        rank = 0;
        root = this;
    }
};

int funion (Frontier *, Frontier *);    /* union method of Frontier returns 0 when not merged, 1 when merged */

Frontier * find (Frontier *);       	/* returns the root of the Frontier */

int rootSize(Frontier *);							/* returns the size of the whole frontier */

#endif /* FRONTIER_H */
