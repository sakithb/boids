#ifndef QUADTREE_H
#define QUADTREE_H

#include <stdbool.h>

#define MAX_SUBLEVELS 6

struct QuadItem {
	void *item;
	float x;
	float y;
};

struct Quad {
	struct QuadItem *items;
	struct Quad *children[4];

	int lvl;

	int items_len;

	bool subdivided;

	float x;
	float y;
	float w;
	float h;
};

void quad_init(struct Quad *q, float x, float y, float w, float h, int lvl);
void quad_insert(struct Quad *q, void *item, float x, float y);
bool quad_is_inside(struct Quad *q, float x, float y);
struct Quad *quad_search(struct Quad *q, float x, float y);

struct QuadPool {
	struct Quad *arr;
	int length;
};


void qt_pool_init(int items_cap);
struct Quad *qt_pool_get(bool root);
void qt_pool_free();

#endif
