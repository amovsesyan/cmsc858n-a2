#ifndef _HOTSPOTS_H_
#define _HOTSPOTS_H_

#include <atomic>
#include "parlay/primitives.h"
#include "parlay/sequence.h"
#include "parlay/hash_table.h"

template <typename vertex, typename graph>
auto hotspots(parlay::sequence<vertex> U, const graph& G) {
	vertex n = G.size();

	// n indicates that the vertex has not been visited yet
	auto nearest = parlay::tabulate<std::atomic<vertex>>(n, [&] (long i) {return n;});

	// mark the set U as visited from self
	parlay::parallel_for(0, U.size(), [&] (long i) {
			nearest[U[i]] = U[i];
    });
	// the general algorithm for this
	// 	frontier: set = U
	// 	next_frontier: set = {}
	// while frontier.not_empty
	//		next_frontier = {}
	// 		for v in frontier
	//
	// 			for n in v.neighbours
	//				if n is not visited
	// 					nearest[n] = nearest[v];
	// 					next_frontier.add(n);
	// 		frontier = next_frontier
	//		next_frontier = {}

	// for the purpose of this project, I'll use parlay::hash_map in lieu of a set

	// initialize sets
	// graph is defined as a sequence of sequence of ints
	// so in the frontiers we'll store the vertices
	parlay::sequence<vertex>* frontier = new parlay::sequence<vertex>();
	parlay::sequence<vertex>* next_frontier = new parlay::sequence<vertex>();

	parlay::sequence<bool> visited(G.size(), false);

	// add U as first frontier
	parlay::parallel_for(0, U.size(), [&] (long i) {
		frontier->push_back(U[i]);
	});

	while (frontier->size() > 0) {

 		for(long i = 0; i < frontier->size(); i++) {
			// get sequence of vertices representing the neighbour of the current vertex
			vertex current_vertex = frontier[0][i];
			auto neighbours = G[current_vertex];
			// get hotspot
			vertex nearest_hotspot = nearest[current_vertex];
			parlay::parallel_for(0, neighbours.size(), [&] (long j) {
				vertex neighbour = neighbours[j];
				if(!visited[neighbour]) {
					visited[neighbour] = true;
					nearest[neighbour] = nearest_hotspot;
					next_frontier->push_back(neighbour);
				}

			});
		}

		// swap next frontier and frontier
		auto tmp = next_frontier;
		next_frontier = frontier;
		frontier = tmp;

		// reset next frontier
		next_frontier->clear();
	}

	// cleanup -- delete frontier
	delete frontier;
	delete next_frontier;
//	delete visited;


	// to be filled in
	// std::cout << "unimplemented" << std::endl;



	// convert from sequence of atomics to a regular sequence
	return parlay::tabulate(n, [&] (long i) {return nearest[i].load();});
}

#endif
