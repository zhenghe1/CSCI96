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
            t.insert(insKey, insObj);
            std::cout << "\ninsKey: " << insKey << "\tinsObj: " << insObj << std::endl;
        }
        if(nextop == "f") {
            int findKey;
            TreeNode *findObj;
            std::cout << "\nEnter key to find: ";
            getline(std::cin, value);
            std::stringstream(value) >> findKey;
            findObj = t.find(findKey);
            std::cout << "\nfindKey: " << findKey << "\tfindObj: " << findObj->key << std::endl;
        }
        if(nextop == "d") {
            int delKey;
            TreeNode *delObj;
            std::cout << "\nEnter key to delete: ";
            getline(std::cin, value);
            std::stringstream(value) >> delKey;
            delObj = t.deleteNode(delKey);
            std::cout << "\ndelKey: " << delKey << "\tdelObj: " << delObj->key << std::endl;
        }
        if(nextop == "p") t.print();
        std::cout << "\nEnter command: ";
        getline(std::cin, nextop);
    }
}
