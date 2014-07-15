#include "awb_tree.h"

AWBTree::AWBTree() : leafCounter(new TreeNode()) {
    TreeNode *tmp;
    tmp = new TreeNode();
    leafCounter->left = tmp;
}

object_t *AWBTree::find(const key_t queryKey) {
    // tree begins below leafCounter
    TreeNode *curr = leafCounter->left;

    if(curr->left == NULL) {
        return NULL;
    }

    while(curr->right != NULL) {
        if(queryKey < curr->key) {
            curr = curr->left;
        }
        else {
            curr = curr->right;
        }
    }

    if(curr->key == queryKey) {
        return static_cast<object_t *>(curr->left->key);
    }
    else {
        return NULL;
    }
}

int AWBTree::insert(const key_t newKey, const object_t *newObj) {
    // used to calculate the depth
    int levels = 0; 
    TreeNode *curr = leafCounter->left;

    if(curr->left = NULL) {
        curr->left = new TreeNode(*newObj);
        curr->key = newKey;
        ++leafCounter;
    } else {
        while(curr->right != NULL) {
            if(newKey < curr->key) curr = curr->left;
            else curr = curr->right;
            ++levels;
        }

        // no duplicates allowed
        if(curr->key = newKey) return 0;
    
        TreeNode *oldLeaf, *newLeaf;
        oldLeaf = new TreeNode(curr->key);
        oldLeaf->left = curr->left;
        
        newLeaf = new TreeNode(newKey);
        newLeaf->left = new TreeNode(*newObj);
        
        newLeaf->parent = curr;
        oldLeaf->parent = curr;
        ++levels;
        ++leafCounter;

        if(curr->key < newKey) {
            curr->left = oldLeaf;
            curr->right = newLeaf;
            curr->key = newKey;
            if(levels > depthBound()) {
                insertSubTree(oldLeaf);
            }
        } else {
            curr->left = newLeaf;
            curr->right = oldLeaf;
            if(levels > depthBound()) {
                insertSubTree(newLeaf);
            }
        }
    }
    return 1;
}

void AWBTree::insertSubTree(TreeNode const * const tn) {
    TreeNode *curr = tn->parent;
    int subTreeSize = 0;
    int levels = 0;
    TreeNode *front = NULL;
    TreeNode *back = NULL;

    if(tn->parent->key > tn->key) {
        convertToList(curr, subTreeSize, front, back, 0);
    } else {
        convertToList(curr, subTreeSize, front, back, 1);
    }
    ++levels;

    while(subTreeSize > weightBound(levels)) {
        curr = curr->parent;
        if(front->key >= curr->key) {
            convertToList(curr->left, subTreeSize, front, back, 0);
        } else {
            convertToList(curr->right, subTreeSize, front, back, 1);
        }
        ++levels;
    }

    if(curr == leafCounter->left) {
        leafCounter->left = rebalance(front);
    } else if(front->key < curr->parent->key) {
        curr->parent->left = rebalance(front);
        curr->parent->left->parent = curr->parent;
    } else {
        curr->parent->right = rebalance(front);
        curr->parent->right->parent = curr->parent;
    }
}

void AWBTree::convertToList(TreeNode *tn, int &size, TreeNode *&front, TreeNode *&back, bool direction) {
    TreeNode *curr;

    if(tn->left == NULL) {
        delete tn;
        return;
    }

    std::stack<TreeNode *> subTrSt;
    subTrSt.push(tn);

    while(!subTrSt.empty()) {
        curr = subTrSt.top();
        subTrSt.pop();

        if(curr->right == NULL) {
            if(direction == 1) {
                if(back == NULL) {
                    back = curr;
                    front = curr;
                } else {
                    back->right = curr;
                    back = back->right;
                }
            } else {
                curr->right = front;
                front = curr;
                if(back == NULL) {
                    back = curr->right;
                }
            }
            ++size;
        } else {
            subTrSt.push(curr->left);
            subTrSt.push(curr->right);
            delete curr;
        }
    }
}

TreeNode* AWBTree::rebalance(TreeNode *subTreeList) {
    st_item current, left, right;
    std::stack<st_item> subTrSt;
    TreeNode *tmp, *root;
    int length = 0;

    for(tmp = list; tmp != NULL; tmp = tmp->right) {
        ++length;
    }

    root = new TreeNode();
    current.tn1 = root;
    current.tn2 = NULL;
    current.num = length;
    subTrSt.push(current);

    while(!subTrSt.empty()) {
        current = subTrSt.top();
        subTrSt.pop();

        if(current.num > 1) {
            left.tn1 = new TreeNode();
            left.tn2 = current.tn2;
            left.num = current.num/2;

            right.tn1 = new TreeNode();
            right.tn2 = current.tn1;
            right.num = current.num - left.num;

            current.tn1->left = left.tn1;
            left.tn1->parent = current.tn1;

            current.tn1->right = right.tn1;
            right.tn1->parent = current.tn1;

            subTrSt.push(right);
            subTrSt.push(left);
        } else {
            current.tn1->left = list->left;
            current.tn1->key = list->key;
            current.tn1->right = NULL;

            if(current.tn2 != NULL) {
                current.tn2->key = list->key;
            }

            tmp = list;
            list = list->right;
            delete tmp;
        }
    }
    return root;
}

object_t *AWBTree::deleteObj(const key_t deleteKey) {
    TreeNode *curr = leafCounter->left;
    TreeNode *tmp, *upper, *other;
    object_t *deletedObj;

    if(curr->left == NULL) {
        return NULL;
    }
    if(curr->right == NULL) {
        if(curr->key == deleteKey) {
            deletedObj = static_cast<object_t *>(curr->left->key);
            curr->left = NULL;
            return deletedObj;
        } else {
            return NULL;
        }
    }

    tmp = curr;
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

    if(tmp->key != deleteKey) {
        return NULL;
    } else {
        upper->key = other->key;
        upper->left = other->left;
        if(upper->right != NULL) {
            upper->left->parent = other->parent;
        }
        upper->right = other->right;
        if(upper->right != NULL) {
            upper->right->parent = other->parent;
        }
        deletedObj = static_cast<object_t *>(tmp->left->key);
        delete tmp;
        delete other;
        --leafCounter;
        return deletedObj;
    }
}
