/* mst.hpp - compute maximal spanning tree from graph.
 *
 * Adapted from mst.jl from https://github.com/seung-lab/Watershed.jl
 */
#pragma once
#include "types.hpp"
#include <zi/disjoint_sets/disjoint_sets.hpp>
#include <deque>
#include <vector>
#include <set>
#include <iostream>
/*
 * Given a region graph, sorted from maximal affinity to minimal,
 * return the maximum spanning tree of that graph as a region graph
 *
 * Template arguments:
 *   ID - the type of the segment IDs
 *   F - the type of the affinity weighting
 *
 * Parameters:
 * rg_ptr - a pointer to the input graph
 * count - the number of IDs (>= the maximum ID in the region graph + 1)
 *
 */
 
template< typename ID, typename F> region_graph_ptr<ID, F> 
    mst(const region_graph_ptr<ID, F> rg_ptr, size_t count) {
    region_graph_ptr<ID, F> new_rg_ptr(new region_graph<ID, F>);
    std::vector<std::set<uint32_t>> adjacency(count);
    zi::disjoint_sets<ID> sets(count);
    std::cout << "mst: processing " << count << " ids " << std::endl 
              << "Creating minimum spanning tree" << std::endl << std::flush;
    /*
     * Create the minimum spanning tree
     */
    for (auto it: *rg_ptr) {
      ID v1 = std::get<1>(it);
      ID v2 = std::get<2>(it);
      ID s1 = sets.find_set(v1);
      ID s2 = sets.find_set(v2);
      if (s1 != s2) {
        F aff=std::get<0>(it);
        new_rg_ptr->emplace_back(aff, s1, s2);
        sets.join(s1, s2);
        adjacency[s1].insert(s2);
        adjacency[s2].insert(s1);
      }
    }
    std::cout << "Relabeling nodes" << std::endl << std::flush;
    /*
     * Relabel nodes.
     */
    std::vector<ID> order(count);
    ID curr = 1;
    for (size_t i=1; i<count; i++) {
      if (order[i] == 0) {
        std::deque<ID> bfs;
        bfs.push_back(i);
        order[i] = curr++;
        while (bfs.size() > 0) {
          ID x = bfs.front();
          bfs.pop_front();
          for (auto y:adjacency[x]) {
            if (order[y] == 0) {
              order[y] = curr++;
              bfs.push_back(y);
            }
          }
        }
      }
    }
    std::cout << "Swapping parents and children" << std::endl << std::flush;
    for (auto e:*new_rg_ptr) {
      if (order[std::get<1>(e)] > order[std::get<2>(e)]) {
        std::swap(std::get<1>(e), std::get<2>(e));
      }
    }
    return new_rg_ptr;
}