/*
 * File:	hashtable.h
 * Author:	Luís Fonseca, 99266
 * Desc:	This header exposes the hashtable interface.
 */

struct HashTable;

struct HashTable* ht_insert(struct HashTable* ht, void* el, char* (*k)(void*));
struct HashTable* ht_remove(struct HashTable* ht, void* el, char* (*k)(void*),
                                                     int (*cmp)(void*, void*));
void* ht_search(struct HashTable* ht, char* v, char* (*k)(void*),
                                     int (*better)(void*, void*));
void ht_destroy(struct HashTable* ht);
