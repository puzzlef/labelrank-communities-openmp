#pragma once
#include <utility>
#include <cmath>
#include <algorithm>
#include <array>
#include <vector>

using std::pair;
using std::array;
using std::vector;
using std::make_pair;
using std::min;
using std::pow;
using std::sort;




// Each vector has a set of labels, which is a fixed array of label ids and
// associated probabilities. This implies, we allow a maximum of, say, 4
// labels per vertex. This is done in order to minimize computation. The most
// probable label is stored first.
template <class K, class V, size_t L>
using Labelset = array<pair<K, V>, L>;




// Labelset for combining multiple labelsets.
template <class K, class V>
struct AccumulatorLabelset {
  vector<V> data;
  vector<K> keys;

  // Lifetime operations.
  AccumulatorLabelset(size_t capacity=0): data(capacity) {}

  // Read operations.
  inline size_t size() const {
    return keys.size();
  }

  // Access operations.
  template <class F>
  inline void forEach(F fn) const {
    for (K k : keys)
      fn(k, data[k]);
  }
  template <class F>
  inline void forEach(F fn) {
    for (K k : keys)
      fn(k, data[k]);
  }
  inline K keyAt(size_t i) const {
    return keys[i];
  }
  inline auto getAt(size_t i) const {
    K k = keyAt(i);
    return make_pair(k, data[k]);
  }
  inline void setAt(size_t i, V v) {
    K k = keyAt(i);
    data[k] = v;
  }
  inline void accumulateAt(size_t i, V v) {
    K k = keyAt(i);
    data[k] += v;
  }
  inline bool has(K k) const {
    return data[k]!=V();
  }
  inline V get(K k) const {
    return data[k];
  }
  inline void set(K k, V v) {
    if (k>=data.size()) data.resize(k+1);
    if (!data[k]) keys.push_back(k);
    data[k] = v;
  }
  inline void accumulate(K k, V v) {
    if (k>=data.size()) data.resize(k+1);
    if (!data[k]) keys.push_back(k);
    data[k] += v;
  }

  // Update operations.
  inline void clear() {
    for (K k : keys)
      data[k] = V();
    keys.clear();
  }
  template <class F>
  inline void reorder(F fn) {
    sort(keys.begin(), keys.end(), fn);
  }
};
template <class K, class V>
using ALabelset = AccumulatorLabelset<K, V>;




/**
 * Get size of accumulator labelset.
 * @param x accumulator labelset
 */
template <class K, class V>
inline size_t labelsetSize(const ALabelset<K, V>& x) {
  return x.size();
}
/**
 * Get size of labelset.
 * @param x labelset
 */
template <class K, class V, size_t N>
inline size_t labelsetSize(const Labelset<K, V, N>& x) {
  size_t a = 0;
  for (const auto& [k, v] : x)
    if (k) ++a;
  return a;
}


/**
 * Check if an accumulator labelset has a key.
 * @param x accumulator labelset
 * @param k key to check
 */
template <class K, class V>
inline bool labelsetHas(const ALabelset<K, V>& x, K k) {
  return x.has(k);
}
/**
 * Check if a labelset has a key.
 * @param x labelset
 * @param k key to check
 */
template <class K, class V, size_t N>
inline bool labelsetHas(const Labelset<K, V, N>& x, K k) {
  for (const auto& [_k, v] : x)
    if (_k==k) return true;
  return false;
}


/**
 * Get sum of values in accumulator labelset.
 * @param x accumulator labelset
 */
template <class K, class V>
inline V labelsetSum(const ALabelset<K, V>& x) {
  V a = V();
  x.forEach([&](auto k, auto v) { a += v; });
  return a;
}
/**
 * Get sum of values in labelset.
 * @param x labelset
 */
template <class K, class V, size_t N>
inline V labelsetSum(const Labelset<K, V, N>& x) {
  V a = V();
  for (const auto& [k, v] : x)
    a += v;
  return a;
}


/**
 * Combine a labelset to an accumulator labelset with given weight.
 * @param a accumulator labelset
 * @param x labelset to combine
 * @param w combining weight
 */
