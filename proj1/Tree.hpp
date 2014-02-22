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
        TreeNode *parent;
        
        TreeNode() : left(NULL), right(NULL), parent(NULL) {}
        TreeNode(int value) : key(value), left(NULL), right(NULL), parent(NULL) {}

        inline void *operator new(size_t);
        inline void operator delete(void *);
};

class Tree {
    private:
        TreeNode *root;
    public:
        TreeNode *leafCounter;
        Tree();

        TreeNode *find(key_t);

        int insert(key_t, object_t);

        TreeNode *deleteNode(key_t);

        inline const double depthBound() {
            return ((pow(log2(1/(1-0.245)), -1)) * log2(leafCounter->key));
        }

        TreeNode *rebalance(TreeNode *);

        TreeNode *convertToList(TreeNode *);

        void print();
};

#endif
