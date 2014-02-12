#include "oracle_internal.h"

#include <cassert>
#include <queue>
#include <cstdio>

using std::printf;
using std::priority_queue;
using std::pair;
using std::make_pair;
using std::greater;
using std::max;

void
getAlphaFamily(
        PlanarGraph& g, 
        double alpha, 
        vector< PlanarGraph >& subgs,
        vector< vector<int> >& mappings,
        vector< vector<int> >& parents) {

    vector<int> parent(g.vs().size(), -1);
    vector<W> dist(g.vs().size(), infinity); 
    typedef pair<W, int> QEl;
    priority_queue< QEl, vector<QEl>, greater<QEl> > queue;

    dist[0] = 0;
    queue.push(make_pair(0, 0));
    while (!queue.empty()) {
        QEl curr = queue.top(); queue.pop();
        int u = curr.second;
        W d = curr.first;
        if (d != dist[u]) continue;
        for (int e: g.vs()[u].edges) {
            int v = g.opp(u, e);
            if (dist[v] > d + g.es()[e].w) {
                dist[v] = d + g.es()[e].w;
                parent[v] = e;
                queue.push(make_pair(dist[v], v));
            }
        }
    }

    vector<int> layer(g.vs().size());
    int maxLayer = 0;
    for (int i=0; i<(int)g.vs().size(); ++i) {
        layer[i] = (int)(dist[i] / alpha);
        maxLayer = max(maxLayer, layer[i]);
    }

    typedef PlanarGraph::Vertex Vertex;
    typedef PlanarGraph::Edge Edge;

    vector< pair< PlanarGraph, vector<int> > > result;
    
    for (int l = 0; l <= maxLayer; ++l) {
        int la = l-1, lb = l+1; 

        vector<int> mapping;
        vector<int> vInd(g.vs().size(), -1), eInd(g.es().size(), -1);

        PlanarGraph subg(1);
        mapping.push_back(-1);

        for (int i=0; i<(int)layer.size(); ++i) {
            if (la <= layer[i] && layer[i] <= lb) {
                vInd[i] = subg.vs().size();
                mapping.push_back(i);
                subg.vs().push_back(Vertex());
            }
        }

        vector<int> subparent(subg.vs().size(), -1);
        for (int e=0; e<(int)g.es().size(); ++e) {
            int u = g.es()[e].u;
            int v = g.es()[e].v;
            int lu = layer[u];
            int lv = layer[v];

            if ((la <= lu) && (lu <= lb) && 
                    (la <= lv) && (lv <= lb)) {
                if (e == parent[u]) {
                    subparent[vInd[u]] = subg.es().size();
                }
                if (e == parent[v]) {
                    subparent[vInd[v]] = subg.es().size();
                }
                eInd[e] = subg.es().size();
                subg.add_edge(vInd[u], vInd[v], g.es()[e].w);
            } else if((la <= lu) && (lu <= lb) && 
                    (la > lv) && (e == parent[u])) {
                subparent[vInd[u]] = subg.es().size();
                eInd[e] = subg.es().size();
                subg.add_edge(0, vInd[u], dist[u] - la*alpha);
            } else if ((la > lu) && (e == parent[v]) &&
                    (la <= lv) && (lv <= lb)) {
                subparent[vInd[v]] = subg.es().size();
                eInd[e] = subg.es().size();
                subg.add_edge(0, vInd[v], dist[v] - la*alpha);
            }
        }

        if (la <= 0) {
            subparent[vInd[0]] = subg.es().size();
            subg.add_edge(0, vInd[0], -la*alpha);
        }

        subgs.push_back(subg);
        mappings.push_back(mapping);
        parents.push_back(subparent);
    }
}

static void printEmbedded(PlanarGraph& pg) {
    for (int v=0; v<(int)pg.vs().size(); ++v) {
        printf("Vertex %d:\n", v);
        if (pg.vs()[v].edges.empty()) continue;
        int e_end = pg.vs()[v].edges[0], e = e_end;
        do {
            printf("%d - %d\n", pg.es()[e].u, pg.es()[e].v);
            e = pg.eNext(v, e);
        } while (e != e_end);
    }
    return;
}

void
subdivide(
        PlanarGraph g,
        vector<int> parent,
        vector< PlanarGraph >& subs,
        vector< vector<int> >& mappings,
        vector< vector<int> >& parents,
        vector< vector< pair<int, int > > >& paths) {

    if (g.vs().size() <= 3) return;

    embed(g);
    triangulate(g);
/*
    for (int i=0; i<(int)parent.size(); ++i) {
        if (parent[i] == -1) printf("-1 ");
        else printf("%d ", g.opp(i,parent[i]));
    }
    printf("\n");
    printEmbedded(g);
*/
    int eC = 0;
    vector< vector< pair< int, int > > > eNum(g.es().size());

    if (g.vs()[0].edges.empty()) return;
    int v = 0, f = g.vs()[0].edges[0], ff, e = ff = g.eNext(v,f);
    g.eNext(0,f) = -1;

    while (e != -1) {
        if (e == parent[v]) {
            v = g.opp(v,e); ++eC;
            e = g.eNext(v,e);
        } else {
            int u = g.opp(v, e);
            if (e == parent[u]) {
                v = u; ++eC;
                e = g.eNext(v,e);
            } else {
                eNum[e].push_back(make_pair(v, eC));
                e = g.eNext(v,e);
            }
        }
    }
    g.eNext(0,f) = ff;

    int x[3], xe[3];
    x[0] = 0;
    xe[0] = g.vs()[0].edges[0];

    printEmbedded(g);

    bool stop = false;   
    while (!stop) {
        stop = true;

        x[1] = g.opp(x[0], xe[0]);
        xe[1] = g.eNext(x[1], xe[0]);
        x[2] = g.opp(x[1], xe[1]);
        xe[2] = g.eNext(x[2], xe[1]);
        assert(x[0] == g.opp(x[2], xe[2]));

        printf("Trying %d %d %d\n", x[0], x[1], x[2]);

        for (int i=0; i<3; ++i) {
            int a = x[i], b = x[(i+1)%3];
            int e = xe[i];
            int n;

            if (eNum[e].empty()) continue;
            if (eNum[e][0].first == a && eNum[e][1].first == b) {
                n = eNum[e][1].second - eNum[e][0].second;
            } else if (eNum[e][0].first == b && eNum[e][1].first == a) {
                n = eNum[e][0].second - eNum[e][1].second;
            } else {
                assert(false);
            }
            if (n <= 0) n += (g.vs().size()-1) * 2;

            for (int i=0; i<eNum[e].size(); ++i) {
                printf("- %d %d\n", eNum[e][i].first, eNum[e][i].second);
            }
            printf("Got %d : (%d %d)\n", n, a, b);
            if (n > (g.vs().size()-1) * 4 / 3) {
                stop = false;
                xe[0] = xe[i];
                x[0] = x[(i+1)%3];
                break;
            }
        }
    }

    for (int i=1; i<(int)g.vs().size(); ++i) {
        printf("%d <- %d\n", i, g.opp(i, parent[i]));
    }

    for (int i=0; i<3; ++i) {
        vector< pair< int, int > > path;
        int v = x[i];
        while (v != 0) {
            path.push_back(make_pair(v, parent[v]));
            printf("%d ", v);
            v = g.opp(v, parent[v]);
        }
        path.push_back(make_pair(0, -1));
        printf("0\n");
        paths.push_back(path);
    }
}
