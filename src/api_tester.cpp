#include "FindUnion.h"
#include "oracle_internal.h"
#include <signal.h>
#include <map>
#include <set>

using std::set;
using std::map;
using std::make_pair;
using std::min;

template <class Oracle>
class IncrementalAPITester {
private:
    Oracle oracle;
    
    int nn;
    double eeps;

    PlanarGraph pg;
    FindUnion fu;
    vector<bool> active;
    map<int, int> mapping;

    set<int> aset, naset;

    void initializeGraph(int n, int m, double eps) {
        int s = n*n;
        eeps = eps;
        nn = s;

        vector< pair< int, int > > edge;
        vector< W > weight;
        for (int i=0; i<n-1; ++i)
            for (int j=0; j<n; ++j) {
                edge.push_back(make_pair(i*n+j, (i+1)*n+j));
                weight.push_back(rand()%m+1);
            }
        for (int i=0; i<n; ++i)
            for (int j=0; j<n-1; ++j) {
                edge.push_back(make_pair(i*n+j, i*n+j+1));
                weight.push_back(rand()%m+1);
            }
        oracle = Oracle(s, edge, weight, eps);
    
        pg = PlanarGraph(s);
        for (int i=0; i<(int)edge.size(); ++i) pg.add_edge(edge[i].first, edge[i].second, weight[i]);
    }

    void initializeState(int n) {
        fu = FindUnion(n);
        active = vector<bool>(n, false);
        mapping.clear();
        for (int i=0; i<n; ++i) {
            mapping[i] = i;
            naset.insert(i);
        }
    }

    void merge(int l1, int l2, int l) {
        int k1 = mapping[l1], k2 = mapping[l2];
        bool a1 = active[k1], a2 = active[k2];
        
        assert(a1 == a2);

        int k = fu.unionn(k1, k2);
        mapping[l] = k;
        active[k] = a1;

        printf("merge: %d + %d -> %d    (%d + %d -> %d)\n", l1, l2, l, k1, k2, k);
        if (a1) {
            aset.erase(l1);
            aset.erase(l2);
            aset.insert(l);
        } else {
            naset.erase(l1);
            naset.erase(l2);
            naset.insert(l);
        }   
    }

    void activate(int l) {
        printf("activate: %d   (%d)\n", l, mapping[l]);
        int k = mapping[l];
        active[k] = true;
        naset.erase(l);
        aset.insert(l);

    }

    W distance_to_color(int v, int l) {
        vector<W> dist;
        getDistances(pg, v, dist);
        int k = mapping[l];

        W res = infinity;
        for (int u=0; u<nn; ++u) {
            if (fu.find(u) == k) {
                res = min(res, dist[u]);
            }
        }
        return res;
    }

    W distance_to_closest(int v) {
        vector<W> dist;
        getDistances(pg, v, dist);
        
        W res = infinity;
        for (int u=0; u<nn; ++u) {
            if (active[fu.find(u)]) {
                res = min(res, dist[u]);
            }
        }
        return res;
    }

    int closest(int v) {
        vector<W> dist;
        getDistances(pg, v, dist);
        
        int last = -1;
        W res = infinity;
        for (int u=0; u<nn; ++u) {
            if (active[fu.find(u)]) {
                res = min(res, dist[u]);
                if (res == dist[u]) last = u;
            }
        }
        return last;
    }

    void randomDistanceQuery() {
        int v = rand()%nn, l;
        int x = rand()%(aset.size() + naset.size());
        if (x < aset.size()) {
            auto it = aset.begin();
            while (x--) ++it;
            l = *it;
        } else {
            x -= aset.size();
            auto it = naset.begin();
            while (x--) ++it;
            l = *it;
        }
        
        W exact = distance_to_color(v, l);
        W approx = oracle.distance_to_color(v, l);

        assert(exact <= approx);
        assert(approx <= exact * (1 + eeps));
    }

    void randomClosestQuery() {
        int v = rand()%nn;
        W exact = distance_to_closest(v);
        W approx = oracle.distance_to_closest(v);

        assert(exact <= approx);
        assert(approx <= exact * (1 + eeps));
    }

    void randomActivateQuery() {
        if (naset.empty()) return;

        int x = rand()%(naset.size());
        int l;
        {
            auto it = naset.begin();
            while (x--) ++it;
            l = *it;
        }

        activate(l);
        oracle.activate(l);
    }

    void randomMergeQuery() {
        int l1, l2;
        int x = rand()%(aset.size() + naset.size());
        if (x < aset.size()) {
            int y = rand()%aset.size(), z = rand()%aset.size();
            auto it1 = aset.begin(), it2 = aset.begin();
            while (y--) ++it1;
            while (z--) ++it2;
            l1 = *it1;
            l2 = *it2;
        } else {
            int y = rand()%naset.size(), z = rand()%naset.size();
            auto it1 = naset.begin(), it2 = naset.begin();
            while (y--) ++it1;
            while (z--) ++it2;
            l1 = *it1;
            l2 = *it2;
        }
        if (l1 == l2) return;

        int l = oracle.merge(l1, l2);
        merge(l1, l2, l);
    }


public:
    IncrementalAPITester(int n, int m, double eps) : fu(0) {
        initializeGraph(n, m, eps);
        initializeState(n*n);
    }

    void runSimpleTest() {
        for (int i=0; i<nn; ++i) {

            printf("i: %d\n", i);

            randomMergeQuery();
            if (rand()%(aset.size() + naset.size()) < naset.size()) {
                randomActivateQuery();
            }

            for (int t=0; t<10; ++t) {
                randomDistanceQuery();
                randomClosestQuery();
            }

        }
    }
};
