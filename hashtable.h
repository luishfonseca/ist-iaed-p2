#define INITIAL_SZ 13

struct HashTable;

struct HashTable* ht_insert(struct HashTable* ht, void* el, char* (*k)(void*));
struct HashTable* ht_remove(struct HashTable* ht, void* el, char* (*k)(void*),
                                                     int (*cmp)(void*, void*));
void* ht_search(struct HashTable* ht, char* v, char* (*k)(void*),
                                     int (*better)(void*, void*));
void ht_destroy(struct HashTable* ht);
