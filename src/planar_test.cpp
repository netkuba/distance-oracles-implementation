#include "planar.h"
#include "oracle.h"

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
//    for (int n = 1; n < 100; ++n)
    int n = 100;
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

        PlanarOracle oracle(n*n, edge, weight, eps);

        int sum = 0;
        for (int i=0; i<(int)oracle.vertices.size(); ++i) {
            sum += oracle.vertices[i].to_portal.size();
        }

        printf("%d / %d = %d\n", sum, n*n, sum / (n*n));

        clock_t begin = clock();

        for (int tt=0; tt<t; ++tt) {
            int u = rand()%(n*n), v = rand()%(n*n);
            if (u == v) continue;
            W exact = oracle.exact_to_vertex(u, v);
            W approx = oracle.distance_to_color(u, v);
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
