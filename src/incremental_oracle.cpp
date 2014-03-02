#include "incremental_oracle.h"

#include <signal.h>
#include <utility>
using std::min;
using std::max;
using std::make_pair;
using std::swap;

void IncrementalPlanarOracle::processLeaf(
        int i,
        const PlanarGraph& pg,
        const vector<int>& mapping,
        const vector<bool>& source) {
    
    for (int v=0; v<(int)pg.vs().size(); ++v) {
        if (!source[v]) continue;
        if (mapping[v] == -1) continue;
        vertices[mapping[v]].direct_pieces.push_back(i);
    }
}

void IncrementalPlanarOracle::processPortals(
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

            portals.back().N[v] = distances[j];
            if (source[j]) {
                vertices[v].to_portal[portals.size()-1] = distances[j];
            }
            labels[v].L.push_back(make_pair(v, portals.size()-1));
        }
    }
}

void IncrementalPlanarOracle::activate(int v) {
    labels[v].active = true;
    for (auto l: labels[v].L) {
        int p = l.second;
        portals[p].H.insert(make_pair(portals[p].N[v], v));
    }
}

int IncrementalPlanarOracle::merge(int v, int u) {
    if (labels[v].L.size() < labels[u].L.size()) swap(u,v);
    if (labels[v].active) {
        for (auto l: labels[u].L) {
            int p = l.second;

            auto it = portals[p].N.find(u);
            if (it == portals[p].N.end()) continue;
            W du = it->second;
            portals[p].N.erase(it);
            portals[p].H.erase(make_pair(du, u));

            it = portals[p].N.find(v);
            if (it != portals[p].N.end()) {
                W dv = it->second;
                du = min(du, dv);
                portals[p].H.erase(make_pair(dv, v));
            }

            portals[p].N[v] = du;
            portals[p].H.insert(make_pair(du, v));
        }
    } else {
        for (auto l: labels[u].L) {
            int p = l.second;
            auto it = portals[p].N.find(u);
            if (it == portals[p].N.end()) continue;
            W du = it->second;;
            portals[p].N.erase(it);

            it = portals[p].N.find(v);
            if (it != portals[p].N.end()) {
                W dv = it->second;
                du = min(du, dv);
            }

            portals[p].N[v] = du;
        }
    }

    for (auto &l: labels[u].L) l.first = v;
    labels[v].L.insert(labels[v].L.end(), labels[u].L.begin(), labels[u].L.end());
    labels[u].L.clear();

    fu.unionn(v, u);
    return v;
}

W IncrementalPlanarOracle::distance_to_color(int v, int u) {
    if (isOfColor(v, u)) return 0;

    W res = infinity;
    for (auto it: vertices[v].to_portal) {
        int p = it.first;
        auto itt = portals[p].N.find(u);
        if (itt == portals[p].N.end()) continue;
        res = min(res, it.second + itt->second);
    }
    for (auto it: vertices[v].direct_pieces) {
        vector<int> mapping = pieces[it].mapping;
        int s = -1;
        for (int i=0; i<(int)mapping.size(); ++i) if (mapping[i] == v) s = i;

        vector<W> distances;
        getDistances(pieces[it].graph, s, distances);
        for (int i=0; i<(int)mapping.size(); ++i) {
            if (mapping[i] == -1) continue;
            if (isOfColor(mapping[i], u)) {
                res = min(res, distances[i]);
            }
        }
    }
    return res;
}

W IncrementalPlanarOracle::distance_to_closest(int v) {
    if (isActive(v)) return 0;

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
