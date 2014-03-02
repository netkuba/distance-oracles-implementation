#ifndef _AVLTREE_H_
#define _AVLTREE_H_

#include <cstddef>
#include <algorithm>
#include <iostream>
#include <cassert>

using std::max;
using std::pair;
using std::make_pair;
using std::ostream;
using std::endl;
using std::cout;

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
    //! Updates node values on a path to a tree root */
    /*! Assumes that the order of values doesn't change */
    void update();
    //! Clears node pointers
    void clear_ptrs();

    //! Insert a value into a subtree
    /*! Returns a pointer to the new node. The new node is inserted before vertices of equal value */
    NodePtr insert(NodePtr node, NodePtr &root);
    //! Finds a node with a given value
    NodePtr find(_Value &value);
    //! Removes vertex from the tree
    void erase(NodePtr &root);
    //! Merge two trees with a middle vertex
    /*! It requires left tree, a node and a right tree to be adeqately ordered */
    NodePtr merge(NodePtr left, NodePtr right);
    //! Split into two trees
    /*! This node become leftmost node of a right tree */
    void split(NodePtr& left, NodePtr& right);    
    //! Split into two trees by erasing a middle vertex
    void split_erase(NodePtr& left, NodePtr& right);

    friend class AVLTree<_Value>;
    friend class AVLTreeIterator<_Value>;

    
    void assert_check() const {
        assert((left == NULL) || 
                (left->parent == this));
        assert((right == NULL) || 
                (right->parent == this));
        assert((parent == NULL) || 
                (parent->left == this) || 
                (parent->right == this));
    }
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

    assert_check();
    y->assert_check();
    
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
    
    assert_check();
    y->assert_check();

    return y;
}

