#include "memory_manager.h"

inline void *MemoryManager::allocate(const size_t size) {
    if(freeListHead == 0) {
        expandPoolSize(size);
    }

    FreeList *head = freeListHead;
    freeListHead = head->next;
    return head;
}

inline void MemoryManager::free(void *toFree) {
    FreeList *head = static_cast<FreeList *>(toFree);
    head->next = freeListHead;
    freeListHead = head;
}

void MemoryManager::expandPoolSize(const size_t objSize, int poolSize) {
    size_t size = (objSize > sizeof(FreeList *)) ? objSize : sizeof(FreeList *);
    FreeList *head = reinterpret_cast<FreeList *>(new char[size]);
    freeListHead = head;

    for(int i = 0; i < poolSize; ++i) {
        head->next = reinterpret_cast<FreeList *>(new char[size]);
        head = head->next;
    }

    head->next = 0;
}

void MemoryManager::cleanUp() {
    FreeList *currPtr = freeListHead;
    for(; currPtr; currPtr = freeListHead) {
        freeListHead = freeListHead->next;
        delete[] currPtr;
    }
}
