#ifndef MEMORYMANAGER_HPP
#define MEMORYMANAGER_HPP

#include <stdlib.h>
#include <iostream>

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
        MemoryManager(size_t size) : freeListHead(0) {
            getMoreNodes(256, size);
        }
        virtual ~MemoryManager() {
            cleanUp();
        }
        virtual void *allocate(size_t);
        virtual void free(void *);

};

inline void *MemoryManager::allocate(size_t size = 24) {
    if (freeListHead == 0) getMoreNodes(256, size);
    FreeList *head = freeListHead;
    freeListHead = head->next;

    return head;
}

inline void MemoryManager::free(void *deletedNode) {
    FreeList *head = static_cast<FreeList *>(deletedNode);
    head->next = freeListHead;
    freeListHead = head;
}

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

inline void MemoryManager::cleanUp() {
    FreeList *nextPtr = freeListHead;
    for(; nextPtr; nextPtr = freeListHead) {
        freeListHead = freeListHead->next;
        delete [] nextPtr;
    }
}

#endif
