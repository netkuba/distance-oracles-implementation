#include "oracle.h"

#include <algorithm>
using std::min;
using std::max;
using std::sort;

void
PlanarOracle::initialize(
        int n,
        const vector< pair< int, int > >& edge, 
        const vector< W >& weight,
        W eps) {
    ro = max(min(n, (int)sqrt((float)n)), 3);
    graph = PlanarGraph(n);
    vertices = vector<Vertex>(n);

    for (int i=0; i<(int)edge.size(); ++i) {
        graph.add_edge(edge[i].first, edge[i].second, weight[i]);
    }
    embed(graph);

    
    double minD = infinity, maxD = 0;
    {
        vector<W> distance;
        getDistances(graph, 0, distance);
        for (int i=1; i<(int)distance.size(); ++i) {
            maxD = max(maxD, distance[i]);
        }
        maxD *= 2;
        for (int i=0; i<(int)graph.es().size(); ++i) {
            minD = min(minD, graph.es()[i].w);
        }
    }

    vector< W > preAlpha;

    {
        vector< vector<int> > parents;
        vector< vector<bool> > sources;
        W alpha = minD;

        while (alpha <= maxD) {
            vector< vector<int> > tmpParents, tmpMappings;
            vector< vector<bool> > tmpSources;
            vector< PlanarGraph > tmpSubgs;
            getAlphaFamily(graph, alpha, tmpSubgs, tmpMappings, tmpParents, tmpSources);

            assert(tmpSources.size() == tmpMappings.size());
            
            for (int i=0; i<(int)tmpSubgs.size(); ++i) {

                for (int j=1; j<(int)tmpMappings[i].size(); ++j) {
                    assert(tmpMappings[i][j] != -1);
                }

                pieces.push_back(Piece(
                            tmpSubgs[i], tmpMappings[i]));
                parents.push_back(tmpParents[i]);
                sources.push_back(tmpSources[i]);
                preAlpha.push_back(alpha);
            }

            alpha *= 2;
        }

        printf("RO! %d\n", ro);

        for (int i=0; i<(int)pieces.size(); ++i) {


            PlanarGraph pg = pieces[i].graph;
            vector<int> mapping = pieces[i].mapping;
            alpha = preAlpha[i];

            if ((int)pg.vs().size() <= ro) {
                processLeaf(i, pg, mapping, sources[i]);
                continue;
            }

            vector< vector<int> > tmpParents, tmpMappings;
            vector< PlanarGraph > tmpSubgs;
            vector< vector< pair<int, int> > > tmpPaths;
            vector<int> newPortals;
            
            subdivide(pg, parents[i], tmpSubgs, tmpMappings, 
                    tmpParents, tmpPaths);
            
            for (int j=0; j<(int)tmpSubgs.size(); ++j) {
                vector<bool> tmpSources;
                for (int k=0; k<(int)tmpMappings[j].size(); ++k) {
                    if (tmpMappings[j][k] == -1) {
                        tmpSources.push_back(false);
                        continue;
                    }
                    tmpSources.push_back(sources[i][tmpMappings[j][k]]);
                    tmpMappings[j][k] = mapping[tmpMappings[j][k]];
                }
                pieces.push_back(Piece(tmpSubgs[j], tmpMappings[j]));
                parents.push_back(tmpParents[j]);
                sources.push_back(tmpSources);
                preAlpha.push_back(alpha);
            }

            assert(tmpPaths.size() <= 3);

            for (int j=0; j<(int)tmpPaths.size(); ++j) {
                pair<int, int> prevV(-1, -1);
                W dist = infinity;
                for (int k=0; k<(int)tmpPaths[j].size()-1; ++k) {
                    int v = tmpPaths[j][k].first;
                    W w = pg.es()[tmpPaths[j][k].second].w;

                    if (dist + w > alpha*eps/2) {
                        newPortals.push_back(v);
                        dist = 0;
                    }

                    dist += w;
                }
                /*
                for (auto v: tmpPaths[j]) {
                    if (dist > alpha*eps/4) {
                        newPortals.push_back(prevV.first);
                        dist = pg.es()[prevV.second].w;
                    }
                    dist += pg.es()[v.second].w;
                    prevV = v;
                }
                */
            }

            sort(newPortals.begin(), newPortals.end());
            auto it = unique(newPortals.begin(), newPortals.end());
            newPortals.resize(std::distance(newPortals.begin(), it));

            assert(newPortals.size() <= 3 * 3 * (int)(1/eps + 1) * 4);

            processPortals(i, pg, mapping, newPortals, sources[i]);
        }
    }
}
