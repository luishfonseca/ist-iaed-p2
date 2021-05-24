/*
 * File:	avl.c
 * Author:	Luís Fonseca, 99266
 * Desc:	AVL implementation.
 */

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "avl.h"

/************************************************
 * AVL NODE:
 * - l: left child.
 *
 * - r: right child.
 *
 * - h: height of node.
 *
 * - el: element associated to node.
 *************************************************/
struct AVL {
	struct AVL *l, *r;
	int h;
	void* el;
};

/*
 * NEW NODE: Creates a new AVL tree node.
 */
struct AVL* new_node(void* el, struct AVL* l, struct AVL* r) {
	struct AVL* n = malloc(sizeof(struct AVL));
	if (n == NULL)
		return NULL;
	n->el = el;
	n->l = l;
	n->r = r;
	n->h = 1;
	return n;
}

/*
 * HEIGHT: Returns the height of a given node.
 */
int height(struct AVL* n) {
	if (n == NULL)
		return 0;
	return n->h;
}

/*
 * COMPUTE HEIGHT: Calculates the height by
 *    looking at the node's children.
 */
void compute_height(struct AVL* n) {
	int hl, hr;

	hl = height(n->l);
	hr = height(n->r);
	n->h = hl > hr ? hl + 1 : hr + 1;
}

/*
 * ROTATION LEFT
 */
struct AVL* rot_l(struct AVL* n) {
	struct AVL* x = n->r;
	n->r = x->l;
	x->l = n;

	compute_height(n);
	compute_height(x);

	return x;
}

/*
 * ROTATION RIGHT
 */
struct AVL* rot_r(struct AVL* n) {
	struct AVL* x = n->l;
	n->l = x->r;
	x->r = n;

	compute_height(n);
	compute_height(x);

	return x;
}

/*
 * ROTATION LEFT-RIGHT
 */
struct AVL* rot_lr(struct AVL* n) {
	if (n == NULL)
		return NULL;

	n->l = rot_l(n->l);
	return rot_r(n);
}

/*
 * ROTATION RIGHT-LEFT
 */
struct AVL* rot_rl(struct AVL* n) {
	if (n == NULL)
		return NULL;

	n->r= rot_r(n->r);
	return rot_l(n);
}

/*
 * BALANCE FACTOR: Calculates the balance factor
 *    of a node.
 */
int balance_factor(struct AVL* n) {
	if (n == NULL)
		return 0;

	return height(n->l) - height(n->r);
}

/*
 * MAX: Returns the max value in the AVL.
 */
struct AVL* max(struct AVL* n) {
	while(n != NULL && n->r != NULL)
		n = max(n->r);
	return n;
}

/*
 * BALANCE: Balances node.
 */
struct AVL* balance(struct AVL* n) {
	int bf;

	if (n == NULL)
		return NULL;

	bf = balance_factor(n);

	if (bf > 1) {
		if (balance_factor(n->l) >= 0)
			n = rot_r(n);
		else
			n = rot_lr(n);
	} else if (bf < -1) {
		if (balance_factor(n->r) <= 0)
			n = rot_l(n);
		else
			n = rot_rl(n);
	} else {
		compute_height(n);
	}

	return n;
}

/*
 * AVL FIND: Returns the element associated to the
 *    node found using a given both the key and a
 *    comparison function.
 */
void* avl_find(struct AVL* n, void* k, int (*cmp_key_el)(void*, void*)) {
	if (n == NULL)
		return NULL;

	if (cmp_key_el(k, n->el) == 0)
		return n->el;
	else if (cmp_key_el(k, n->el) < 0)
		return avl_find(n->l, k, cmp_key_el);
	else
		return avl_find(n->r, k, cmp_key_el);
}

/*
 * AVL INSERT: Insert a given element into the AVL
 *    using a given comparison function.
 */
struct AVL* avl_insert(struct AVL* n, void* el, int (*cmp_els)(void*, void*)) {
	if (n == NULL)
		return new_node(el, NULL, NULL);

	if (cmp_els(el, n->el) < 0) {
		n->l = avl_insert(n->l, el, cmp_els);
		if (n->l == NULL)
			return NULL;
	} else {
		n->r = avl_insert(n->r, el, cmp_els);
		if (n->r == NULL)
			return NULL;
	}

	return balance(n);
}

/*
 * AVL REMOVE: Remve a given element from the AVL
 *    using a given comparison function.
 */
struct AVL* avl_remove(struct AVL* n, void* el, int (*cmp_els)(void*, void*)) {
	if (n == NULL)
		return NULL;

	if (cmp_els(el, n->el) < 0)
		n->l = avl_remove(n->l, el, cmp_els);
	else if (cmp_els(el, n->el) > 0)
		n->r = avl_remove(n->r, el, cmp_els);
	else {
		if (n->l != NULL && n->r != NULL) {
			n->el = max(n->l)->el;
			n->l = avl_remove(n->l, n->el, cmp_els);
		} else {
			struct AVL* aux = n;
			if (n->l != NULL)
				n = n->l;
			else
				n = n->r;
			free(aux);
		}
	}

	return balance(n);
}

/*
 * AVL TRAVERSE: Traverse the AVL in order and run
 *    the given function on every element.
 */
void avl_traverse(struct AVL* n, void (*visit)(void*, void*), void* extra) {
	if (n != NULL) {
		avl_traverse(n->l, visit, extra);
		visit(n->el, extra);
		avl_traverse(n->r, visit, extra);
	}
}

/*
 * AVL DESTROY: Free the AVL
 */
void avl_destroy(struct AVL* n) {
	if (n != NULL) {
		avl_destroy(n->l);
		avl_destroy(n->r);
		free(n);
	}
}
