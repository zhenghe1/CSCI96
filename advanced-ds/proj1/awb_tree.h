#ifndef AWB_TREE_H
#define AWB_TREE_H

#include <math.h>
#include <stack>
#include "memory_manager.hpp"

typedef int key_t;
typedef int object_t;
MemoryManager mm;

class TreeNode {
    public:
        key_t key;
        TreeNode *left;
        TreeNode *right;
        TreeNode *parent;

        explicit TreeNode() : key(0), left(NULL), right(NULL), parent(NULL) {}
        explicit TreeNode(key_t k) : key(k), left(NULL), right(NULL), parent(NULL) {}
        
        inline void *operator new(size_t size) {
            return mm.allocate(size);            
        }
        
        inline void operator delete(void *ptrDelete) {
            mm.free(ptrDelete);
        }
};

class AWBTree {
    private:
        const double ALPHA = 0.245;
        TreeNode *m_leafCounter;

        typedef struct {
            TreeNode *tn1;
            TreeNode *tn2;
            int num;
        } st_item;

    public:
        explicit AWBTree();

        object_t *find(const key_t);

        int insert(const key_t, const *object_t);

        void insertSubTree(TreeNode const * const);

        TreeNode *rebalance(TreeNode *);

        object_t *deleteObj(const key_t);

        inline double depthBound() {
            return ((pow(log2(1/(1-ALPHA)), -1)) * log2(leafCounter));
        }

        inline double weightBound(const int i) {
            return ((pow(1/(1-ALPHA), i)));
        }
};

#endif
