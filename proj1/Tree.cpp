#include "Tree.hpp"
#include <queue>
#include <utility>

MemoryManager mm;

void *TreeNode::operator new(size_t size) {
    std::cout << "In TreeNode::new - @param size: " << size << std::endl;
    mm.allocate(size);
}

void TreeNode::operator delete(void *ptr) {
    std::cout << "In TreeNode::delete" << std::endl;
    mm.free(ptr);
}

Tree::Tree() {
    std::cout << "In Tree() " << std::endl;
    root = new TreeNode();
    std::cout << "In Tree() created new root " << std::endl;
    root->left = NULL;
    std::cout << "In Tree::constructor - Created tree" << std::endl;
}

TreeNode *Tree::find(key_t query) {
    if(root->left == NULL) return NULL;
    TreeNode *tmp = root;
    while(tmp->right != NULL) {
        if(query < tmp->key) {
            tmp = tmp->left;
        } else {
            tmp = tmp->right;
        }
    }

    if(tmp->key == query) return tmp->left;
    else return NULL;

}

void Tree::insert(key_t newKey, object_t newObj) {
    if(root->left == NULL) {
        TreeNode *newNode = new TreeNode(newObj);
        root->left = newNode;
        root->key = newKey;
        root->right = NULL;
    } else {
        TreeNode *tmp = root;
        while(tmp->right != NULL) {
            if(newKey < tmp->key) tmp = tmp->left;
            else tmp = tmp->right;
        }

        if(tmp->key == newKey) return;

        TreeNode *oldLeaf, *newLeaf;
        oldLeaf = new TreeNode();
        oldLeaf->left = tmp->left;
        oldLeaf->key = tmp->key;
        oldLeaf->right = NULL;

        newLeaf = new TreeNode();
        newLeaf->left = new TreeNode(newObj);
        newLeaf->key = newKey;
        newLeaf->right = NULL;

        if(tmp->key < newKey) {
            tmp->left = oldLeaf;
            tmp->right = newLeaf;
            tmp->key = newKey;
        } else {
            tmp->left = newLeaf;
            tmp->right = oldLeaf;
        }
    }

}

TreeNode *Tree::deleteNode(key_t deleteKey) {
    TreeNode *delObj, *upper, *other;
    if(root->left == NULL) return NULL;
    if(root->right == NULL) { // object on left
        if(root->left->key == deleteKey) {
            delObj = root->left;
            root->left = NULL;
            return delObj;
        } else return NULL;
    }
    
    TreeNode *tmp = root;
    while(tmp->right != NULL) {
       upper = tmp;
       if(deleteKey < tmp->key) {
            tmp = upper->left;
            other = upper->right;
       } else {
            tmp = upper->right;
            other = upper->left;
       }
    }
    if(tmp->key != deleteKey) return NULL;
    else {
        upper->key = other->key;
        upper->left = other->left;
        upper->right = other->right;
        delObj = tmp->left;
        delete tmp;
        delete other;
        return delObj;
    }
}


/* Breadth first traversal printing to show each level */
void Tree::print() {
    if(root == NULL) return;
    const TreeNode *tmp;
    int level = 0;
    typedef std::pair<const TreeNode *, int> nodeLevel;
    std::queue<nodeLevel> q;
    q.push(nodeLevel(root, 1));

    while(!q.empty()) {
        nodeLevel nl = q.front();
        q.pop();
        if((tmp = nl.first) != NULL) {
            if(level != nl.second) {
                std::cout << " Level " << nl.second << ": ";
                level = nl.second;
            }
            std::cout << tmp->key << ' ';
            q.push(nodeLevel(tmp->left, level+1));
            q.push(nodeLevel(tmp->right, level+1));
        } 
    }
    std::cout << std::endl;
}
