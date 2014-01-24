#ifndef _AVLTREE_H_
#define _AVLTREE_H_

#include <cstddef>
#include <algorithm>
#include <iostream>

using std::max;
using std::pair;
using std::make_pair;
using std::ostream;

template <class _Value>
class AVLTree;
template <class _Value>
class AVLTreeIterator;

//! Structure of the node of AVL Tree
template <class _Value>
class AVLTreeNode {
public:
    //! Pointer to a node
    typedef AVLTreeNode* NodePtr;

    //! Pointer to sons and a parent in a tree structure
    ///*! Null parent indicates the root. Null sons indicate a leaf */
    NodePtr left, right, parent;

    //! Height of a subtree
    /*! Minimal height of a nonempty tree is 1 */
    int height;
    
    //! Value stored in the node
    _Value value;

    AVLTreeNode(const _Value& new_value, NodePtr new_parent);

    //! Returns a height of a tree or 0 if the tree is empty
    static int get_height(NodePtr node);

    //! Returns a difference of subtrees heights
    int get_balance();

    //! Updates height
    void merge_height();
    //! Updates node value based on values of sons
    void merge_children();

    //! Returns a rightmost vertex of a subtree
    NodePtr get_rightmost();
    //! Returns a leftmost vertex of a subtree
    NodePtr get_leftmost();
    //! Returns next vertex in a in-order 
    NodePtr get_next();
    //! Returns previous vertex in a in-order 
    NodePtr get_previous();
    //! Returns a root of a supertree
    NodePtr get_root();

    //! Performs left rotation
    /*! Returns a root of a new subtree */
    NodePtr rotate_left(NodePtr &root);
    //! Performs right rotation
    /*! Returns a root of a new subtree */
    NodePtr rotate_right(NodePtr &root);
    //! Restore an AVL property
    /*! Returns a root of a new subtree */
    NodePtr restore(NodePtr &root);

    //! Insert a value into a subtree
    /*! Returns a pointer to the new node */
    NodePtr insert(_Value &value, NodePtr &root);
    //! Finds a node with a given value
    NodePtr find(_Value &value);
    //! Removes vertex from the tree
    void erase(NodePtr &root);
    //! Merge two trees with a middle vertex
    /*! It requires left tree, a node and a right tree to be adeqately ordered */
    NodePtr merge(NodePtr left, NodePtr right);
    //! Split two trees with a middle vertex
    void split(NodePtr& left, NodePtr& right);    

    friend class AVLTree<_Value>;
    friend class AVLTreeIterator<_Value>;
};

template <class _Value>
AVLTreeNode<_Value>::AVLTreeNode(const _Value &new_value, NodePtr new_parent) :
    left(NULL), right(NULL), parent(new_parent), value(new_value) {
    merge_height();
    merge_children();
}

template <class _Value>
int
AVLTreeNode<_Value>::get_height(AVLTreeNode* node) {
    if (node == NULL) return 0;
    return node->height;
}

template <class _Value>
int
AVLTreeNode<_Value>::get_balance() {
    return get_height(right) - get_height(left);
}

template <class _Value>
void
AVLTreeNode<_Value>::merge_height() {
    height = max(get_height(left), get_height(right)) + 1;
}

template <class _Value>
AVLTreeNode<_Value>*
AVLTreeNode<_Value>::rotate_left(NodePtr &root) {
    NodePtr y = (NodePtr) right;

    if (this == root) {
        y->parent = NULL;
        root = y;
    } else {
        y->parent = parent;
        if (parent->left == this) {
            parent->left = y;
        } else {
            parent->right = y;
        }
    }
    
    right = y->left;
    if (right != NULL) {
        right->parent = this;
    }
    y->left = this;
    parent = y;

    merge_height();
    y->merge_height();

    merge_children();
    y->merge_children();

    return y;
}

template <class _Value>
AVLTreeNode<_Value>*
AVLTreeNode<_Value>::rotate_right(NodePtr &root) {
    NodePtr y = (NodePtr) left;
    
    if (this == root) {
        y->parent = NULL;
        root = y;
    } else {
        y->parent = parent;
        if (parent->left == this) {
            parent->left = y;
        } else {
            parent->right = y;
        }
    }

    left = y->right;
    if (left != NULL) {
        left->parent = this;
    }
    y->right = this;
    parent = y;

    merge_height();
    y->merge_height();

    merge_children();
    y->merge_children();
    
    return y;
}

template <class _Value>
AVLTreeNode<_Value>*
AVLTreeNode<_Value>::restore(NodePtr &root) {
    merge_height();
    if (get_balance() > 1) {
        NodePtr y = (NodePtr)right;
        if (y->get_balance() == -1) {
          y->rotate_right(root);  
        }
        return rotate_left(root);
    } else if (get_balance() < -1) {
        NodePtr y = (NodePtr)left;
        if (y->get_balance() == 1) {
            y->rotate_left(root);
        }
        return rotate_right(root);
    } else {
        merge_children();
        return this;
    }
}

