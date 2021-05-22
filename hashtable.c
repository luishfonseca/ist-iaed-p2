#include <stdlib.h>
#include <string.h>

#include "hashtable.h"

struct HashTable {
	int table_sz;
	int amt;
	void** ht;
};

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

int hash(char* v, int M) {
	long int h, a = 31415, b = 27183;

	for (h = 0; *v != '\0'; v++, a = a*b % (M-1))
		h = (a*h + *v) % M;
	return h;
}

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

struct HashTable* ht_remove(struct HashTable* ht, void* el, char* (*k)(void*),
                                                     int (*cmp)(void*, void*)) {
	int i = hash(k(el), ht->table_sz);

	while (ht->ht[i] != NULL) {
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
		i = (i + 1) % ht->table_sz;
	}
	return ht;
}

void ht_destroy(struct HashTable* ht) {
	free(ht->ht);
	free(ht);
}
