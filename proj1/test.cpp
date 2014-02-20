#include "MemoryManager.hpp"
#include <iostream>

MemoryManager mm;

class Tree {
    public:
        int value;
        Tree *left, *right;
        Tree() : value(5) {}
        Tree(int x) : value(x) {
            std::cout << "Value of x: " << x << std::endl;
        }
        inline void *operator new(size_t size) {
            std::cout << "Size: " << size << std::endl;
            return mm.allocate(size);
        }
        inline void operator delete(void *ptr) {
            std::cout << "In delete: " << ((Tree *)ptr)->value << std::endl;
            mm.free((void *) ptr);
        }
};

int main() {
    size_t treeSize = sizeof(Tree);
    Tree *t1 = new Tree(50);
    delete t1;
}
