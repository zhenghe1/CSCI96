#ifndef TREE_HPP
#define TREE_HPP

#include "MemoryManager.hpp"
#include <stdlib.h>
#include <iostream>

typedef int key_t;
typedef int object_t;

class TreeNode {
    public:
        key_t key;
        TreeNode *left;
        TreeNode *right;
        TreeNode *parent;
        
        TreeNode(){}
        TreeNode(int value) : key(value) {}

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

        void print();
};

#endif
