#include "planar.h"

#include <cstdio>
using namespace std;

const int n = 3;

int main() {
    PlanarGraph pg(n*n);
    

    for (int i=0; i<n-1; ++i)
        for (int j=0; j<n; ++j)
            pg.add_edge(i*n+j, (i+1)*n+j);

    for (int i=0; i<n; ++i)
        for (int j=0; j<n-1; ++j)
            pg.add_edge(i*n+j, i*n+j+1);

    embed(pg);
    triangulate(pg);

    for (int v=0; v<(int)pg.vs().size(); ++v) {
        printf("Vertex %d:\n", v);
        if (pg.vs()[v].edges.empty()) continue;
        int e_end = pg.vs()[v].edges[0], e = e_end;
        do {
            printf("%d - %d\n", pg.es()[e].u, pg.es()[e].v);
            e = pg.eNext(v, e);
        } while (e != e_end);
    }

    return 0;
}
