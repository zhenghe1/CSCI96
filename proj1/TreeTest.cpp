#include "Tree.hpp"
#include <stdio.h>
#include <sstream>

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
            if(ret == 1) {
                std::cout << "\ninsKey: " << insKey << "\tinsObj: " << insObj << "\tleafCount: " << t.leafCounter->key << std::endl;
            } else {
                std::cout << "Duplicate insert key" << std::endl;
            }
        }
        if(nextop == "f") {
            int findKey;
            TreeNode *findObj;
            std::cout << "\nEnter key to find: ";
            getline(std::cin, value);
            std::stringstream(value) >> findKey;
            findObj = t.find(findKey);
            if(findObj != NULL) {
                std::cout << findObj->key << " ";
                TreeNode *tn = findObj;
                while(tn->parent != NULL) {
                    std::cout << tn->parent->key << " ";
                    tn = tn->parent;
                }
                std::cout << "\nfindKey: " << findKey << "\tfindObj: " << findObj->left->key << std::endl;
        
            } else {
                std::cout << "key not found" << std::endl;
            }   
        }
        if(nextop == "d") {
            int delKey;
            TreeNode *delObj;
            std::cout << "\nEnter key to delete: ";
            getline(std::cin, value);
            std::stringstream(value) >> delKey;
            delObj = t.deleteNode(delKey);
            if(delObj != NULL) {
                std::cout << "\ndelKey: " << delKey << "\tdelObj: " << delObj->key << "\tleafCount: " << t.leafCounter->key << std::endl;
            } else {
                std::cout << "delete key not found" << std::endl;
            }
        }
        if(nextop == "p") t.print();
        std::cout << "\nEnter command: ";
        getline(std::cin, nextop);
    }
}
