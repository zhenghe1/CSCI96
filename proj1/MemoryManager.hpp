#ifndef MEMORYMANAGER_HPP
#define MEMORYMANAGER_HPP

/*
 * These classes are for memory management via a free list. Implementation is very similar to the one on IBM's developer tutorial guides. 
 * Methods to allocate and free nodes for the tree class.
 * Does so in a way that it limits the number of context switches between the user and the kernel.
 */

class IMemoryManager {
    public:
        virtual void *allocate(size_t) = 0;
        virtual void free(voide *) = 0;
}

class MemoryManager : public IMemoryManager {
    public:
        MemoryManager() : freeListHead(0) {
            getMoreNodes();
        }
        virtual ~MemoryManager() {
            cleanUp();
        }
        virtual void *allocate(size_t);
        virtual void free(void *);

    private:
        struct FreeList {
            FreeList *next;
        }
        
        FreeList *freeListHead;

        void getMoreNodes(size_t, size_t);
        void cleanUp();

}

// Returns a node from the free list
inline void *MemoryManager::allocate(size_t size) {
    if(freeListHead == 0) getMoreNodes(256, size); 
    
    FreeList *head = freeListHead;
    freeListHead = head->next;
    return head;
}

// Adds an unused node back to the free list
inline void MemoryManager:: free(void *deletedNode) {
    FreeList *head = static_cast<FreeList *>(deletedNode);
    head->next = freeListHead;
    freeListHead = head;
}

// Allocates a chunk of memory to be used as nodes as part of the free list
inline void MemoryManager::getMoreNodes(size_t poolSize, size_t objectSize) {
    size_t size = objectSize > sizeof(FreeList *) ? objectSize : sizeof(FreeList *);
    FreeList *head = reinterpret_cast<FreeList *>(new char[size]);
    freeListHead = head;

    for(int i = 0; i < poolSize; ++i) {
        head->next = reinterpret_cast<FreeList *>(new char[size]);
        head = head->next;
    }

    head->next = 0;
}

// De-allocates the free list
inline void MemoryManager::cleanUp() {
    FreeList *nextPtr = freeListHead;
    for(; nextPtr; nextPtr = freeListHead) {
        freeListHead = freeListHead->next;
        delete [] nextPtr;
    }
}

#endif
