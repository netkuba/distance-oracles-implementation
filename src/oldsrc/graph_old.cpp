#include <boost/graph/adjacency_list.hpp>
#include <boost/property_map/property_map.hpp>
#include <boost/graph/boyer_myrvold_planar_test.hpp>
#include <cstdio>
#include <utility>

typedef boost::adjacency_list< boost::vecS, boost::vecS, boost::undirectedS, boost::property<boost::vertex_index_t, int> > Graph;

typedef boost::graph_traits<Graph>::vertex_descriptor vertex_descriptor;
typedef boost::graph_traits<Graph>::edge_descriptor edge_descriptor;

typedef std::vector< std::vector< boost::graph_traits<Graph>::edge_descriptor > >  planar_embedding_storage_t;
typedef boost::iterator_property_map< planar_embedding_storage_t::iterator, boost::property_map<Graph, boost::vertex_index_t>::type > planar_embedding_t;

typedef std::vector< boost::graph_traits<Graph>::edge_descriptor > kuratowski_edges_t;

typedef std::pair<int, int> E;

int main() {
    E edges[] = { E(0,1), E(1,2), E(2,3), E(3,0), E(0,2), E(1,3) };
    int weight[] = { 1, 2, 3, 4, 5, 6 };

    int n = sizeof(weight) / sizeof(int);
    int m = sizeof(edges) / sizeof(E);
    
    Graph g(n);
    for (int i=0; i<m; ++i) add_edge(edges[i].first, edges[i].second, g);

    planar_embedding_t embedding_pmap;
    kuratowski_edges_t out_itr; 

    if (boyer_myrvold_planarity_test(boost::boyer_myrvold_params::graph = g, 
                boost::boyer_myrvold_params::embedding = embedding_pmap,  
                boost::boyer_myrvold_params::kuratowski_subgraph = out_itr
                )
       ) {
        printf("TAK!\n");
    } else {
        printf("NIE.\n");
    }    
    
    return 0;
}
