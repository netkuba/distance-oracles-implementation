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

    int initializeGraph(int n, int m, double eps) {
        int s = n*n;

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
        return s;
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
        eeps = eps;
        nn = initializeGraph(n, m, eps);
        initializeState(nn);
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

template <class Oracle>
class FullAPITester {
    Oracle oracle;
    
    int nn;
    double eeps;

    PlanarGraph pg;

    vector< vector<int> > edges;
    vector< pair<int, int> > alledges;
    vector< int > label;
    vector< bool > active;
    set<int> aset, naset;

    int initializeGraph(int n, int m, double eps) {
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
        return s;
    }

    void initializeState(int n) {
        edges = vector< vector<int> >(n);
        alledges.clear();
        label = vector<int>(n);
        active = vector<bool>(n);
        naset.clear();
        for (int i=0; i<n; ++i) {
            label[i] = i;
            active[i] = false;
            naset.insert(i);
        }
    }

    void merge(int l1, int l2, int u, int v, int l) {
        printf("merge: %d + %d (%d - %d) -> %d\n", l1, l2, u, v, l);
        bool a1 = active[l1], a2 = active[l2];
     
        assert(label[u] == l1);
        assert(label[v] == l2);
        assert(a1 == a2);

        edges[u].push_back(v);
        edges[v].push_back(u);
        alledges.push_back(make_pair(u,v));

        active[l1] = false;
        active[l2] = false;
        active[l] = a1;

        for (int i=0; i<nn; ++i) {
            if (label[i] == l1 || label[i] == l2) label[i] = l;
        }

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
    void split(int l, int u, int v, int l1, int l2) {
        printf("split: %d (%d - %d) -> %d + %d\n", l, u, v, l1, l2);
        assert(label[u] == l);
        assert(label[v] == l);

        int uuu[2] = {u, v};
        int vvv[2] = {v, u};
        int lll[2] = {l1, l2};

        bool a = active[l];
        active[l] = false;
        if (a) {
            aset.erase(l);
        } else {
            naset.erase(l);
        }
        for (int i=0; i<nn; ++i) {
            if (label[i] == l) label[i] = -1;
        }

        for (int i=0; i<(int)alledges.size(); ++i) {
            if ((alledges[i] == make_pair(u,v)) ||
                    (alledges[i] == make_pair(v,u))) {
                alledges[i] = alledges.back();
                alledges.pop_back();
                break;
            }
        }
        
        for (int k=0; k<2; ++k) {
            int uu = uuu[k], vv = vvv[k];
            int ll = lll[k];
            int ok = false;
            for (int i=0; i<edges[uu].size(); ++i) {
                if (edges[uu][i] == vv) {
                    edges[uu][i] = edges[uu].back();
                    edges[uu].pop_back();
                    ok = true;
                    break;
                }
            }
            assert(ok);

            active[ll] = a;
            if (a) {
                aset.insert(ll);
            } else {
                naset.insert(ll);
            }

            vector<int> us(1,uu);
            for (int i=0; i<(int)us.size(); ++i) {
                if (label[us[i]] != -1) continue;
                label[us[i]] = ll;
                for (int j=0; j<(int)edges[us[i]].size(); ++j) {
                    us.push_back(edges[us[i]][j]);
                }
            }
        }
    }

    void activate(int l) {
        printf("activate %d\n", l);
        assert(!active[l]);
        active[l] = true;
        naset.erase(l);
        aset.insert(l);
    }

    void deactivate(int l) {
        printf("deactivate %d\n", l);
        assert(active[l]);
        active[l] = false;
        aset.erase(l);
        naset.insert(l);
    }

    W distance_to_color(int v, int l) {
        vector<W> dist;
        getDistances(pg, v, dist);

        W res = infinity;
        for (int u=0; u<nn; ++u) {
            if (label[u] == l) {
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
            if (active[label[u]]) {
                res = min(res, dist[u]);
            }
        }
        return res;
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

        oracle.activate(l);
        activate(l);
    }
    
    void randomDeactivateQuery() {
        if ((int)aset.size() < 2) return;

        int x = rand()%(aset.size());
        int l;
        {
            auto it = aset.begin();
            while (x--) ++it;
            l = *it;
        }

        oracle.deactivate(l);
        deactivate(l);
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

        int lll[2] = {l1, l2};
        int vvv[2];

        for (int k=0; k<2; ++k) {
            int ll = lll[k];
            int c = 0;
            for (int i=0; i<nn; ++i) if (label[i] == ll) ++c;
            int x = rand()%c;
            c = 0;
            for (int i=0; i<nn; ++i) if (label[i] == ll) {
                if (c == x) vvv[k] = i;
                ++c;
            }
        }

        int l = oracle.merge(l1, l2, vvv[0], vvv[1]);
        merge(l1, l2, vvv[0], vvv[1], l);
    }

    void randomSplitQuery() {
        if (alledges.empty()) return;

        int x = rand()%alledges.size();
        int u = alledges[x].first, v = alledges[x].second;
        int l = label[u];

        pair<int, int> ls = oracle.split(l, u, v);
        split(l, u, v, ls.first, ls.second);
    }

public:
    FullAPITester(int n, int m, double eps) {
        eeps = eps;
        nn = initializeGraph(n, m, eps);
        initializeState(nn);
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

        for (int i=0; i<nn; ++i) {
            printf("i: %d\n", i);

            randomSplitQuery();
            if (rand()%(aset.size() + naset.size()) < aset.size()) {
                randomDeactivateQuery();
            }

            for (int t=0; t<10; ++t) {
                randomDistanceQuery();
                randomClosestQuery();
            }
        }
    }

};