template <class _Value>
AVLTreeNode<_Value>*
AVLTreeNode<_Value>::restore(NodePtr &root) {
    merge_height();
    int balance = get_balance();
    if (balance > 1) {
        NodePtr y = (NodePtr)right;
        if (y->get_balance() == -1) {
          y->rotate_right(root);  
        }
        return rotate_left(root);
    } else if (balance < -1) {
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
AVLTreeNode<_Value>::update() {
    NodePtr node = this;
    while (node != NULL) {
        node->merge_children();
        node = node->parent;
    }
}

template <class _Value>
void
AVLTreeNode<_Value>::clear_ptrs() {
    left = NULL;
    right = NULL;
    parent = NULL;
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
AVLTreeNode<_Value>::insert(NodePtr node, NodePtr &root) {
    NodePtr result = NULL;

    if (value < node->value) {
        if (right != NULL) {
            result = right->insert(node, root);
        } else {
            result = right = node;
            node->parent = this;
        }
    } else {
        if (left != NULL) {
            result = left->insert(node, root);
        } else {
            result = left = node;
            node->parent = this;
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
    NodePtr node = this, son = NULL, old_node = NULL;
    if (node->right == NULL) {
        son = node->left;
    } else {
        node = node->get_next();
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
   
    if (node != this) {
        node->left = left;
        if (node->left != NULL) node->left->parent = node;
        node->right = right;
        if (node->right != NULL) node->right->parent = node;
        node->parent = parent;
        if (node->parent != NULL) {
            if (node->parent->left == this) {
                node->parent->left = node;
            } else {
                node->parent->right = node;
            }
        } else {
            root = node;
        }
    }

    NodePtr path = node->parent;
    while (path != NULL) {
        path = path->restore(root)->parent;
    }
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
    if (lNode) lNode->parent = NULL;
    rNode = right;
    if (rNode) rNode->parent = NULL;
    
    NodePtr node = this, p = node->parent, son;
    while (p != NULL) {
        if (p->left == node) {
            node = p;
            p = node->parent;
            son = node->right;
            node->clear_ptrs();
            if (son) son->parent = NULL;
            rNode = node->merge(rNode, son);
        } else {
            node = p;
            p = node->parent;
            son = node->left;
            node->clear_ptrs();
            if (son) son->parent = NULL;
            lNode = node->merge(son, lNode);
        }
    }

    parent = NULL;
    left = NULL;
    right = NULL;
    if (rNode) {
        rNode->insert(this, rNode);
    } else {
        rNode = this;
    }
}

template <class _Value>
void
AVLTreeNode<_Value>::split_erase(NodePtr& lNode, NodePtr& rNode) {
    lNode = left;
    if (lNode) lNode->parent = NULL;
    rNode = right;
    if (rNode) rNode->parent = NULL;
    
    NodePtr node = this, p = node->parent, son;
    while (p != NULL) {
        if (p->left == node) {
            node = p;
            p = node->parent;
            son = node->right;
            node->clear_ptrs();
            if (son) son->parent = NULL;
            rNode = node->merge(rNode, son);
        } else {
            node = p;
            p = node->parent;
            son = node->left;
            node->clear_ptrs();
            if (son) son->parent = NULL;
            lNode = node->merge(son, lNode);
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
    AVLTreeIterator(NodePtr pptr) : ptr(pptr) {}
public:
    AVLTreeIterator(const AVLTreeIterator &it) : ptr(it.ptr) {}
    AVLTreeIterator() : ptr(NULL) {}
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
    bool operator<(const AVLTreeIterator& it) const {
        return ptr < it.ptr;
    }
    _Value& operator*() const {
        return ptr->value;
    }

    //! Update values on a path to the root of a tree
    /*! Assumes that the order of values doesn't change */
    void update() const {
        ptr->update();
    }
    //! Returns an iterator of a root of a tree
    AVLTreeIterator<_Value> get_root() const {
        return AVLTreeIterator(ptr->get_root());
    }

    friend AVLTree<_Value>;
    friend ostream &operator<< (ostream &stream, const AVLTreeIterator &iter) {
        stream << iter.ptr->value;
/*
        stream << " (" << iter.ptr 
            << "," << iter.ptr->left 
            << "," << iter.ptr->right
            << "," << iter.ptr->parent << ")";
        return stream;
*/
        return stream;
    }
};

//! Balanced structure implementing an AVL tree
/*! It provides interfaces which allow to extend it into any structure
 *  requiring binary balanced tree as a base. */
template <typename _Value>
class AVLTree {
private:
public:
    //! Pointer to a node
    typedef AVLTreeNode<_Value>* NodePtr;
    //! Root of the tree
    NodePtr root;

    typedef AVLTreeIterator<_Value> iterator;
    
    AVLTree() : root(NULL) {}
    //! Inserts a value
    /*! The new value is inserted in front of all equal ones */
    iterator insert(_Value value);
    //! Removes a node pointed by a iterator
    void erase(iterator it);
    //! Removes a node with a given value
    /*! Returns true if the vertex was erased */
    bool erase(_Value value);
    //! Merges two trees by inserting a new value between them
    /*! This tree, the new value and the second tree should be ordered adequately */
    iterator merge_insert(_Value value, AVLTree& right);
    //! Merges two trees
    /*! This tree and the second tree should be ordered adequately */
    void merge(AVLTree& right);

    //! Split into two trees by removing a vertex
    void split_erase(iterator it, AVLTree& left, AVLTree& right);
    //! Split into two trees
    /*! Specified vertex become a leftmost vertex of a right tree */
    void split(iterator it, AVLTree& left, AVLTree& right);

    template <typename _VValue> 
    friend ostream &operator<< (ostream &stream, const AVLTree<_VValue> &tree);
   
    //! Check whether a tree is empty
    bool empty() const;
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
AVLTreeIterator<_Value>
AVLTree<_Value>::insert(_Value value) {
    NodePtr ptr = new AVLTreeNode<_Value>(value, NULL);
    if (root == NULL) {
        root = ptr;
    } else {
        root->insert(ptr, root);
    }
    return iterator(ptr);
}

template <typename _Value>
void
AVLTree<_Value>::erase(iterator it) {
    it.ptr->erase(root);
    delete it.ptr;
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
AVLTree<_Value>::merge(AVLTree<_Value>& right) {
    if (right.empty()) return;
    iterator it = right.begin();
    it.ptr->erase(right.root);
    root = it.ptr->merge(root, right.root);
    right.root = NULL;
}

template <typename _Value>
AVLTreeIterator<_Value>
AVLTree<_Value>::merge_insert(_Value value, AVLTree<_Value>& right) {
    NodePtr ptr = new AVLTreeNode<_Value>(value, NULL);
    root = ptr->merge(root, right.root);
    right.root = NULL;
    return iterator(ptr);
}

template <typename _Value>
void
AVLTree<_Value>::split_erase(iterator it, AVLTree<_Value>& left, AVLTree<_Value>& right) {
    it.ptr->split_erase(left.root, right.root);
    delete it.ptr;
    root = NULL;
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
bool
AVLTree<_Value>::empty() const {
    return root == NULL;
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
