#ifndef _FULL_ORACLE_H_
#define _FULL_ORACLE_H_

#include "oracle.h"
#include "ETTree.h"

#include <utility>
using std::min;
using std::make_pair;

class FullPlanarOracle : public PlanarOracle {

    typedef vector< pair<int, W> > LList;
    struct ETValue {
        LList L; // pair (p, N_i^p(x))
    };

    struct ETMerger {
        LList L;

        void merge_helper(LList& left, const LList& right) {
            LList result;
            int p = 0, q = 0;
            while ((p < (int)left.size()) && (q < (int)right.size())) {
                if (left[p].first < right[q].first) {
                    result.push_back(left[p++]);
                } else if (left[p].first > right[q].first) {
                    result.push_back(right[q++]);
                } else {
                    result.push_back(make_pair(
                                left[p].first, 
                                min(left[p].second, right[q].second)));
                    ++p;
                    ++q;
                }
            }
            while (p < (int)left.size()) result.push_back(left[p++]);
            while (q < (int)right.size()) result.push_back(right[q++]);
            swap(left, result);
        }

        void merge_both(ETValue* curr, const ETMerger& left, const ETMerger& right) {
            if (curr != NULL) L = curr->L;
            else L = LList();
            merge_helper(L, left.L);
            merge_helper(L, right.L);
        }

        void merge_left(ETValue* curr, const ETMerger& left) {
            if (curr != NULL) L = curr->L;
            else L = LList();
            merge_helper(L, left.L);
        }

        void merge_right(ETValue* curr, const ETMerger& right) {
            if (curr != NULL) L = curr->L;
            else L = LList();
            merge_helper(L, right.L);
        }
        
        void merge_none(ETValue* curr) {
            if (curr != NULL) L = curr->L;
            else L = LList();
        }

        W getDistance(int p) const {
            assert(!L.empty());
            if (L.empty()) return infinity;
            int a = 0, b = L.size(), c;
            while (b-a>1) {
                c = (a+b)/2;
                if (L[c].first <= p) {
                    a = c;
                } else {
                    b = c;
                }
            }
            if (L[a].first == p) return L[a].second;
            return infinity;
        }
    };

    struct Portal {
        int p, v;
        set< pair<W, int> > H;
        
        Portal(int pp, int vv) : p(pp), v(vv) {}
    };
    
    struct Label {
        bool active = false;
    };

    virtual
    bool isOfColor(int v, int c) {
        return forest.vertex_tree_number(v) == c;
    }

    virtual
    bool isActive(int v) {
        return labels[forest.vertex_tree_number(v)].active;
    }
    
    virtual
    void processLeaf(
            int i,
            const PlanarGraph& pg,
            const vector<int>& mapping,
            const vector<bool>& source);

    virtual
    void processPortals(
            int i,
            const PlanarGraph& pg,
            const vector<int>& mapping,
            const vector<int>& portal,
            const vector<bool>& source);
    
    vector< Label > labels;
    vector< Portal > portals;
    ETForest<ETValue, ETMerger> forest;

public:
    FullPlanarOracle(
            int n,
            const vector< pair< int, int > >& edges,
            const vector< W >& weights,
            W eps) : labels(n), forest(n) {
        initialize(n, edges, weights, eps);
        for (int v=0; v<n; ++v) forest.update(v);
    }

    int merge(int l1, int l2, int u, int v);
    pair<int, int> split(int l, int u, int v);
    
    void activate(int l);
    void deactivate(int l);
    
    W distance_to_color(int v, int l);
    W distance_to_closest(int v);
};

#endif
