#include "graph.h"

#include <queue>
#include <cstdio>
using std::printf;
using std::priority_queue;

pair<Weight, Weight> getStretch(G &g) {
    typedef G::Edge Edge;

    if (g.empty()) return make_pair(0,0);
    
    Weight minD = std::numeric_limits<Weight>::max();
    Weight maxD = std::numeric_limits<Weight>::min();


    for (Edge& e: g.es()) {
        minD = min(minD, e.value.w);
        maxD = max(maxD, e.value.w);
    }

    return make_pair(minD, maxD * (g.vs().size() - 1));
}

vector< SubG > getAlphaFamily(G &g, Weight alpha) {
    typedef pair<Weight, int> El;
    typedef priority_queue< El, vector<El>, std::greater<El> > Q;

    vector<int> layer(g.vs().size(), -1);
    vector<G::Edge*> parent(g.vs().size(), NULL);
    vector<Weight> dist(g.vs().size(), std::numeric_limits<Weight>::max());

    dist[0] = 0;
    Q que;

    int cLayer = 0;
    vector<int> cLayerV;
    
    que.push(make_pair(0, 0));
    while (!que.empty()) {
        El curr = que.top(); que.pop();
        if (dist[curr.second] != curr.first) continue;

        if (curr.first > alpha) {
            que.push(curr);

            ++cLayer;
            while (!cLayerV.empty()) {
                int v = cLayerV.back(); cLayerV.pop_back();
                for (G::Edge* e: g.vs()[v].edges) {
//                    Weight d = e->value.w;
                    Weight d = 0;
                    int u = e->opp(&g.vs()[v])->ind;
                    if (layer[u] == -1) {
                        if (dist[u] > d) {
                            dist[u] = d;
                            parent[u] = e;
                            que.push(make_pair(d, u));
                        }
                    }
                }
            }

            continue;
        }

        int v = curr.second;
        layer[v] = cLayer;
        cLayerV.push_back(v);
        
        for (G::Edge* e: g.vs()[v].edges) {
            Weight d = dist[v] + e->value.w;
            int u = e->opp(&g.vs()[v])->ind;
            if (dist[u] > d) {
                dist[u] = d;
                parent[u] = e;
                que.push(make_pair(d, u));
            }
        }
    }
    ++cLayer;

    vector<SubG> result;
    for (int i=1; i<cLayer-1; ++i) {
        int a = i-1, b = i+1;
        typedef SubG::Edge Edge;
        typedef SubG::Vertex Vertex;


        vector<int> vInd(g.vs().size(), -1);
        vector<int> eInd(g.es().size(), -1);

        SubG curr;
        curr.vs().push_back(Vertex(NULL));
        for (G::Vertex &v: g.vs()) {
            if ((layer[v.ind] < a) || (layer[v.ind] > b)) continue;
            vInd[v.ind] = curr.vs().size();
            curr.vs().push_back(Vertex(SubVVal(&v)));
        }

        if (a == 0) {
            curr.es().push_back(Edge(
                    NULL,
                    &curr.vs()[vInd[0]],
                    &curr.vs()[0]));
        }
        for (auto &e: g.es()) {
            int ul = layer[e.u->ind], vl = layer[e.v->ind];
            if ((a <= ul) && (ul <= b) && (a <= vl) && (vl <= b)) {
                eInd[e.ind] = curr.es().size();
                curr.es().push_back(Edge(
                        &e,
                        &curr.vs()[vInd[e.u->ind]],
                        &curr.vs()[vInd[e.v->ind]]));
            }
        }
        for (auto &v: g.vs()) if (vInd[v.ind] != -1) {
            G::Edge* e = parent[v.ind];
            if (e != NULL) {
                if (e->opp(&v)->ind < a) {
                    eInd[e->ind] = curr.es().size();
                    curr.es().push_back(Edge(
                            e,
                            &curr.vs()[vInd[v.ind]],
                            &curr.vs()[vInd[e->opp(&v)->ind]]));
                }
            }
        }
        for (auto &v: g.vs()) if (vInd[v.ind] != -1) {
            for (auto *e: v.edges) {
               if (eInd[e->ind] != -1) {
                    curr.vs()[vInd[v.ind]].edges.push_back(&curr.es()[eInd[e->ind]]);
               }
            }
        }
        if (a == 0) {
            curr.vs()[0].edges.push_back(&curr.es()[0]);
            curr.vs()[vInd[0]].edges.push_back(&curr.es()[0]);
        }

        curr.updateIndices();
        result.push_back(curr);
    }
    return result;
}
