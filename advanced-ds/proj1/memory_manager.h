#ifndef MEMORY_MANAGER_H
#define MEMORY_MANAGER_H

#include <cstdef>

class IMemoryManager {
    public:
        virtual void *allocate(const size_t) = 0;
        virtual void free(void *) = 0;
};

class MemoryManager : public IMemoryManager {
    private:
        struct FreeList {
            FreeList *next;
        };

        void expandPoolSize(const size_t, int poolSize = 32);
        void cleanUp();

        FreeList *freeListHead;

    public:
        MemoryManager() {
            freeListHead = 0;
            expandPoolSize();
        }
        virtual ~MemoryManager() {
            cleanUp();
        }

        virtual void *allocate(const size_t);
        virtual void free(void *);
};

#endif
