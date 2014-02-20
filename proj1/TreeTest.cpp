#include "Tree.hpp"
#include <stdio.h>
int main(int argc, char **argv) {
    Tree t;
    char nextop;   
    while((nextop = getchar()) != 'q') {
        if(nextop == 'i') {
            int insKey, insObj;
            scanf(" %d", &insKey);
            insObj = 10*insKey+2;
            //t.insert(insKey, insObj);
            //std::cout << "insKey: " << insKey << "\tinsObj: " << insObj << std::endl;
        }
        if(nextop == 'f') {
            int findKey;
            TreeNode *findObj;
            scanf(" %d", &findKey);
            //findObj = find(findKey);
            //std::cout << "findKey: " << findKey << "\tfindObj: " << findObj->key << std::endl;
        }
        if(nextop == 'd') {
            int delKey;
            TreeNode *delObj;
            scanf(" %d", &delKey);
            //delObj = deleteNode(delKey);
            //std::cout << "delKey: " << delKey << "\tdelObj: " << delObj->key << std::endl;
        }
    }
}
