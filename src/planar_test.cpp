#include "planar.h"
#include "incremental_oracle.h"

#include <cstdio>
#include <cstdlib>
#include <ctime>
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

//const int n = 50;
const int m = 1000;
const W eps = 0.5;

const int t = 10000;

int main() {
    for (int n = 1; n < 100; ++n)
//    int n = 100;
    {
        printf("%d\n", n);
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

        IncrementalPlanarOracle oracle(n*n, edge, weight, eps);

        PlanarGraph pg(n*n);
        for (int i=0; i<(int)edge.size(); ++i) pg.add_edge(edge[i].first, edge[i].second, weight[i]);
        vector<W> dist;

        clock_t begin = clock();

        for (int tt=0; tt<t; ++tt) {
            int u = rand()%(n*n), v = rand()%(n*n);
            if (u == v) continue;
            W approx = oracle.distance_to_color(u, v);
            getDistances(pg, v, dist);
            W exact = dist[u];
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

        clock_t end = clock();

        printf("%lf s\n", double(end-begin) / CLOCKS_PER_SEC);
    }
    return 0;
}