template <class _Value>
void
AVLTreeNode<_Value>::merge_children() {
    if (left != NULL) {
        if (right != NULL) {
            value.merge_both(left->value, right->value);
        } else {
            value.merge_left(left->value);
        }
    } else {
        if (right != NULL) {
            value.merge_right(right->value);
        } else {
            value.merge_none();
        }
    }
}

template <class _Value>
AVLTreeNode<_Value>*
AVLTreeNode<_Value>::get_leftmost() {
    NodePtr x = this;
    while (x->left != NULL) {
        x = x->left;
    }
    return x;
}

template <class _Value>
AVLTreeNode<_Value>*
AVLTreeNode<_Value>::get_next() {
    if (right == NULL) {
        NodePtr x = this;
        while ((x->parent != NULL) && (x->parent->right == x)) {
            x = x->parent;
        }
        return x->parent;
    }
    return right->get_leftmost();
}

template <class _Value>
AVLTreeNode<_Value>*
AVLTreeNode<_Value>::get_previous() {
    if (left == NULL) {
        NodePtr x = this;
        while ((x->parent != NULL) && (x->parent->left == x)) {
            x = x->parent;
        }
        return x->parent;
    }
    return left->get_rightmost();
}

template <class _Value>
AVLTreeNode<_Value>*
AVLTreeNode<_Value>::get_root() {
    NodePtr x = this;
    while (x->parent != NULL) {
        x = x->parent;
    }
    return x;
}

template <class _Value>
AVLTreeNode<_Value>*
AVLTreeNode<_Value>::insert(_Value &new_value, NodePtr &root) {
    NodePtr result = NULL;

    if (value < new_value) {
        if (right != NULL) {
            result =  right->insert(new_value, root);
        } else {
            result = right = new AVLTreeNode<_Value>(new_value, this);
        }
    } else {
        if (left != NULL) {
            result = left->insert(new_value, root);
        } else {
            result = left = new AVLTreeNode<_Value>(new_value, this);
        }
    }
    restore(root);
    return result; 
}

template <class _Value>
AVLTreeNode<_Value>*
AVLTreeNode<_Value>::find(_Value &new_value) {
    if (new_value < value) {
        if (left == NULL) {
            return NULL;
        }
        return left->find(new_value);
    } else if (value < new_value) {
        if (right == NULL) {
            return NULL;
        }
        return right->find(new_value);
    } else {
        return this;
    }
}

template <class _Value>
void
AVLTreeNode<_Value>::erase(NodePtr &root) {
    NodePtr node = this, son = NULL;
    if (node->right == NULL) {
        son = node->left;
    } else {
        NodePtr new_node = node->get_next();
        node->value = new_node->value;
        node = new_node;
        son = node->right;
    }

    if (son != NULL) {
        son->parent = node->parent;
    }
    
    if (node->parent != NULL) {
        if (node->parent->left == node) {
            node->parent->left = son;
        } else {
            node->parent->right = son;
        }
    } else {
        root = son;
    }
    
    NodePtr path = node->parent;
    while (path != NULL) {
        path = path->restore(root)->parent;
    }

    delete node;
}

template <class _Value>
AVLTreeNode<_Value>*
AVLTreeNode<_Value>::merge(NodePtr lNode, NodePtr rNode) {
    int lH = get_height(lNode), rH = get_height(rNode);
    
    if (lNode != NULL) lNode->parent = NULL;
    if (rNode != NULL) rNode->parent = NULL;

    NodePtr x, y;
    if (lH < rH) {
        x = rNode;
        y = NULL;
        while (lH + 1 < rH) {
            y = x;
            x = x->left;
            rH = get_height(x);
        }

        right = x;
        if (x != NULL) {
            x->parent = this;
        }
        left = lNode;
        if (lNode != NULL) {
            lNode->parent = this;
        }
        parent = y;
        if (y != NULL) {
            y->left = this;
        }

        if (y == NULL) {
            y = this;
        } else {
            y = rNode;
        }
    } else {
        x = lNode;
        y = NULL;
        while (rH + 1 < lH) {
            y = x;
            x = x->right;
            lH = get_height(x);
        }

        left = x;
        if (x != NULL) {
            x->parent = this;
        }
        right = rNode;
        if (rNode != NULL) {
            rNode->parent = this;
        }
        parent = y;
        if (y != NULL) {
            y->right = this;
        }

        if (y == NULL) {
            y = this;
        } else {
            y = lNode;
        }
    }
    
    x = this;
    while (x != NULL) {
        x = x->restore(y)->parent;
    }
    return y;
}

template <class _Value>
void
AVLTreeNode<_Value>::split(NodePtr& lNode, NodePtr& rNode) {
    lNode = left;
    rNode = right;
    
    NodePtr node = this, p = node->parent;
    while (p != NULL) {
        if (p->left == node) {
            node = p;
            p = node->parent;
            rNode = node->merge(rNode, node->right);
        } else {
            node = p;
            p = node->parent;
            lNode = node->merge(node->left, lNode);
        }
    }
}

