#include "api_tester.cpp"

#include "planar.h"
#include "incremental_oracle.h"
#include "full_oracle.h"

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

const int n = 100;
const int m = 100;
const W eps = 0.5;

const int t = 10000;

clock_t begint, endt;
void start_timer() {
    begint = clock();
}

void stop_timer() {
    endt = clock();
    printf("- %lf s\n", double(endt-begint) / CLOCKS_PER_SEC);
}

int main() {

    IncrementalAPITester<IncrementalPlanarOracle> tester(30, 4000, 0.5);
    tester.runSimpleTest();

//    for (int n = 1; n < 100; ++n)
/*    int n = 100;
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

        start_timer();
        IncrementalPlanarOracle incrementalOracle(n*n, edge, weight, eps);
        stop_timer();
        start_timer();
        FullPlanarOracle fullOracle(n*n, edge, weight, eps);
        stop_timer();

        PlanarGraph pg(n*n);
        for (int i=0; i<(int)edge.size(); ++i) pg.add_edge(edge[i].first, edge[i].second, weight[i]);
        vector<W> dist;

        start_timer();
        for (int tt=0; tt<t; ++tt) {
            int u = rand()%(n*n), v = rand()%(n*n);
            if (u == v) continue;
            W approx1 = incrementalOracle.distance_to_color(u, v);
        }
        stop_timer();

        start_timer();
        for (int tt=0; tt<t; ++tt) {
            int u = rand()%(n*n), v = rand()%(n*n);
            if (u == v) continue;
            W approx2 = fullOracle.distance_to_color(u, v);
        }
        stop_timer();

        start_timer();
        for (int tt=0; tt<t; ++tt) {
            int u = rand()%(n*n), v = rand()%(n*n);
            if (u == v) continue;
            getDistances(pg, v, dist);
            W exact = dist[u];
        }
        stop_timer();
*/
/*
        for (int tt=0; tt<t; ++tt) {
            int u = rand()%(n*n), v = rand()%(n*n);
            if (u == v) continue;
            W approx1 = incrementalOracle.distance_to_color(u, v);
            W approx2 = fullOracle.distance_to_color(u, v);
            getDistances(pg, v, dist);
            W exact = dist[u];
            printf("%d: %lf %lf %lf\n", tt, exact, approx1, approx2);
            
            assert(approx1 <= exact * (1 + eps));
            assert(exact <= approx1);
            assert(approx2 <= exact * (1 + eps));
            assert(exact <= approx2);
        }
    }
*/
    return 0;
}
