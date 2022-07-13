#pragma once
#include <utility>
#include <cmath>
#include <array>
#include "_main.hxx"
#include "Labelset.hxx"
#include "labelrank.hxx"

using std::pair;
using std::array;
using std::pow;




/**
 * Initialize labelset for a given vertex.
 * @param a accumulator labelset (scratch)
 * @param as target labelsets
 * @param x original graph
 * @param u given vertex
 * @param e exponent value
 */
template <class G, class K, class V, size_t N>
void labelrankInitializeVertexW(ALabelset<K, V>& a, vector<Labelset<K, V, N>>& as, const G& x, K u, V e) {
  V sumw = V(); a.clear();
  x.forEachEdge(u, [&](auto v, auto w) {
    a.set(v, w);
    sumw += w;
  });
  labelsetReorderU(a);
  labelsetCopyW(as[u], a);
  labelsetMultiplyPowU(as[u], 1/sumw, e);
}


/**
 * Update labelset for a given vertex.
 * @param a accumulator labelset (scratch)
 * @param as target labelsets
 * @param ls original labelsets
 * @param x original graph
 * @param u given vertex
 * @param e exponent value
 */
template <class G, class K, class V, size_t N>
void labelrankUpdateVertexW(ALabelset<K, V>& a, vector<Labelset<K, V, N>>& as, const vector<Labelset<K, V, N>>& ls, const G& x, K u, V e) {
  V sumw = V(); a.clear();
  x.forEachEdge(u, [&](auto v, auto w) {
    labelsetCombineU(a, ls[v], w);
    sumw += w;
  });
  labelsetReorderU(a);
  labelsetCopyW(as[u], a);
  labelsetMultiplyPowU(as[u], 1/sumw, e);
}


/**
 * Check if a vertex is stable.
 * @param ls labelsets
 * @param x original graph
 * @param u given vertex
 * @param q conditional update parameter
 */
template <class G, class K, class V, size_t N>
bool labelrankIsVertexStable(const vector<Labelset<K, V, N>>& ls, const G& x, K u, V q) {
  K count = K();
  x.forEachEdgeKey(u, [&](auto v) {
    if (labelsetIsSubset(ls[u], ls[v])) count++;
  });
  return count > q * x.degree(u);
}


/**
 * Get best label for each vertex in graph.
 * @param ls labelsets
 * @param x original graph
 */
template <class G, class K, class V, size_t N>
auto labelrankBestLabels(const vector<Labelset<K, V, N>>& ls, const G& x) {
  vector<K> a(x.span());
  x.forEachVertexKey([&](auto u) {
    a[u] = ls[u][0].first;
  });
  return a;
}




/**
 * Detect community membership of each vertex in a graph using LabelRank algorithm.
 * @param x original graph
 * @param o labelrank options
 */
template <size_t N, class G>
auto labelrankSeq(const G& x, const LabelrankOptions& o={}) {
  using K = typename G::key_type;
  using V = typename G::edge_value_type;
  ALabelset<K, V> la(x.span());
  vector<Labelset<K, V, N>> ls(x.span());
  vector<Labelset<K, V, N>> ms(x.span());
  float t = measureDurationMarked([&](auto mark) {
    la.clear();
    ls.clear(); ls.resize(x.span());
    ms.clear(); ms.resize(x.span());
    mark([&]() {
      x.forEachVertexKey([&](auto u) {
        labelrankInitializeVertexW(la, ls, x, u, V(o.inflation));
      });
      for (int i=0; i<o.maxIterations; ++i) {
        x.forEachVertexKey([&](auto u) {
          if (labelrankIsVertexStable(ls, x, u, V(o.conditionalUpdate))) ms[u] = ls[u];
          else labelrankUpdateVertexW(la, ms, ls, x, u, V(o.inflation));
        });
        swap(ls, ms);
      }
    });
  }, o.repeat);
  return LabelrankResult(labelrankBestLabels(ls, x), o.maxIterations, t);
}
