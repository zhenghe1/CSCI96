#include "Tree.hpp"
#include <stdio.h>
#include <sstream>
#include <math.h>

int main(int argc, char **argv) {
    Tree t;
    std::string nextop, value;
    
    std::cout << "\n'i' to insert\n'f' to find\n'd' to delete\n'p' to print\n" << std::endl;
    std::cout << "\nEnter command: ";
    getline(std::cin, nextop);
    
    while(nextop != "q") {
        if(nextop == "i") {
            int insKey, insObj;
            std::cout << "\nEnter key to insert: ";
            getline(std::cin, value);
            std::stringstream(value) >> insKey;
            insObj = 10*insKey+2;
            int ret = t.insert(insKey, insObj);
            if(ret != 1) std::cout << "\nInsert key: " << insKey << " is a duplicate." << std::endl;
            
        }
        if(nextop == "f") {
            int findKey;
            TreeNode *findObj;
            std::cout << "\nEnter key to find: ";
            getline(std::cin, value);
            std::stringstream(value) >> findKey;
            findObj = t.find(findKey);
            if(findObj != NULL) std::cout << "\nObject: " << findObj->left->key << " at key: " << findKey << std::endl;
            else std::cout << "\nKey :" << findKey << " does not exist." << std::endl;
            
        }
        if(nextop == "d") {
            int delKey;
            TreeNode *delObj;
            std::cout << "\nEnter key to delete: ";
            getline(std::cin, value);
            std::stringstream(value) >> delKey;
            delObj = t.deleteNode(delKey);
            if(delObj != NULL) std::cout << "\nDeleted object: " << delObj->key << " at key: " << delKey << "\tLeaf count now at: " << t.leafCounter->key << std::endl;
            else std::cout << "\nDelete key: " << delKey << " does not exist" << std::endl;
            
        }
        
        if(nextop == "p") t.print();
    
        std::cout << "\nEnter command: ";
        getline(std::cin, nextop);
    }
}
