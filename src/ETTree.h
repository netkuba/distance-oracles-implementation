#ifndef _ETTREE_H_
#define _ETTREE_H_

#include <set>
#include <map>
#include "AVLTree.h"

using std::set;
using std::map;
using std::vector;
using std::swap;

template <class _Value, class _Merger>
class ETForest {
public:

    struct ValueBox;
    struct TreeValue {
        
        bool real;
        ValueBox* ptr;
        int treeNum;
        _Merger merger;

        TreeValue(ValueBox* pptr, bool rreal = false) : 
                real(rreal), ptr(pptr) {}

        bool operator<(const TreeValue& b) const { return false; }

        void merge_both(const TreeValue& left, const TreeValue& right) {
            if (real) {
                merger.merge_both(&ptr->value, left.merger, right.merger);
            } else {
                merger.merge_both(NULL, left.merger, right.merger);
            }
        }
        
        void merge_left(const TreeValue& left) {
            if (real) {
                merger.merge_left(&ptr->value, left.merger);
            } else {
                merger.merge_left(NULL, left.merger);
            }
        }
        
        void merge_right(const TreeValue& right) {
            if (real) {
                merger.merge_right(&ptr->value, right.merger);
            } else {
                merger.merge_right(NULL, right.merger);
            }
        }
        void merge_none() {
            if (real) {
                merger.merge_none(&ptr->value);
            } else {
                merger.merge_none(NULL);
            }
        }

    };

    typedef AVLTree<TreeValue> Tree;
    typedef class Tree::iterator Iter;

    struct ValueBox {
        _Value value;
        _Merger merger;
        
        typedef set<Iter> Set;
        typedef class Set::iterator SetIter;

        int iterNum;
        Iter trueIter;
        set<Iter> falseIters;

        ValueBox() : iterNum(0) {}
        
        void remove_iter(const Iter& it) {
            if (trueIter == it) {
                SetIter newIt = falseIters.begin();
                if (newIt != falseIters.end()) { 
                    (**newIt).real = true;
                    newIt->update();
                    
                    trueIter = *newIt;
                    falseIters.erase(newIt);
                }
            } else {
                falseIters.erase(it);
            }
            --iterNum;
            if (iterNum == 0) {
                merger.merge_none(&value);
            }
        }

        const Iter& add_iter(const Iter& it) {
            if (iterNum) {
                falseIters.insert(it);
            } else {
                trueIter = it;
                (*it).real = true;
                it.update();
            }
            ++iterNum;
            return it;
        }
        
        void update_value() {
            if (iterNum == 0) {
                merger.merge_none(&value);
            } else {
                merger.merge_none(NULL);
                trueIter.update();
            }
        }
    };
   
    map< pair<int, int>, Iter > edgeMap;
    vector<ValueBox> values;
    vector< Tree > trees;

public:
    //! Construct a forest of n ET-trees
    ETForest(int n) : values(n), trees(n) {}

    //! Returns a reference to a value of vertex
    _Value& vertex_value(int u) 
        { return values[u].value; }

    //! Returns a reference to a value of vertex ET-tree root
    _Merger& vertex_tree_value(int u) {
        if (values[u].iterNum == 0) return values[u].merger;
        return (*values[u].trueIter.get_root()).merger;
    }

    int vertex_tree_number(int u) {
        if (values[u].iterNum == 0)  return u;
        return (*values[u].trueIter.get_root()).treeNum;
    }

    //! Returns a reference to a value of ET-tree root
    const _Merger& tree_value(int a) { 
        if (trees[a].empty()) return values[a].merger;
        return (*trees[a].get_root()).ptr->merger;
    }

    //! Updates the structure after modification of a value
    void update(int u) { 
        values[u].update_value();
    }

    int merge(int a, int b, int u, int v) {
        if (values[u].iterNum) {
            Iter it = values[u].trueIter;
            Tree al, ar;
            trees[a].split(it, al, ar);
            ar.merge(al);
            swap(trees[a], ar);
        }
        if (values[v].iterNum) {
            Tree al, ar;
            Iter it = values[v].trueIter;
            Tree bl, br;
            trees[b].split(it, bl, br);
            br.merge(bl);
            swap(trees[b], br);
        }
        edgeMap[make_pair(u,v)] = values[u].add_iter(trees[a].merge_insert(TreeValue(&values[u]), trees[b]));
        edgeMap[make_pair(v,u)] = values[v].add_iter(
                trees[a].insert(TreeValue(&values[v])));
        (*trees[a].get_root()).treeNum = a;
        return a;
    }

    pair<int, int> split(int a, int u, int v) {
        Iter uv = edgeMap[make_pair(u,v)];
        Iter vu = edgeMap[make_pair(v,u)];
        edgeMap.erase(make_pair(u,v));
        edgeMap.erase(make_pair(v,u));

        Tree lt, rt, ltt, mtt, rtt;
        values[u].remove_iter(uv);
        values[v].remove_iter(vu);
        trees[a].split_erase(uv, lt, rt);
        if (lt.get_root() == vu.get_root()) {
            rtt = rt;
            lt.split_erase(vu, ltt, mtt);
          
            trees[u] = mtt;
            rtt.merge(ltt);
            trees[v] = rtt;
        } else {
            ltt = lt;
            rt.split_erase(vu, mtt, rtt);
            trees[v] = mtt;
            rtt.merge(ltt);
            trees[u] = rtt;
        }
        if (!trees[u].empty()) (*trees[u].get_root()).treeNum = u;
        if (!trees[v].empty()) (*trees[v].get_root()).treeNum = v;
        return make_pair(u, v);
    }
};

#endif
