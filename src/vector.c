#include <stdio.h>
#include <stdlib.h>
#include "vector.h"

void vector_init(vector *v) {
    v->capacity = VECTOR_INIT_CAPACITY;
    v->total = 0;
    v->items = malloc(sizeof(void *) * v->capacity);
}

int vector_total(vector *v) {
    return v->total;
}

static void vector_resize(vector *v, int capacity) {
    #ifdef DEBUG_ON
        ILOG("vector_resize: %d to %d\n", v->capacity, capacity);
    #endif

    void **items = realloc(v->items, sizeof(void *) * capacity);
    if (items) {
        v->items = items;
        v->capacity = capacity;
    }
}

void vector_add(vector *v, void *item) {
    // ILOG("Size of item : %d\n", sizeof(item));
    // ILOG("Size of item *: %d\n", sizeof(*item));
    if (v->capacity == v->total)
        vector_resize(v, v->capacity * 2);
    v->items[v->total++] = item;
}

void vector_set(vector *v, int index, void *item) {
    if (index >= 0 && index < v->total)
        v->items[index] = item;
}
void *vector_get(vector *v, int index) {
    if (index >= 0 && index < v->total)
        return v->items[index];
    return NULL;
}

/**
 * @brief           this funcation will delete an element from the vector(double pointer)
 * 
 * @param v         pointer to vector 
 * @param index     index to delete
 * @param isDynamic this variable will indicate if the pointer to be freed was allocated via dynamic memory allocation 
 *                  or its just and static, in case it is dynamic the memory allocated would be freed first and then pointer to be freed
 */
void vector_delete(vector *v, int index, char isDynamic) {
    if (index < 0 || index >= v->total)
        return;
    if(isDynamic){
        free(v->items[index]);
    }
    v->items[index] = NULL;
    int i;
    for (i = index; i < v->total - 1; i++) {
        v->items[i] = v->items[i + 1];
        v->items[i + 1] = NULL;
    }
    v->total--;
    if (v->total > 0 && v->total == v->capacity / 4)
        vector_resize(v, v->capacity / 2);
}


/**
 * @brief           this funcation will free the vector and dynamic allocated memory pointed by each element
 * 
 * @param v         pointer to vector 
 * @param isDynamic this variable will indicate if the pointer to be freed was allocated via dynamic memory allocation 
 *                  or its just and static, in case it is dynamic the memory allocated would be freed first and then pointer to be freed
 */
void vector_free(vector *v, char isDynamic) {
    int i = 0;
    if(isDynamic){
        for (i = 0; i < v->total - 1; i++) {
            free(v->items[i]); //free up the memory allocated by the pointer (in our case for assoc if it is allocated by malloc or calloc)
            v->items[i] = NULL;
        }
    }
    free(v->items);
}


// int main(void){
//     vector v;
//     int a  = 10;
//     vector_init(&v);
//     vector_add(&v,&a);
//     vector_add(&v,"hello world this is the test of size");
//     vector_add(&v,"hello world this is the test of size");
//     vector_add(&v,"hello world this is the test of size");
//     vector_add(&v,"hello world this is the test of size");
//     vector_add(&v,"hello world this is the test of size");
//     vector_add(&v,"hello world this is the test of size");
//     vector_add(&v,"hello world this is the test of size");
//     vector_add(&v,"hello world this is the test of size");
//     vector_add(&v,"hello world this is the test of size");
//     vector_add(&v,"hello world this is the test of size");
//     vector_add(&v,"hello world this is the test of size");
//     vector_add(&v,"hello world this is the test of size");
//     vector_add(&v,"hello world this is the test of size");
//     vector_add(&v,"hello world this is the test of size");
//     vector_add(&v,"hello world this is the test of size");
//     vector_add(&v,"hello world this is the test of size");
//     vector_add(&v,"hello world this is the test of size");
//     vector_add(&v,"hello world this is the test of size");
//     vector_add(&v,"hello world this is the test of size");
//     vector_add(&v,"hello world this is the test of size");
//     a = 30;
//     ILOG("Vector implementation: %d\r\n",*(int *)v.items[0]);
//     ILOG("Vector implementation: %s\r\n",v.items[1]);
//     ILOG("cap: %d ptr size: %ld\r\n",v.capacity,sizeof(void *));
    
// }
