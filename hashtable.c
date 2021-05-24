/*
 * File:	hashtable.c
 * Author:	Luís Fonseca, 99266
 * Desc:	HashTable implementation.
 */

#include <stdlib.h>
#include <string.h>
#include "hashtable.h"

#define INITIAL_SZ 13

/************************************************
 * HASHTABLE:
 * - table_sz: Current size of the table.
 *
 * - amt: Amount of elements in the table.
 *
 * - ht: Table of elements.
 *************************************************/
struct HashTable {
	int table_sz;
	int amt;
	void** ht;
};

/*
 * NEW TABLE: Creates a new hashtable.
 */
struct HashTable* new_table(int max) {
	int i;
	struct HashTable* ht = malloc(sizeof(struct HashTable));

	if (ht == NULL)
		return NULL;

	ht->table_sz = 2 * max;
	ht->amt = 0;

	ht->ht = malloc(ht->table_sz * sizeof(void*));

	if (ht->ht == NULL)
		return NULL;

	for (i = 0; i < ht->table_sz; i++)
		ht->ht[i] = NULL;

	return ht;
}

/*
 * HASH: Returns the sting's hash.
 */
int hash(char* v, int M) {
	unsigned int h, a = 31415, b = 27183;

	for (h = 0; *v != '\0'; v++, a = a*b % (M-1))
		h = (a*h + *v) % M;
	return h;
}

/*
 * EXPAND: Doubles the size of the table and
 *    reashes the elements.
 */
struct HashTable* expand(struct HashTable* ht, char* (*k)(void*)) {
	int i;
	struct HashTable* new_ht = new_table(ht->table_sz * 2);

	if (new_ht == NULL)
		return NULL;

	for (i = 0; i < ht->table_sz; i++)
		if (ht->ht[i] != NULL) {
			new_ht = ht_insert(new_ht, ht->ht[i], k);
			if (new_ht == NULL)
				return NULL;
		}

	ht_destroy(ht);
	return new_ht;
}

/*
 * HASHTABLE INSERT: Insert an element into the
 *    table by hashing the key gotten with the
 *    given key function.
 */
struct HashTable* ht_insert(struct HashTable* ht, void* el, char* (*k)(void*)) {
	int i;

	if (ht == NULL) {
		ht = new_table(INITIAL_SZ);
		if (ht == NULL)
			return NULL;
	}

	i = hash(k(el), ht->table_sz);

	while (ht->ht[i] != NULL)
		i = (i + 1) % ht->table_sz;
	ht->ht[i] = el;

	if (++ht->amt * 2 > ht->table_sz) {
		ht = expand(ht, k);
		if (ht == NULL)
			return NULL;
	}

	return ht;
}

/*
 * HASHTABLE SEARCH: Returns the element with the
 *    given key, when there are multiple
 *    candidates uses the given "better" function
 *    to pick the best one.
 */
void* ht_search(struct HashTable* ht, char* v, char* (*k)(void*),
                                     int (*better)(void*, void*)) {
	int i = hash(v, ht->table_sz);
	void* el = NULL;

	while (ht->ht[i] != NULL) {
		if (strcmp(v, k(ht->ht[i])) == 0 && better(ht->ht[i], el))
				el = ht->ht[i];
		i = (i + 1) % ht->table_sz;
	}

	return el;
}

/*
 * HASHTABLE REMOVE: Removes a given element from
 *    the table.
 */
struct HashTable* ht_remove(struct HashTable* ht, void* el, char* (*k)(void*),
                                                     int (*cmp)(void*, void*)) {
	int i = hash(k(el), ht->table_sz);

	while (ht->ht[i] != NULL) {
		/* Two elements with the same key might not be
		 * the same so the given cmp function is used */
		if (cmp(el, ht->ht[i]) == 0) {
			ht->ht[i] = NULL;
			--ht->amt;
			break;
		}
		i = (i + 1) % ht->table_sz;
	}

	i = (i + 1) % ht->table_sz;

	while (ht->ht[i] != NULL) {
		void* aux = ht->ht[i];
		ht->ht[i] = NULL;
		--ht->amt;
		ht = ht_insert(ht, aux, k);
		/* No need the verify if the allocation was sucessful
		 * since space in the table has just been freed. */
		i = (i + 1) % ht->table_sz;
	}
	return ht;
}

/*
 * HASHTABLE DESTROY: Free the hashtable.
 */
void ht_destroy(struct HashTable* ht) {
	free(ht->ht);
	free(ht);
}
