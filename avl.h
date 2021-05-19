struct AVL;

struct AVL* avl_insert(struct AVL* n, void* el, int (*cmp_els)(void*, void*));
struct AVL* avl_remove(struct AVL* n, void* el, int (*cmp_els)(void*, void*));
void* avl_find(struct AVL* n, void* k, int (*cmp_key_el)(void*, void*));
void avl_traverse(struct AVL* n, void (*visit)(void*));
void avl_destroy(struct AVL* n, void (*free_el)(void*));
