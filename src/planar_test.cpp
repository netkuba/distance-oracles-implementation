#include "planar.h"
#include "oracle.h"

#include <cstdio>
#include <cstdlib>
using namespace std;

void printEmbedded(PlanarGraph& pg) {
    for (int v=0; v<(int)pg.vs().size(); ++v) {
        printf("Vertex %d:\n", v);
        if (pg.vs()[v].edges.empty()) continue;
        int e_end = pg.vs()[v].edges[0], e = e_end;
        do {
            printf("%d - %d  (%.1lf)\n", pg.es()[e].u, pg.es()[e].v, pg.es()[e].w);
            e = pg.eNext(v, e);
        } while (e != e_end);
    }
    return;
}

const int n = 100;
const int m = 1000;
const W eps = 0.5;

const int t = 1000;

int main() {
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

    PlanarOracle oracle(n*n, edge, weight, eps);

    printf("PYTAM! %d\n", oracle.pieces.size());
/*
    for (int j=0; j<oracle.pieces.size(); ++j) {
        printf("Graf %d\n", j);
        auto pg = oracle.pieces[j].first;
        auto mapping = oracle.pieces[j].second;
        auto portals = oracle.portals;
        embed(pg);
        printEmbedded(pg);
        for (int i=0; i<mapping.size(); ++i) printf("%d ", mapping[i]);
        printf("\n");
        for (int i=0; i<portals.size(); ++i) if (portals[i].p == j) printf("%d ", portals[i].v);
        printf("\n\n");
    }
    printf("Portals!\n");
    auto portals = oracle.portals;
    for (int i=0; i<portals.size(); ++i) printf("%d %d!\n", portals[i].p, portals[i].v);
*/
    int sum = 0;
    for (int i=0; i<(int)oracle.portals.size(); ++i) {
        sum += oracle.portals[i].to_vertex.size();
    }

    printf("%d / %d = %d\n", sum, n*n, sum / (n*n));

    for (int tt=0; tt<t; ++tt) {
        int u = rand()%(n*n), v = rand()%(n*n);
        if (u == v) continue;
        W exact = oracle.exact_to_vertex(u, v);
        W approx = oracle.distance_to_vertex(u, v);
        if (approx != exact) {
            printf("%.4lf - %.4lf\n", exact, approx);
        }
        if (approx < exact) {
            printf("%d - %d\n", u, v);
            assert(approx >= exact);
        }
        if (approx > exact * (1 + eps)) {
            printf("%d - %d\n", u, v); 
            assert(approx <= exact * (1 + eps));
        }
    }
    return 0;
}
