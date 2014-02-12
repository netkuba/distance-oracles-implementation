#include "planar.h"
#include "oracle_internal.h"

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

const int n = 10;
const int m = 5;

int main() {
    PlanarGraph pg(n*n);
    

    for (int i=0; i<n-1; ++i)
        for (int j=0; j<n; ++j)
            pg.add_edge(i*n+j, (i+1)*n+j, rand()%m);

    for (int i=0; i<n; ++i)
        for (int j=0; j<n-1; ++j)
            pg.add_edge(i*n+j, i*n+j+1, rand()%m);

    embed(pg);
    printEmbedded(pg);

    vector< PlanarGraph > resPG, rresPG;
    vector< vector<int> > resMappings, rresMappings;
    vector< vector<int> > resParents, rresParents;
    vector< vector< pair< int, int > > > paths;

    getAlphaFamily(pg, m*n*2, resPG, resMappings, resParents);
    for (int i=0; i<(int)resPG.size(); ++i) {
        subdivide(resPG[i], resParents[i], rresPG, rresMappings, rresParents, paths);  
    }

    

    return 0;
}
