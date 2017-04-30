#include "debug.h"
#include "arraylist.h"
#include "foreach.h"

pthread_key_t iterator;
static pthread_once_t once = PTHREAD_ONCE_INIT;

void init_key(){
    if(pthread_key_create(&iterator, NULL) != 0)
        return;
}

void *foreach_init(arraylist_t *self){
    void *ret = NULL;
    if(pthread_key_create(&iterator, NULL) != 0)
        return ret;
    pthread_once(&once, init_key);
    int* num = malloc(sizeof(int));
    *num = 0;
    if(pthread_setspecific(iterator, num) != 0)
        return ret;

    if(self->length == 0)
        return NULL;
    return get_index_al(self, 0);

    return ret;
}

void *foreach_next(arraylist_t *self, void* data){
    void *ret = NULL;

    if(self-> length == 0)
        return ret;

    void* indexPtr = pthread_getspecific(iterator);
    if(indexPtr == NULL)
        return ret;

    int index = *(int*)indexPtr;
    if(index = self->length)
        return NULL;
    if(data != NULL){
        void* ptr = (char*)(self->base) + index * self->item_size;
        memcpy(ptr, data, self->item_size);
    }

    free(indexPtr);
    index++;
    int* newIdxPtr = malloc(sizeof(int));
    *newIdxPtr = index;
    if(pthread_setspecific(iterator, newIdxPtr) != 0)
        return ret;

    return NULL;
}

size_t foreach_index(){
    size_t ret = UINT_MAX;

    void* indexPtr = pthread_getspecific(iterator);
    if(indexPtr == NULL)
        return ret;

    return *(int*)indexPtr;
}

bool foreach_break_f(){
    if(pthread_getspecific(iterator) != NULL)
        free(pthread_getspecific(iterator));
    pthread_setspecific(iterator, NULL);
    return true;
}

int apply(void *self, int (*application)(void*)){
    int32_t ret = 0;

    void* currPtr = foreach_init(self);
    while(currPtr != NULL){
        if(application(currPtr) == 0)
            currPtr = foreach_next(self, currPtr);
        else
            currPtr = foreach_next(self, NULL);
    }

    return ret;
}
