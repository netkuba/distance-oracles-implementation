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

const int n = 100;
const int m = 5;

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

    PlanarOracle oracle(n*n, edge, weight, 5);

    for (int i=0; i<(int)oracle.pieces.size(); ++i) {
        PlanarGraph& pg = oracle.pieces[i].first;
        vector<int>& mapping = oracle.pieces[i].second;
        vector<int>& portal = oracle.portals[i];
/*
        embed(pg);
        printf("Piece %d\n", i);
        printEmbedded(pg);
        printf("\n");
        printf("Mapping:\n");
        for (auto v: mapping) {
            printf("%d ", v);
        }
        printf("\n");
        printf("Portals:\n");
        for (auto v: portal) {
            printf("%d ", v);
        }
        printf("\n");
        printf("\n");*/
    }
/*
    for (int i=0; i<(int)oracle.graph.vs().size(); ++i) {
        printf("Clusters %d (%d): \n", i, (int)oracle.clusters[i].size());
        for (auto c: oracle.clusters[i]) {
            printf("%d - %d\n", c.first, c.second);
        }
    }
*/
    return 0;
}
