#ifndef TREE_HPP
#define TREE_HPP

#include "MemoryManager.hpp"

#include <stdlib.h>
#include <math.h>
#include <iostream>
#include <queue>
#include <utility>
#include <stack>

typedef int key_t;
typedef int object_t;

class TreeNode {
    public:
        key_t key;
        TreeNode *left;
        TreeNode *right;
        TreeNode *parent; // Back pointers to parent node
        
        TreeNode() : left(NULL), right(NULL), parent(NULL) {}
        TreeNode(int value) : key(value), left(NULL), right(NULL), parent(NULL) {}

        inline void *operator new(size_t);
        inline void operator delete(void *);
};

class Tree {
    private:
        TreeNode *root;
        double alpha;
        // Inner class for building top-down optimal tree
        class st_item {
            public:
                TreeNode *n1;
                TreeNode *n2;
                int num;
        };
    public:
        TreeNode *leafCounter; // Node above root to keep track of number of leaves
        Tree();

        TreeNode *find(key_t);

        int insert(key_t, object_t);

        TreeNode *deleteNode(key_t);

        inline const double depthBound() {
            return ((pow(log2(1/(1-alpha)), -1)) * log2(leafCounter->key));
        }

        inline double weightBound(int i) {
            return ((pow(1/(1-alpha), i)));
        }

        TreeNode *insertSubtree(TreeNode *);

        TreeNode *rebalance(TreeNode *);

        TreeNode *convertToList(TreeNode *, int &, TreeNode *&, TreeNode *&, bool);

        void print();
};

#endif
