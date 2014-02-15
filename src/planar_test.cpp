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
            printf("%d - %d\n", pg.es()[e].u, pg.es()[e].v);
            e = pg.eNext(v, e);
        } while (e != e_end);
    }
    return;
}

const int n = 20;
const int m = 100;
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

    printf("\n\n ==== ===== ==== \n\n");
/*
    int u = 6, v = 10;
    for (int i=0; i<oracle.pieces.size(); ++i) {
        vector<int> mapping = oracle.pieces[i].second;
        bool oku = false, okv = false;
        for (int j=0; j<mapping.size(); ++j) {
            if (mapping[j] == u) oku = true;
            if (mapping[j] == v) okv = true;
        }
        if (!oku || !okv) continue;

        embed(oracle.pieces[i].first);
        printEmbedded(oracle.pieces[i].first);

        printf("Portals\n");
        for (int j=0; j<oracle.portals.size(); ++j) {
            if (oracle.portals[j].p == i) {
                printf("%d ", oracle.portals[j].v);
            }
        }
        printf("\n");

        printf("Mapping\n");
        for (int j=0; j<mapping.size(); ++j) {
            printf("%d ", mapping[j]);
        }
        printf("\n\n");
    }

    printf("%d!\n", oracle.pieces.size());
*/
    for (int tt=0; tt<t; ++tt) {
        int u = rand()%(n*n), v = rand()%(n*n);
        if (u == v) continue;
        W exact = oracle.exact_to_vertex(u, v);
        W approx = oracle.distance_to_vertex(u, v);
        printf("%.4lf - %.4lf\n", exact, approx);
        if (approx < exact) {
            printf("%d - %d\n", u, v);
            assert(approx >= exact);
        }
        if (approx > exact * (1 + eps)) {
            printf("%d - %d\n", u, v); 
            assert(approx <= exact * (1 + eps*2));
        }
    }
    return 0;
}
