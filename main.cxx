#include <utility>
#include <vector>
#include <string>
#include <cstdio>
#include <iostream>
#include "src/main.hxx"

using namespace std;




template <class G>
void adjustOptions(const G& x, int repeat) {
  using K = typename G::key_type;
  int iterations = 10;
  auto M = edgeWeight(x)/2;
  auto Q = modularity(x, M, 1.0f);
  printf("[%01.6f modularity] noop\n", Q);
  for (float conditionalUpdate=1.0f; conditionalUpdate>0.0f; conditionalUpdate-=0.1f) {
    for (float inflation=2.0f; inflation>=1.0f; inflation-=0.1f) {
      LabelrankResult<K> a = labelrankSeq<4>(x, {repeat, iterations, inflation, conditionalUpdate});
      auto fc = [&](auto u) { return a.membership[u]; };
      auto Q  = modularity(x, fc, M, 1.0f);
      printf("[%09.3f ms; %01.6f modularity] labelrankSeq {inflation: %01.1f, cond_update: %01.1f}\n", a.time, Q, inflation, conditionalUpdate);
    }
  }
}


int main(int argc, char **argv) {
  using K = int;
  using V = float;
  char *file = argv[1];
  int repeat = argc>2? stoi(argv[2]) : 5;
  OutDiGraph<K, None, V> x; V w = 1;
  printf("Loading graph %s ...\n", file);
  readMtxW(x, file); println(x);
  auto y  = symmetricize(x); print(y); printf(" (symmetricize)\n");
  auto fl = [](auto u) { return true; };
  selfLoopU(y, w, fl); print(y); printf(" (selfLoopAllVertices)\n");
  adjustOptions(y, repeat);
  printf("\n");
  return 0;
}
