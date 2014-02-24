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
            minD = min(minD, distance[i]);
            maxD = max(maxD, distance[i]);
        }
        maxD *= 2;
    }

    vector< W > preAlpha;

    {
        vector< vector<int> > parents;
        vector< vector<bool> > sources;
        W alpha = minD;
        while (alpha <= maxD) {
            vector< vector<int> > tmpParents, tmpMappings;
            vector< vector<bool> > tmpFocus;
            vector< PlanarGraph > tmpSubgs;
            getAlphaFamily(graph, alpha, tmpSubgs, tmpMappings, tmpParents, tmpFocus);

            for (int i=0; i<(int)tmpSubgs.size(); ++i) {

                for (int j=1; j<(int)tmpMappings[i].size(); ++j) {
                    assert(tmpMappings[i][j] != -1);
                }

                pieces.push_back(Piece(
                            tmpSubgs[i], tmpMappings[i]));
                parents.push_back(tmpParents[i]);
                sources.push_back(tmpFocus[i]);
                preAlpha.push_back(alpha);
            }

            alpha *= 2;
        }

        for (int i=0; i<(int)pieces.size(); ++i) {


            PlanarGraph pg = pieces[i].graph;
            vector<int> mapping = pieces[i].mapping;
            alpha = preAlpha[i];

            if (pg.vs().size() <= ro) {
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
                vector<bool> tmpFocus;
                for (int k=0; k<(int)tmpMappings[j].size(); ++k) {
                    if (tmpMappings[j][k] == -1) {
                        tmpFocus.push_back(false);
                        continue;
                    }
                    tmpFocus.push_back(sources[i][tmpMappings[j][k]]);
                    tmpMappings[j][k] = mapping[tmpMappings[j][k]];
                }
                pieces.push_back(Piece(tmpSubgs[j], tmpMappings[j]));
                parents.push_back(tmpParents[j]);
                sources.push_back(tmpFocus);
                preAlpha.push_back(alpha);
            }
            for (int j=0; j<(int)tmpPaths.size(); ++j) {
                pair<int, int> prevV(-1, -1);
                W dist = 0;
                for (auto v: tmpPaths[j]) {
                    if (dist > alpha*eps/4) {
                        newPortals.push_back(prevV.first);
                        dist = pg.es()[prevV.second].w;
                    }
                    dist += pg.es()[v.second].w;
                    prevV = v;
                }
            }

            sort(newPortals.begin(), newPortals.end());
            auto it = unique(newPortals.begin(), newPortals.end());
            newPortals.resize(std::distance(newPortals.begin(), it));

            processPortals(i, pg, mapping, newPortals, sources[i]);
        }
    }
}
