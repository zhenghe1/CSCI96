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
        MemoryManager() : treeNodesHead(0) {
            getMoreNodes();
        }
        virtual ~MemoryManager() {
            cleanUp();
        }
        virtual void *allocate(size_t);
        virtual void free(void *);
    private:
        struct TreeNodes {
            TreeNodes *next;
        }
        
        TreeNodes *treeNodesHead;

        void getMoreNodes();
        void cleanUp();

}

#endif
