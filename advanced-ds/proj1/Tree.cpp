#include "Tree.hpp"

MemoryManager mm;

/* Overloaded operator new to use MemoryManager's free list for memory allocation. */
void *TreeNode::operator new(size_t size) {
    mm.allocate(size);
}

/* Overloaded operator delete to use MemoryManager's free list for de-allocation. */
void TreeNode::operator delete(void *ptr) {
    mm.free(ptr);
}

/* 
 * Default constructor
 * Change alpha value as needed.
 */
Tree::Tree() : alpha(0.245) {
    leafCounter = new TreeNode(0);
    root = new TreeNode();
    leafCounter->left = root;
    root->left = NULL;
    root->parent = NULL;
    root->right = NULL;
    std::cout << "\nDefault params: alpha = " << alpha << std::endl << std::endl;
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

    if(tmp->key == query) return tmp;
    else return NULL;

}

int Tree::insert(key_t newKey, object_t newObj) {
    std::cout << "\n----Inserting object----" << std::endl;
    int level = 0; // Used to compare the depth with the max height for balancing check
    
    /* No objects yet in the tree. */
    if(root->left == NULL) {
        TreeNode *newNode = new TreeNode(newObj);
        root->left = newNode;
        root->key = newKey;
        root->right = NULL;
        leafCounter->key++;
        std::cout << "Inserted object: " << newObj << "  at key: " << newKey << std::endl;
        std::cout << "Level of tree: " << level << "  Depth bound: " << depthBound() << std::endl;

    } else {
        TreeNode *tmp = root;

        // Find place to insert
        while(tmp->right != NULL) {
            if(newKey < tmp->key) tmp = tmp->left;
            else tmp = tmp->right;
            level++;
        }

        if(tmp->key == newKey) return 0;

        // Create new comparison and object nodes
        TreeNode *oldLeaf, *newLeaf;
        oldLeaf = new TreeNode();
        oldLeaf->left = tmp->left;
        oldLeaf->key = tmp->key;
        oldLeaf->right = NULL;

        newLeaf = new TreeNode();
        newLeaf->left = new TreeNode(newObj);
        newLeaf->key = newKey;
        newLeaf->right = NULL;

        newLeaf->parent = tmp;
        oldLeaf->parent = tmp;
        leafCounter->key++;
        level++;

        std::cout << "Inserted object: " << newObj << "  at key: " << newKey << std::endl;
        std::cout << "Level of tree: " << level << "  Depth bound: " << depthBound() << std::endl;
        
        if(tmp->key < newKey) {
            tmp->left = oldLeaf;
            tmp->right = newLeaf;
            tmp->key = newKey;
            if(level > depthBound()) insertSubtree(oldLeaf);
        } else {
            tmp->left = newLeaf;
            tmp->right = oldLeaf;
            if(level > depthBound()) insertSubtree(newLeaf);
        }

    }
    
    return 1;
}

/* Attaches balanced subtree to the actual tree */
TreeNode *Tree::insertSubtree(TreeNode *tn) {
    std::cout << "\n----Tree before rebuilding----" << std::endl;
    print(); 
    TreeNode *tmp;
    tmp = tn->parent;
    int size = 0, level = 0;
    TreeNode *list = NULL;
    TreeNode *back = NULL;
    
    // Goes up the path from the level that breaks threshold and combines left and right subtrees into a list.
    std::cout << "\n---- Going up the path ----" << std::endl;
    if(tn->parent->key > tn->key) convertToList(tmp, size, list, back, 0);
    else convertToList(tmp, size, list, back, 1);
    level++;
    std::cout << "Number of leaves: " << size << "  Weight bound: " << weightBound(level) << std::endl;
    while(size > weightBound(level)) {
        tmp = tmp->parent;
        if(list->key >= tmp->key) {
            convertToList(tmp->left, size, list, back, 0);
        } else {
            convertToList(tmp->right, size, list, back, 1);
        }
        level++;
        std::cout << "Number of leaves: " << size << "  Weight bound: " << weightBound(level) << std::endl;
    }

    // Inserts subtree into tree
    if(tmp == root)  {
        leafCounter->left = rebalance(list);
        root = leafCounter->left;
        std::cout << "\n----Rebalanced Tree----" << std::endl;
        print();
    }
    else if(list->key < tmp->parent->key) {
        tmp->parent->left = rebalance(list);
        std::cout << "\n----Rebalanced Tree----" << std::endl;
        print();
    }
    else {
        tmp->parent->right = rebalance(list);
        std::cout << "\n----Rebalanced Tree----" << std::endl;
        print();
    }
}

/* Does the rebalancing by using the top-down algorithm. */
TreeNode *Tree::rebalance(TreeNode *list) {
    std::cout << "\n----Starting to rebuild subtree to rebalance----." << std::endl;
    st_item current, left, right;
    std::stack<st_item> s;
    TreeNode *tmp, *root;
    int length = 0;

    for(tmp = list; tmp != NULL; tmp = tmp->right) length++;
    
    root = new TreeNode();
    current.n1 = root;
    current.n2 = NULL;
    current.num = length;
    s.push(current);

    while(!s.empty()) {
        current = s.top();
        s.pop();

        if(current.num > 1) {
            left.n1 = new TreeNode();
            left.n2 = current.n2;
            left.num = current.num/2;

            right.n1 = new TreeNode();
            right.n2 = current.n1;
            right.num = current.num - left.num;

            current.n1->left = left.n1;
            left.n1->parent = current.n1;

            current.n1->right = right.n1;
            right.n1->parent = current.n1;

            s.push(right);
            s.push(left);
        } else {
            current.n1->left = list->left;
            current.n1->key = list->key;
            current.n1->right = NULL;

            if(current.n2 != NULL) current.n2->key = list->key;
            tmp = list;
            list = list->right;
            delete tmp;
        }
    }
    return root;
}

/* Converts subtree into list. 
 * params: size keeps track of number of leaves.
 *         list is the head pointer.
 *         back is back pointer.
 *         direction is the flag for left(0) or right(1) subtree.
 */
TreeNode* Tree::convertToList(TreeNode *tn, int &size, TreeNode *&list, TreeNode *&back, bool direction) {
    TreeNode *node, *tmp = list;
    if(tn->left == NULL) {
        delete tn;
        return NULL;
    }
    std::stack<TreeNode *> s;
    s.push(tn);
    while(!s.empty()) {
        node = s.top();
        s.pop();
        if(node->right == NULL) {
            if(direction == 1) {
                if(back == NULL) {
                    back = node;
                    list = node;
                }
                else {
                    back->right = node;
                    back = back->right;
                }
            }
            else {
                node->right = list;
                list = node;
                if(back == NULL) back = node->right;
            }
            size++;
        } else {
            s.push(node->left);
            s.push(node->right);
            delete node;
        }
    }
     
    return list;
}

TreeNode *Tree::deleteNode(key_t deleteKey) {
    TreeNode *delObj, *upper, *other;
    if(root->left == NULL) return NULL;
    if(root->right == NULL) { // object on left
        if(root->key == deleteKey) {
            delObj = root->left;
            root->left = NULL;
            leafCounter->key--;
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
        // Have to change parent pointers
        if(upper->right != NULL) upper->left->parent = other->parent;
        upper->right = other->right;
        if(upper->right != NULL) upper->right->parent = other->parent;
        delObj = tmp->left;
        delete tmp;
        delete other;
        leafCounter->key--;
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
