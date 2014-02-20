class FullPlanarOracle : public PlanarOracle {
public:
    FullPlanarOracle(
            int n,
            const vector< pair< int, int > >& edges,
            const vector< W >& weights,
            W eps);

    int merge(int l1, int l2, int u, int v);
    pair<int, int> split(int l, int u, int v);
    
    void activate(int l);
    void deactivate(int l);
    
    W distance_to_color(int v, int l);
    W distance_to_closest(int v);
};
