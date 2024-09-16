#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include "quadtree.h"

void quad_init(struct Quad *q, float x, float y, float w, float h, int lvl) {
	q->x = x;
	q->y = y;
	q->w = w;
	q->h = h;
	q->lvl = lvl;
	q->subdivided = false;
	q->items_len = 0;
}

void quad_insert(struct Quad *q, void *item, float x, float y) {
	assert(quad_is_inside(q, x, y));

	if (q->items_len < 4 || q->lvl == MAX_SUBLEVELS) {
		q->items[q->items_len++] = (struct QuadItem){item, x, y};
	} else {
		float w = q->w/2;
		float h = q->h/2;
		int lvl = q->lvl+1;

		q->children[0] = qt_pool_get(false);
		quad_init(q->children[0], q->x, q->y, w, h, lvl);
		q->children[1] = qt_pool_get(false);
		quad_init(q->children[1], q->x + w, q->y, w, h, lvl);
		q->children[2] = qt_pool_get(false);
		quad_init(q->children[2], q->x + w, q->y + h, w, h, lvl);
		q->children[3] = qt_pool_get(false);
		quad_init(q->children[3], q->x, q->y + h, w, h, lvl);

		if (quad_is_inside(q->children[0], x, y)) {
			quad_insert(q->children[0], item, x, y);
		} else if (quad_is_inside(q->children[1], x, y)) {
			quad_insert(q->children[1], item, x, y);
		} else if (quad_is_inside(q->children[2], x, y)) {
			quad_insert(q->children[2], item, x, y);
		} else if (quad_is_inside(q->children[3], x, y)) {
			quad_insert(q->children[3], item, x, y);
		}

		for (int i = 0; i < q->items_len; i++) {
			struct QuadItem item = q->items[i];

			if (quad_is_inside(q->children[0], item.x, item.y)) {
				quad_insert(q->children[0], item.item, item.x, item.y);
			} else if (quad_is_inside(q->children[1], item.x, item.y)) {
				quad_insert(q->children[1], item.item, item.x, item.y);
			} else if (quad_is_inside(q->children[2], item.x, item.y)) {
				quad_insert(q->children[2], item.item, item.x, item.y);
			} else if (quad_is_inside(q->children[3], item.x, item.y)) {
				quad_insert(q->children[3], item.item, item.x, item.y);
			}
		}

		q->subdivided = true;
	}
}

bool quad_is_inside(struct Quad *q, float x, float y) {
	return x >= q->x && y >= q->y && x < q->x + q->w && y < q->y + q->h;
}

struct Quad *quad_search(struct Quad *q, float x, float y) {
	assert(quad_is_inside(q, x, y));

	if (!q->subdivided || q->lvl == MAX_SUBLEVELS) {
		return q;
	}

	if (x < q->x + (q->w / 2)) {
		if (y < q->y + (q->h / 2)) {
			return quad_search(q->children[0], x, y);
		} else {
			return quad_search(q->children[3], x, y);
		}
	} else {
		if (y < q->y + (q->h / 2)) {
			return quad_search(q->children[1], x, y);
		} else {
			return quad_search(q->children[2], x, y);
		}
	}
}

struct QuadPool qp;

void qt_pool_init(int items_cap) {
	int quads_len = 1;

	for (int i = 1; i <= MAX_SUBLEVELS; i++) {
		quads_len += pow(4, i);
	}

	qp.length = 0;
	qp.arr = calloc(quads_len, sizeof(struct Quad));
	if (qp.arr == NULL) {
		fprintf(stderr, "Error while allocating quad pool");
		abort();
	}

	for (int i = 0; i < quads_len; i++) {
		struct Quad *q = &qp.arr[i];

		q->items = calloc(items_cap, sizeof(struct QuadItem));
		if (q->items == NULL) {
			fprintf(stderr, "Error while allocating quad items");
			abort();
		}
	}
}

struct Quad *qt_pool_get(bool root) {
	if (root) {
		qp.length = 0;
	}

	return &qp.arr[qp.length++];
}

void qt_pool_free() {
	int quads_len = 1;

	for (int i = 1; i <= MAX_SUBLEVELS; i++) {
		quads_len += pow(4, i);
	}

	for (int i = 0; i < quads_len; i++) {
		struct Quad q = qp.arr[i];
		free(q.items);
	}

	free(qp.arr);
	qp.length = 0;
	qp.arr = NULL;
}

