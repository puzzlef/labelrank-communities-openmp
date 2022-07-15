Effect of adjusting schedule in OpenMP-based LabelRank algorithm for community
detection.

[LabelRank] is an algorithm for **detecting communities in graphs**. *Community*
*detection* helps us understand the *natural divisions in a network* in an
**unsupervised manner**. It is used in **e-commerce** for *customer*
*segmentation* and *advertising*, in **communication networks** for *multicast*
*routing* and setting up of *mobile networks*, and in **healthcare** for
*epidemic causality*, setting up *health programmes*, and *fraud detection* is
hospitals. *Community detection* is an **NP-hard** problem, but heuristics exist
to solve it (such as this). *LabelRank* is an **iterative algorithm** that is
based on the concept of *propagation of weighted labels* on a *weighted*
*(directed) network*, where the highest weight label determines the *community*
*membership* of each vertex.

Our implementation of LabelRank **differs** from the original algorithm in that
there is a *fixed upper limit* on the *number of labels per vertex* (**labelset**
**capacity**). Therefore we do not use the *cutoff operator* (which removes
low-weighted labels), but instead trim-off labels if they do not fit within
labelset capacity. *Labels* are **sorted by weight** such that only low-weighted
labels are eliminated.

In this experiment we attempt performing the OpenMP-based LabelRank algorithm
with all different **schedule kinds** (`static`, `dynamic`, `guided`, and
`auto`), which adjusting the **chunk size** from `1` to `65536` in multiples of
2. In all cases, we use a total of `12 threads`. We choose the LabelRank
*parameters* as `inflation = 1.5` and `conditionalUpdate = 0.5`. In addition, we
use a fixed `labelset capacity` of `4`, and run the algorithm for exactly
`10 iterations`. We measure the **time taken** for the *computation* (performed 5
times for averaging), and *measure* the **modularity score** (one of the ways to
measure quality of communities). This is repeated for *seventeen* different
graphs.

From the results, we observe that a **dynamic schedule kind**, with a **chunk**
**size of 1024** appears to perform the **best** in most cases. Exceptions include
`soc-Epinions1` graphs, where using a *chunk size of 256* would be a better
choice; and `web-Google`, where using a *guided schedule kind* with *chunk size*
*of 256* would be better. Note that these paramter choices might differ if a
different *labelset capacity*, or different LabelRank *parameters* are used.

All outputs are saved in a [gist] and a small part of the output is listed here.
Some [charts] are also included below, generated from [sheets]. The input data
used for this experiment is available from the [SuiteSparse Matrix Collection].
This experiment was done with guidance from [Prof. Kishore Kothapalli] and
[Prof. Dip Sankar Banerjee].

<br>

```bash
$ g++ -std=c++17 -O3 main.cxx
$ ./a.out ~/data/web-Stanford.mtx
$ ./a.out ~/data/web-BerkStan.mtx
$ ...

# Loading graph /home/subhajit/data/web-Stanford.mtx ...
# order: 281903 size: 2312497 [directed] {}
# order: 281903 size: 3985272 [directed] {} (symmetricize)
# order: 281903 size: 4267175 [directed] {} (selfLoopAllVertices)
# OMP_NUM_THREADS=12
# [0.065665 modularity] noop
# [02884.445 ms; 0.621151 modularity] labelrankSeq
# [00512.958 ms; 0.621151 modularity] labelrankOmp {sch_kind: static, chunk_size: 1}
# [00495.183 ms; 0.621151 modularity] labelrankOmp {sch_kind: static, chunk_size: 2}
# [00469.175 ms; 0.621151 modularity] labelrankOmp {sch_kind: static, chunk_size: 4}
# ...
# [00433.700 ms; 0.621151 modularity] labelrankOmp {sch_kind: auto, chunk_size: 16384}
# [00437.218 ms; 0.621151 modularity] labelrankOmp {sch_kind: auto, chunk_size: 32768}
# [00453.283 ms; 0.621151 modularity] labelrankOmp {sch_kind: auto, chunk_size: 65536}
#
# Loading graph /home/subhajit/data/web-BerkStan.mtx ...
# order: 685230 size: 7600595 [directed] {}
# order: 685230 size: 13298940 [directed] {} (symmetricize)
# order: 685230 size: 13984170 [directed] {} (selfLoopAllVertices)
# OMP_NUM_THREADS=12
# [0.048410 modularity] noop
# [04931.904 ms; 0.642543 modularity] labelrankSeq
# [01263.836 ms; 0.642543 modularity] labelrankOmp {sch_kind: static, chunk_size: 1}
# [01182.344 ms; 0.642543 modularity] labelrankOmp {sch_kind: static, chunk_size: 2}
# [01109.387 ms; 0.642543 modularity] labelrankOmp {sch_kind: static, chunk_size: 4}
# ...
```

[![](https://i.imgur.com/cjlysbl.png)][sheetp]

<br>
<br>


## References

- [LabelRankT: Incremental Community Detection in Dynamic Networks via Label Propagation](https://arxiv.org/abs/1305.2006)
- [LabelRank: A Stabilized Label Propagation Algorithm for Community Detection in Networks](https://arxiv.org/abs/1303.0868)
- [SuiteSparse Matrix Collection]

<br>
<br>

[![](https://i.imgur.com/IweIQy2.jpg)](https://www.youtube.com/watch?v=5TtY3Wuet58)<br>


[Prof. Dip Sankar Banerjee]: https://sites.google.com/site/dipsankarban/
[Prof. Kishore Kothapalli]: https://faculty.iiit.ac.in/~kkishore/
[SuiteSparse Matrix Collection]: https://sparse.tamu.edu
[LabelRank]: https://arxiv.org/abs/1303.0868
[gist]: https://gist.github.com/wolfram77/67e45060f011e72ea4bc8cb4e8db893b
[charts]: https://imgur.com/a/VQVLJgq
[sheets]: https://docs.google.com/spreadsheets/d/17jyI_kXEQXRpk6L3lXJw054xAIPPOJ25EG1ZvdrNFXw/edit?usp=sharing
[sheetp]: https://docs.google.com/spreadsheets/d/e/2PACX-1vQ_XHjLT5Zxc5TI7M2te4ypmxU02kh8SzDzPIwfI6mrmrylPxpqPRpPFp0nWz9BI3jABn0WUc-LAeZ0/pubhtml
