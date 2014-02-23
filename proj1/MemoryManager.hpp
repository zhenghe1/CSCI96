#ifndef MEMORYMANAGER_HPP
#define MEMORYMANAGER_HPP

#include <stdlib.h>
#include <iostream>

/*
 * MemoryManager uses a free list in order to assign memory.
 * Point of this is to decrease the number of context switches between the user space and the kernel, thus increasing performance.
 */

class IMemoryManager {
    public:
        virtual void *allocate(size_t) = 0;
        virtual void free(void *) = 0;
};

class MemoryManager : public IMemoryManager {
    private:
        struct FreeList {
            FreeList *next;
        };

        FreeList *freeListHead;

        void getMoreNodes(size_t, size_t);
        void cleanUp();

    public:
        MemoryManager() : freeListHead(0) {}
        virtual ~MemoryManager() {
            cleanUp();
        }
        virtual void *allocate(size_t);
        virtual void free(void *);

};

/* Allocates memory from free list. Expands memory space if free list is empty. */
inline void *MemoryManager::allocate(size_t size) {
    if (freeListHead == 0) getMoreNodes(256, size);
    FreeList *head = freeListHead;
    freeListHead = head->next;

    return head;
}

/* Adds a deleted node to the free list. */
inline void MemoryManager::free(void *deletedNode) {
    FreeList *head = static_cast<FreeList *>(deletedNode);
    head->next = freeListHead;
    freeListHead = head;
}

/* Expands memory space. */
inline void MemoryManager::getMoreNodes(size_t poolSize, size_t objectSize) {
    size_t size = objectSize > sizeof(FreeList *) ? objectSize : sizeof(FreeList *);
    FreeList *head = reinterpret_cast<FreeList *>(new char[size]);
    freeListHead = head;

    for(int i = 0; i < poolSize; i++) {
        head->next = reinterpret_cast<FreeList *>(new char[size]);
        head = head->next;
    }

    head->next = 0;
}

/* De-allocates the free list. */
inline void MemoryManager::cleanUp() {
    FreeList *nextPtr = freeListHead;
    for(; nextPtr; nextPtr = freeListHead) {
        freeListHead = freeListHead->next;
        delete [] nextPtr;
    }
}

#endif