template <class K, class V, size_t L>
inline void labelsetCombineU(ALabelset<K, V>& a, const Labelset<K, V, L>& x, V w) {
  for (auto [k, v] : x)
    if (k) a.accumulate(k, w*v);
}


/**
 * Copy to a labelset from accumulator labelset.
 * @param a target labelset
 * @param x accumulator labelset
 */
template <class K, class V, size_t L>
inline void labelsetCopyW(Labelset<K, V, L>& a, const ALabelset<K, V>& x) {
  size_t M = min(L, x.size());
  for (size_t i=0; i<M; ++i)
    a[i] = x.getAt(i);
  for (size_t i=M; i<L; ++i)
    a[i] = make_pair(K(), V());
}


/**
 * Multiply a value to probabilities in accumulator labelset (probability scaling).
 * @param a accumulator labelset
 * @param m value to multiply
 */
template <class K, class V>
inline void labelsetMultiplyU(ALabelset<K, V>& a, V m) {
  a.forEach([&](auto k, auto& v) { v *= m; });
}
/**
 * Multiply a value to probabilities in labelset (probability scaling).
 * @param a target labelset
 * @param m value to multiply
 */
template <class K, class V, size_t N>
inline void labelsetMultiplyU(Labelset<K, V, N>& a, V m) {
  for (auto& [k, v] : a)
    v *= m;
}


/**
 * Raise each probability with a given exponent in accumulator labelset (inflation operator).
 * @param a accumulator labelset
 * @param e exponent value
 */
template <class K, class V>
inline void labelsetPowU(ALabelset<K, V>& a, V e) {
  a.forEach([&](auto k, auto& v) { v = pow(v, e); });
}
/**
 * Raise each probability with a given exponent in labelset (inflation operator).
 * @param a target labelset
 * @param e exponent value
 */
template <class K, class V, size_t N>
inline void labelsetPowU(Labelset<K, V, N>& a, V e) {
  for (auto& [k, v] : a)
    v = pow(v, e);
}


/**
 * Multiply a value and raise each probability with a given exponent in accumulator labelset (probability scaling + inflation operator).
 * @param a accumulator labelset
 * @param m value to multiply
 * @param e exponent value
 */
template <class K, class V>
inline void labelsetMultiplyPowU(ALabelset<K, V>& a, V m, V e) {
  a.forEach([&](auto k, auto& v) { v = pow(v*m, e); });
}
/**
 * Multiply a value and raise each probability with a given exponent in labelset (probability scaling + inflation operator).
 * @param a target labelset
 * @param m value to multiply
 * @param e exponent value
 */
template <class K, class V, size_t N>
inline void labelsetMultiplyPowU(Labelset<K, V, N>& a, V m, V e) {
  for (auto& [k, v] : a)
    v = pow(v*m, e);
}


/**
 * Reorder accumulator set placing higher probability labels first.
 * @param a accumulator labelset
 */
template <class K, class V>
inline void labelsetReorderU(ALabelset<K, V>& a) {
  a.reorder([&](auto k, auto l) { return a.get(k)>a.get(l); });
}


/**
 * Count the number of matching keys between labelsets.
 * @param x a labelset
 * @param y another labelset
 */
template <class K, class V, size_t N>
inline size_t labelsetMatchCount(const Labelset<K, V, N>& x, const Labelset<K, V, N>& y) {
  size_t a = 0;
  for (const auto& [k, v] : x)
    if (k && labelsetHas(y, k)) ++a;
  return a;
}


/**
 * Sum the values of matching keys between labelsets.
 * @param x a labelset
 * @param y another labelset
 */
template <class K, class V, size_t N>
inline V labelsetMatchValue(const Labelset<K, V, N>& x, const Labelset<K, V, N>& y) {
  V a = V();
  for (const auto& [k, v] : x)
    if (k && labelsetHas(y, k)) a += v;
  return a;
}


/**
 * Check if first labelset is subset of second.
 * @param x a labelset
 * @param y another labelset
 */
template <class K, class V, size_t N>
inline bool labelsetIsSubset(const Labelset<K, V, N>& x, const Labelset<K, V, N>& y) {
  for (const auto& [k, v] : x)
    if (k && !labelsetHas(y, k)) return false;
  return true;
}
