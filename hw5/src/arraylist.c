#include "arraylist.h"
#include <errno.h>

/**
 * @visibility HIDDEN FROM USER
 * @return     true on success, false on failure
 */
static bool resize_al(arraylist_t* self){
    bool ret = false;

    if(self -> length == self -> capacity){
        self -> base = realloc(self -> base, (self -> capacity * 2));
        self -> capacity = self -> capacity * 2;
        return true;
    }
    else if (self -> length == (self -> capacity / 2) - 1 && self -> capacity != INIT_SZ){
        self -> base = realloc(self -> base, (self->capacity / 2));
        self -> capacity = (self -> capacity / 2);
        return true;
    }
    else
        return ret;
}

arraylist_t *new_al(size_t item_size){
    arraylist_t* arrayStruct = malloc(sizeof(arraylist_t));
    if(arrayStruct == NULL)
        return NULL;;
    arrayStruct -> capacity = INIT_SZ;
    arrayStruct -> length = 0;
    arrayStruct -> base = calloc(INIT_SZ, item_size);
    arrayStruct -> item_size = item_size;
    arrayStruct -> readcnt = 0;
    if(arrayStruct -> base == NULL)
        return NULL;
    if(sem_init(&(arrayStruct -> read_mutex), 0 ,1) == -1)
        return NULL;
    if(sem_init(&(arrayStruct -> write_mutex), 0 ,1) == -1)
        return NULL;

    return arrayStruct;
}

//writer
size_t insert_al(arraylist_t *self, void* data){
    size_t ret = UINT_MAX;

    if(sem_wait(&(self -> write_mutex)) == -1)      //lock writing
        return ret;
    if(self -> capacity == self -> length){
        resize_al(self);
    }

    char* tmp = (char*)(self -> base) + ((self -> length) * (self -> item_size));
    ret = self-> length;
    self -> length++;
    memcpy(tmp, data, self->item_size);

    if(sem_post(&(self -> write_mutex)) == -1)      //unlock writing
        return UINT_MAX;

    return ret;
}

//reader
size_t get_data_al(arraylist_t *self, void *data){
    size_t ret = UINT_MAX;
    if(sem_wait(&(self -> read_mutex)) == -1)       //lock reading
        return ret;
    self -> readcnt++;
    if(self -> readcnt == 1)
        if(sem_wait(&(self -> write_mutex)) == -1)  //if first thread to read, lock writing
            return ret;
    if(sem_post(&(self -> read_mutex)) == -1)       //unlock reading
        return ret;

    int times = self -> length;
    void* ptr = self -> base;
    int size = self -> item_size;
    int i;
    for(i = 0; i < times; i++){
        if(memcmp(data, ptr, size) == 0)
            break;
        ptr = (char*)ptr + size;
    }

    if(sem_wait(&(self -> read_mutex)) == -1)       //lock reading
        return ret;
    self -> readcnt--;
    if(self -> readcnt == 0)
        if(sem_post(&(self -> write_mutex)) == -1)  //if last thread to read, unlock writing
            return ret;

    if(sem_post(&(self -> read_mutex)) == -1)       //unlock reading
        return ret;

    if(i == times){
        return UINT_MAX;
    }
    return i;
}

//reader
void *get_index_al(arraylist_t *self, size_t index){
    void *ret = NULL;

    if(sem_wait(&(self -> read_mutex)) == -1)       //lock reading
        return ret;
    self -> readcnt++;
    if(self -> readcnt == 1)
        if(sem_wait(&(self -> write_mutex)) == -1)  //if first thread to read, lock writing
            return ret;
    if(sem_post(&(self -> read_mutex)) == -1)       //unlock reading
        return ret;

    int offset = self -> item_size * index;
    void* ptr = (char*)(self -> base) + offset;
    void* copy = malloc(self -> item_size);
    memcpy(copy, ptr, self -> item_size);

    if(sem_wait(&(self -> read_mutex)) == -1)       //lock reading
        return ret;
    self -> readcnt--;
    if(self -> readcnt == 0)
        if(sem_post(&(self -> write_mutex)) == -1)  //if last thread to read, unlock writing
            return ret;

    if(sem_post(&(self -> read_mutex)) == -1)       //unlock reading
        return ret;

    return copy;
}

//writer
bool remove_data_al(arraylist_t *self, void *data){
    bool ret = false;

    if(sem_wait(&(self -> write_mutex)) == -1)
        return ret;

    if (self -> length == (self -> capacity / 2) - 1)
        resize_al(self);

    int i;
    void* ptr = self -> base;
    int size = self -> item_size;
    for(i = 0; i < self -> length; i++){
        if(memcmp(data, ptr, size) == 0)
            break;
        ptr = (char*)ptr + size;
    }

    void* src = (char*)ptr + size;
    int bytes = (self -> length - i) * size;
    memmove(ptr, src, bytes);

    if(sem_post(&(self -> write_mutex)) == -1)
        return ret;

    if(i == self  -> length)
        return ret;
    self-> length--;
    return true;
}

//writer
void *remove_index_al(arraylist_t *self, size_t index){
    void *ret = 0;

    if(sem_wait(&(self -> write_mutex)) == -1)
        return ret;

    if (self -> length == (self -> capacity / 2) - 1)
        resize_al(self);

    if(index > self->length){
        if(sem_post(&(self -> write_mutex)) == -1)
            return 0;
        return NULL;
    }

    int size = self -> item_size;
    void* ptr = (char*)(self -> base) + index * size;
    void* src = (char*)ptr + size;
    ret = malloc(size);
    memcpy(ret, ptr, size);
    int bytes = (self -> length - index) * size;
    memmove(ptr, src, bytes);
    self -> length--;

    if(sem_post(&(self -> write_mutex)) == -1)
        return 0;

    return ret;
}

//writer
void delete_al(arraylist_t *self, void (*free_item_func)(void*)){
    if(sem_wait(&(self -> write_mutex)) == -1)
        return ;

    if(self -> length != 0 && free_item_func != NULL){
        void* ptr = self -> base;
        while(self -> length != 0){
            free_item_func(ptr);
            ptr = (char*)ptr + self -> item_size;
            self -> length--;
        }
    }
    free(self->base);
    self->capacity = 0;
    self->length = 0;

    if(sem_post(&(self -> write_mutex)) == -1)
        return;
}