//! Iteratof of the AVLTree
template <typename _Value>   
class AVLTreeIterator {
private:
    //! Pointer to a node
    typedef AVLTreeNode<_Value>* NodePtr;
    //! Value of the iterator
    NodePtr ptr;
    AVLTreeIterator(const NodePtr &ptr) : ptr(ptr) {}
public:
    AVLTreeIterator(const AVLTreeIterator &it) : ptr(it.ptr) {};
    AVLTreeIterator() : ptr(NULL) {};
    AVLTreeIterator& operator++() {
        ptr = ptr->get_next();
        return *this;
    }
    bool operator==(const AVLTreeIterator& it) const {
        return ptr == it.ptr;
    }
    bool operator!=(const AVLTreeIterator& it) const {
        return ptr != it.ptr;
    }
    _Value& operator*() {
        return ptr->value;
    }

    /*! Returns an iterator of a root of a tree */
    AVLTreeIterator<_Value> get_root() {
        return AVLTreeIterator(ptr->get_root());
    }

    friend AVLTree<_Value>;
    friend ostream &operator<< (ostream &stream, const AVLTreeIterator &iter) {
        stream << iter.ptr->value;
        return stream;
    }
};

//! Balanced structure implementing an AVL tree
/*! It provides interfaces which allow to extend it into any structure
 *  requiring binary balanced tree as a base. */
template <typename _Value>
class AVLTree {
private:
    //! Pointer to a node
    typedef AVLTreeNode<_Value>* NodePtr;
    //! Root of the tree
    NodePtr root;

public:
    typedef AVLTreeIterator<_Value> iterator;
    
    AVLTree() : root(NULL) {}
    //! Inserts a value
    void insert(_Value value);
    //! Removes a node pointed by a iterator
    void erase(iterator it);
    //! Removes a node with a given value
    /*! Returns true if the vertex was erased */
    bool erase(_Value value);
    //! Merges two trees by inserting a new value between them
    /*! This tree, the new value and the second tree should be ordered adequately */
    void merge(_Value value, AVLTree& right);

    void split(iterator it, AVLTree& left, AVLTree& right);

    template <typename _VValue> 
    friend ostream &operator<< (ostream &stream, const AVLTree<_VValue> &tree);
    
    //! Find a vertex with equal value
    /*! Returns end iterator if such a vertex doesn't exist */
    iterator find(_Value value) const;
    //! Returns the iterator leftmost vertex
    iterator begin() const;
    //! Returns empty iterator
    iterator end() const;
    /*! Returns an iterator of a root of a tree */
    iterator get_root() const;
};

template <typename _Value>
void
AVLTree<_Value>::insert(_Value value) {
    if (root == NULL) {
        root = new AVLTreeNode<_Value>(value, NULL);
    } else {
        root->insert(value, root);
    }
}

template <typename _Value>
void
AVLTree<_Value>::erase(iterator it) {
    it.ptr->erase(root);
    return;
}

template <typename _Value>
bool
AVLTree<_Value>::erase(_Value value) {
    NodePtr x = root->find(value);
    if (x == NULL) {
        return false;
    }
    erase(iterator(x));
    return true;
}

template <typename _Value>
void
AVLTree<_Value>::merge(_Value value, AVLTree<_Value>& right) {
    root = (new AVLTreeNode<_Value>(value, NULL))->merge(root, right.root);
    right.root = NULL;
}

template <typename _Value>
void
AVLTree<_Value>::split(iterator it, AVLTree<_Value>& left, AVLTree<_Value>& right) {
    it.ptr->split(left.root, right.root);
    root = NULL;
}

template <typename _Value>
ostream &operator<<(ostream &stream, const AVLTree<_Value> &tree) {
    stream << "AVLTree";
    stream << " [";
    typename AVLTree<_Value>::iterator it = tree.begin();
    while (it != tree.end()) {
        stream << it;
        ++it;
        if (it != tree.end()) stream << ", ";
    }
    stream << "]";
    return stream;
}

template <typename _Value>
AVLTreeIterator<_Value>
AVLTree<_Value>::find(_Value value) const {
    if (root == NULL) {
        return AVLTreeIterator<_Value>(NULL);
    }
    return AVLTreeIterator<_Value>(root->find(value));
}

template <typename _Value>
AVLTreeIterator<_Value>
AVLTree<_Value>::begin() const {
    if (root == NULL) return end();
    return AVLTreeIterator<_Value>(root->get_leftmost());
}

template <typename _Value>
AVLTreeIterator<_Value>
AVLTree<_Value>::end() const {
    return AVLTreeIterator<_Value>();
}

template <typename _Value>
AVLTreeIterator<_Value>
AVLTree<_Value>::get_root() const {
    return AVLTreeIterator<_Value>(root);
}

#endif
