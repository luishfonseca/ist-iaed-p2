#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "avl.h"

struct AVL {
	struct AVL *l, *r;
	int h;
	void* el;
};

struct AVL* new(void* el, struct AVL* l, struct AVL* r) {
	struct AVL* n = malloc(sizeof(struct AVL));
	n->el = el;
	n->l = l;
	n->r = r;
	n->h = 1;
	return n;
}

int height(struct AVL* n) {
	if (n == NULL)
		return 0;
	return n->h;
}

void compute_height(struct AVL* n) {
	int hl, hr;

	hl = height(n->l);
	hr = height(n->r);
	n->h = hl > hr ? hl + 1 : hr + 1;
}


struct AVL* rot_l(struct AVL* n) {
	struct AVL* x = n->r;
	n->r = x->l;
	x->l = n;

	compute_height(n);
	compute_height(x);

	return x;
}

struct AVL* rot_r(struct AVL* n) {
	struct AVL* x = n->l;
	n->l = x->r;
	x->r = n;

	compute_height(n);
	compute_height(x);

	return x;
}

struct AVL* rot_lr(struct AVL* n) {
	if (n == NULL)
		return NULL;

	n->l = rot_l(n->l);
	return rot_r(n);
}

struct AVL* rot_rl(struct AVL* n) {
	if (n == NULL)
		return NULL;

	n->r= rot_r(n->r);
	return rot_l(n);
}

int balance_factor(struct AVL* n) {
	if (n == NULL)
		return 0;

	return height(n->l) - height(n->r);
}

struct AVL* max(struct AVL* n) {
	while(n != NULL && n->r != NULL)
		n = max(n->r);

	return n;
}

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

struct AVL* avl_insert(struct AVL* n, void* el, int (*cmp_els)(void*, void*)) {
	if (n == NULL)
		return new(el, NULL, NULL);

	if (cmp_els(el, n->el) < 0)
		n->l = avl_insert(n->l, el, cmp_els);
	else if (cmp_els(el, n->el) > 0)
		n->r = avl_insert(n->r, el, cmp_els);
	else
		printf("Attempted to insert duplicate!");

	return balance(n);
}

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

void avl_traverse(struct AVL* n, void (*visit)(void*)) {
	if (n != NULL) {
		avl_traverse(n->l, visit);
		visit(n->el);
		avl_traverse(n->r, visit);
	}
}

void avl_traverse_post(struct AVL* n, void (*visit)(void*)) {
	if (n != NULL) {
		avl_traverse_post(n->l, visit);
		avl_traverse_post(n->r, visit);
		visit(n->el);
	}
}

void avl_delete(struct AVL* n) {
	if (n != NULL) {
		avl_delete(n->l);
		avl_delete(n->r);
		free(n);
	}
}
