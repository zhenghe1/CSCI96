#include "Tree.hpp"

MemoryManager mm;

void *TreeNode::operator new(size_t size) {
    //std::cout << "In TreeNode::new - @param size: " << size << std::endl;
    mm.allocate(size);
}

void TreeNode::operator delete(void *ptr) {
    //std::cout << "In TreeNode::delete" << std::endl;
    mm.free(ptr);
}

Tree::Tree() : alpha(0.245) {
    //std::cout << "In Tree() " << std::endl;
    leafCounter = new TreeNode(0);
    root = new TreeNode();
    leafCounter->left = root;
    //std::cout << "In Tree() created new root " << std::endl;
    root->left = NULL;
    root->parent = NULL;
    root->right = NULL;
    //std::cout << "In Tree::constructor - Created tree" << std::endl;
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
    int level = 0;
    if(root->left == NULL) {
        TreeNode *newNode = new TreeNode(newObj);
        root->left = newNode;
        root->key = newKey;
        root->right = NULL;
        leafCounter->key++;
    } else {
        TreeNode *tmp = root;
        while(tmp->right != NULL) {
            if(newKey < tmp->key) tmp = tmp->left;
            else tmp = tmp->right;
            level++;
        }

        if(tmp->key == newKey) return 0;

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
        //std::cout << "Level of node: " << level << std::endl;
        const double bound = depthBound();
        //std::cout << "Bound: " << bound << " depth: " << level << std::endl;
        if(tmp->key < newKey) {
            tmp->left = oldLeaf;
            tmp->right = newLeaf;
            tmp->key = newKey;
        } else {
            tmp->left = newLeaf;
            tmp->right = oldLeaf;
        }
        if(level > depthBound()) insertSubtree(newLeaf);
    }
    
    return 1;
}


TreeNode *Tree::insertSubtree(TreeNode *tn) {
    print();
    /*std::cout << "alpha weight: " << weightBound(1) << "  i: " << 1 << std::endl;
    std::cout << "alpha weight: " << weightBound(2) << "  i: " << 2 << std::endl;
    std::cout << "alpha weight: " << weightBound(3) << "  i: " << 3 << std::endl;
    std::cout << "alpha weight: " << weightBound(4) << "  i: " << 4 << std::endl;
    std::cout << "alpha weight: " << weightBound(5) << "  i: " << 5 << std::endl;
    std::cout << "alpha weight: " << weightBound(6) << "  i: " << 6 << std::endl;
    std::cout << "alpha weight: " << weightBound(7) << "  i: " << 7 << std::endl;
    std::cout << "alpha weight: " << weightBound(8) << "  i: " << 8 << std::endl;
    std::cout << "alpha weight: " << weightBound(9) << "  i: " << 9 << std::endl;
    std::cout << "alpha weight: " << weightBound(10) << "  i: " << 10 << std::endl;
    */
    TreeNode *tmp;
    tmp = tn->parent;
    int size = 0, level = 0;
    TreeNode *list = NULL;
    TreeNode *back = NULL;
    if(tn->parent->key > tn->key) convertToList(tmp, size, list, back, 0);
    else convertToList(tmp, size, list, back, 1);
    level++;
    //std::cout << "rebalance size: " << size << "  bound: " << weightBound(level) << std::endl;
    while(size > weightBound(level)) {
        tmp = tmp->parent;
        //std::cout << "listkey " << list->key << "  tmpKey: " << tmp->key << std::endl;
        if(list->key >= tmp->key) {
            convertToList(tmp->left, size, list, back, 0);
        } else {
            convertToList(tmp->right, size, list, back, 1);
        }
        level++;
        //std::cout << "rebalance size: " << size << "  bound: " << weightBound(level) << std::endl;
    }

    while(list != NULL) {
        std::cout << list->key << std::endl;
        list = list->right;
    }
    std::cout << "PARENT: " << tmp->parent->key << std::endl;
    if(list->key < tmp->parent->key) tmp->parent->left = rebalance(list);
    else tmp->parent->right = rebalance(list);
}

TreeNode *Tree::rebalance(TreeNode *list) {
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


    std::cout << "Before while loop" << std::endl;

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
                back = node->right;
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
        upper->right = other->right;
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
