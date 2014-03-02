#include "full_oracle.h"

#include <utility>
using std::min;

void FullPlanarOracle::processLeaf(
        int i,
        const PlanarGraph& pg,
        const vector<int>& mapping,
        const vector<bool>& source) {
    for (int v=0; v<(int)pg.vs().size(); ++v) {
        if (!source[v]) continue;
        vertices[mapping[v]].direct_pieces.push_back(i);
    }
}

void FullPlanarOracle::processPortals(
        int i,
        const PlanarGraph& pg,
        const vector<int>& mapping,
        const vector<int>& newPortals,
        const vector<bool>& source) {
    vector<W> distances;
    for (int p: newPortals) {
        getDistances(pg, p, distances);
        portals.push_back(Portal(i, p));
        for (int j=0; j<(int)mapping.size(); ++j) {
            if (mapping[j] == -1) continue;
            int v = mapping[j];

            if (source[j]) {
                vertices[v].to_portal[portals.size()-1] = distances[j];
            }
            forest.vertex_value(v).L.push_back(
                    make_pair(portals.size()-1, distances[j]));
        }
    }
}

void FullPlanarOracle::activate(int l) {
    labels[l].active = true;
    for (auto lp: forest.tree_value(l).L) {
        int p = lp.first;
        W dist = lp.second;
        if (dist != infinity)
            portals[p].H.insert(make_pair(dist, l));
    }
}

void FullPlanarOracle::deactivate(int l) {
    labels[l].active = false;
    for (auto lp: forest.tree_value(l).L) {
        int p = lp.first;
        W dist = lp.second;
        if (dist != infinity)
            portals[p].H.erase(make_pair(dist, l));
    }
}

int FullPlanarOracle::merge(int l1, int l2, int u, int v) {
    bool active = labels[v].active;
    assert(labels[u].active == active);

    if (active) {
        deactivate(l1);
        deactivate(l2);
    }
    int l = forest.merge(l1,l2,u,v);
    if (active) {
        activate(l);
    }
    return l;
}

pair<int, int> FullPlanarOracle::split(int l, int u, int v) {
    bool active = labels[v].active;
    if (active) {
        deactivate(l);
    }
    pair<int, int> res = forest.split(l, u, v);
    if (active) {
        activate(res.first);
        activate(res.second);
    }
    return res;
}


W FullPlanarOracle::distance_to_color(int v, int l) {
    W res = infinity;
    for (auto it: vertices[v].to_portal) {
        int p = it.first;
        W dist = forest.tree_value(l).getDistance(p);
        res = min(res, it.second + dist);
    }
    for (auto it: vertices[v].direct_pieces) {
        vector<int> mapping = pieces[it].mapping;
        int s = -1;
        for (int i=0; i<(int)mapping.size(); ++i) if (mapping[i] == v) s = i;

        vector<W> distances;
        getDistances(pieces[it].graph, s, distances);
        for (int i=0; i<(int)mapping.size(); ++i) {
            if (mapping[i] == -1) continue;
            if (isOfColor(mapping[i], l)) {
                res = min(res, distances[i]);
            }
        }
    }
    return res;
}

W FullPlanarOracle::distance_to_closest(int v) {
    W res = infinity;
    for (auto it: vertices[v].to_portal) {
        int p = it.first;
        if (portals[p].H.empty()) continue;
        res = min(res, it.second + portals[p].H.begin()->first);
    }
    for (auto it: vertices[v].direct_pieces) {
        vector<int> mapping = pieces[it].mapping;
        int s = -1;
        for (int i=0; i<(int)mapping.size(); ++i) if (mapping[i] == v) s = i;

        vector<W> distances;
        getDistances(pieces[it].graph, s, distances);
        for (int i=0; i<(int)mapping.size(); ++i) {
            if (mapping[i] == -1) continue;
            if (isActive(mapping[i])) {
                res = min(res, distances[i]);
            }
        }
    }
    return res;
}
