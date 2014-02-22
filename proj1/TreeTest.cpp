#include "Tree.hpp"
#include <stdio.h>
#include <sstream>
#include <math.h>

int main(int argc, char **argv) {
    double x = log2(1/(1-0.167));
    double y = pow(x, -1);
    std::cout << x << " " << y << " " << y * 2 << std::endl;
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
        /*if(nextop == "l") {
            int size = 0;
            TreeNode *tn = t.convertToList(t.leafCounter->left->right->right, size, NULL);
            while(tn != NULL) {
                std::cout << tn->key << " ";
                tn = tn->right;
            }
            std::cout << "size: " << size << std::endl;
            
            t.print();
            size = 0;
            TreeNode *tn2 = t.convertToList(t.leafCounter->left->right, size, tn);
            while(tn2 != NULL) {
                std::cout << tn2->key << " ";
                tn2 = tn2->right;
            }
            std::cout << "size: " << size << std::endl;
        
        }*/
        std::cout << "\nEnter command: ";
        getline(std::cin, nextop);
    }
}
