#include <utility>
#include <vector>
#include <string>
#include <cstdio>
#include <iostream>
#include <omp.h>
#include "src/main.hxx"

using namespace std;




template <class G>
void adjustOptions(const G& x, int repeat) {
  using K = typename G::key_type;
  int maxThreads  = 12;
  int iterations  = 10;
  float inflation = 1.5f;
  float conditionalUpdate = 0.5f;
  omp_set_num_threads(maxThreads);
  printf("OMP_NUM_THREADS=%d\n", maxThreads);
  auto M = edgeWeight(x)/2;
  auto Q = modularity(x, M, 1.0f);
  printf("[%01.6f modularity] noop\n", Q);
  do {
    LabelrankResult<K> a = labelrankSeq<4>(x, {repeat, iterations, inflation, conditionalUpdate});
    auto fc = [&](auto u) { return a.membership[u]; };
    auto Q  = modularity(x, fc, M, 1.0f);
    printf("[%09.3f ms; %01.6f modularity] labelrankSeq\n", a.time, Q);
  } while (false);
  for (int chunkSize=1; chunkSize<=65536; chunkSize*=2) {
    omp_set_schedule(omp_sched_static, chunkSize);
    LabelrankResult<K> a = labelrankOmp<4>(x, {repeat, iterations, inflation, conditionalUpdate});
    auto fc = [&](auto u) { return a.membership[u]; };
    auto Q  = modularity(x, fc, M, 1.0f);
    printf("[%09.3f ms; %01.6f modularity] labelrankOmp {sch_kind: static, chunk_size: %d}\n", a.time, Q, chunkSize);
  }
  for (int chunkSize=1; chunkSize<=65536; chunkSize*=2) {
    omp_set_schedule(omp_sched_dynamic, chunkSize);
    LabelrankResult<K> a = labelrankOmp<4>(x, {repeat, iterations, inflation, conditionalUpdate});
    auto fc = [&](auto u) { return a.membership[u]; };
    auto Q  = modularity(x, fc, M, 1.0f);
    printf("[%09.3f ms; %01.6f modularity] labelrankOmp {sch_kind: dynamic, chunk_size: %d}\n", a.time, Q, chunkSize);
  }
  for (int chunkSize=1; chunkSize<=65536; chunkSize*=2) {
    omp_set_schedule(omp_sched_guided, chunkSize);
    LabelrankResult<K> a = labelrankOmp<4>(x, {repeat, iterations, inflation, conditionalUpdate});
    auto fc = [&](auto u) { return a.membership[u]; };
    auto Q  = modularity(x, fc, M, 1.0f);
    printf("[%09.3f ms; %01.6f modularity] labelrankOmp {sch_kind: guided, chunk_size: %d}\n", a.time, Q, chunkSize);
  }
  for (int chunkSize=1; chunkSize<=65536; chunkSize*=2) {
    omp_set_schedule(omp_sched_auto, chunkSize);
    LabelrankResult<K> a = labelrankOmp<4>(x, {repeat, iterations, inflation, conditionalUpdate});
    auto fc = [&](auto u) { return a.membership[u]; };
    auto Q  = modularity(x, fc, M, 1.0f);
    printf("[%09.3f ms; %01.6f modularity] labelrankOmp {sch_kind: auto, chunk_size: %d}\n", a.time, Q, chunkSize);
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
