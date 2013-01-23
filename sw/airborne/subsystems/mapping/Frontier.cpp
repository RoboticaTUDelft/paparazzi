#include "Frontier.h"

void merge(Frontier *, Frontier *);

int funion (Frontier * x, Frontier * y) {
	Frontier * xRoot = find(x);
	Frontier * yRoot = find(y);

	if(xRoot == yRoot) {
		return 0;
	}

	/* add the smallest frontier to the largest,                   *
	 * in case they're equal the frontier (tree) depth incerements */
	if(xRoot->rank < yRoot->rank) {
		merge(yRoot, xRoot);
	} else if (xRoot->rank > yRoot->rank) {
		merge(xRoot, yRoot);
	} else {
		merge(xRoot, yRoot);
		xRoot->rank++;
	}
	return 1;
}

Frontier * find (Frontier * x) {
	if(x->root != x) {
		x->root = find(x->root);
	}
	return x->root;
}

int rootSize (Frontier * x) {
	return find(x)->size;
}

/* merges the 2 frontiers by adjusting the root, size and centre of the frontier */
void merge (Frontier * root, Frontier * branch) {
	branch->root = root;

	int newSize = root->size + branch->size;
	root->centre.x = (root->centre.x * root->size + branch->centre.x * branch->size) / newSize;
	root->centre.y = (root->centre.y * root->size + branch->centre.y * branch->size) / newSize;
	root->size = newSize;
}


