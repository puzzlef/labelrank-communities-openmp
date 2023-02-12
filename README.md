Comparing approaches for *community detection* using *OpenMP-based* **LabelRank algorithm**.

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
labels are eliminated. The input data used for each experiment
given below is available from the [SuiteSparse Matrix Collection]. All
experiments are done with guidance from [Prof. Kishore Kothapalli] and
[Prof. Dip Sankar Banerjee].

<br>


### Adjusting OpenMP schedule

In this experiment ([adjust-schedule], [main]), we attempt performing the
OpenMP-based LabelRank algorithm with all different **schedule kinds**
(`static`, `dynamic`, `guided`, and `auto`), which adjusting the **chunk size**
from `1` to `65536` in multiples of 2. In all cases, we use a total of `12 threads`.
We choose the LabelRank *parameters* as `inflation = 1.5` and `conditionalUpdate = 0.5`.
In addition, we use a fixed `labelset capacity` of `4`, and run the algorithm
for exactly `10 iterations`. We measure the **time taken** for the *computation*
(performed 5 times for averaging), and *measure* the **modularity score** (one
of the ways to measure quality of communities). This is repeated for *seventeen*
different graphs.

From the results, we observe that a **dynamic schedule kind**, with a **chunk**
**size of 1024** appears to perform the **best** in most cases. Exceptions include
`soc-Epinions1` graphs, where using a *chunk size of 256* would be a better
choice; and `web-Google`, where using a *guided schedule kind* with *chunk size*
*of 256* would be better. Note that these paramter choices might differ if a
different *labelset capacity*, or different LabelRank *parameters* are used.

[adjust-schedule]: https://github.com/puzzlef/labelrank-communities-openmp/tree/adjust-schedule
[main]: https://github.com/puzzlef/labelrank-communities-openmp

<br>
<br>


## References

- [LabelRankT: Incremental Community Detection in Dynamic Networks via Label Propagation](https://arxiv.org/abs/1305.2006)
- [LabelRank: A Stabilized Label Propagation Algorithm for Community Detection in Networks](https://arxiv.org/abs/1303.0868)
- [SuiteSparse Matrix Collection]

<br>
<br>


[![](https://img.youtube.com/vi/5TtY3Wuet58/maxresdefault.jpg)](https://www.youtube.com/watch?v=5TtY3Wuet58)<br>
[![ORG](https://img.shields.io/badge/org-puzzlef-green?logo=Org)](https://puzzlef.github.io)
[![DOI](https://zenodo.org/badge/513425848.svg)](https://zenodo.org/badge/latestdoi/513425848)


[Prof. Dip Sankar Banerjee]: https://sites.google.com/site/dipsankarban/
[Prof. Kishore Kothapalli]: https://faculty.iiit.ac.in/~kkishore/
[SuiteSparse Matrix Collection]: https://sparse.tamu.edu
[LabelRank]: https://arxiv.org/abs/1303.0868
