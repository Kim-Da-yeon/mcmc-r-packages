# mcmc-r-packages

R packages built while learning package authoring around MCMC methods. Four
previously separate repositories, consolidated here with their full commit
history intact.

| Directory                              | What it is                                    | Backend                | Entry point               |
|----------------------------------------|-----------------------------------------------|------------------------|---------------------------|
| [`R.SamcPackage/`](R.SamcPackage/)     | SAMC, reference implementation                | Pure R                 | `run_simulation()`        |
| [`C.SamcPackage/`](C.SamcPackage/)     | SAMC, compiled                                | C++/C via **Rcpp**     | `run_samc_simulation()`   |
| [`P.SamcPackage/`](P.SamcPackage/)     | SAMC, Python interop                          | Python via **reticulate** | `run_samc_simulation()` |
| [`myNimblePackage/`](myNimblePackage/) | Minimal NIMBLE model-compilation example      | **nimble**             | `createNimbleModel()`     |

Each directory is a standalone R package — install whichever you need. Inline
documentation (roxygen comments) is written in Korean.

---

## The SAMC example

The three `*.SamcPackage` directories all implement **the same algorithm**:
Example 1 from

> Liang, F., Liu, C., & Carroll, R. J. (2007). Stochastic Approximation in
> Monte Carlo Computation. *Journal of the American Statistical Association*,
> **102**(477), 305–320.

Stochastic Approximation Monte Carlo (SAMC) samples a state space partitioned
into subsets while adaptively adjusting per-subset weights, so that the sampler
visits every subset at a prescribed rate instead of getting stuck in the modes
of the target. The toy problem used here makes that behaviour easy to check:

| Setting                             | Value                          |
|-------------------------------------|--------------------------------|
| State space size `N`                | 10                             |
| Number of subsets `NE`              | 5                              |
| Unnormalized mass `f(x)`            | `1, 100, 2, 1, 3, 3, 1, 200, 2, 1` |
| True subset weights                 | `1, 1, 2, 2, 4`                |
| Desired sampling distribution `π`   | uniform, `1/5` per subset      |
| Iterations                          | 500,000                        |
| Gain factor                         | `γ_t = t₀ / max(t₀, t)`, `t₀ = 10` |

States are grouped into subsets by their `f(x)` value (`200 → 1`, `100 → 2`,
`3 → 3`, `2 → 4`, everything else `→ 5`). Because `f` spans two orders of
magnitude, an ordinary Metropolis–Hastings sampler concentrates on states 2 and
8; SAMC's weight updates flatten that out toward the uniform target. Success is
measured by two things the run reports:

- **visiting frequency per subset** should approach 20% each, and
- **`Estimation Error of g` **, the χ²-style distance between the estimated
  weights `exp(θ)` and the true `weight` vector, should be small.

The proposal matrix `Q` (a fixed, non-symmetric 10×10 stochastic matrix) ships
with each package. `R.SamcPackage` and `C.SamcPackage` read it as
space-delimited `inst/extdata/Q.tranE`; `P.SamcPackage` reads the
comma-delimited `inst/extdata/Q.csv`. **The two files hold the same matrix** —
verified numerically identical to the last digit, differing only in delimiter
and trailing-zero formatting.

### Differences between the three backends

They are not drop-in equivalents. Known discrepancies:

| | `R.SamcPackage` | `C.SamcPackage` | `P.SamcPackage` |
|---|---|---|---|
| Return value | `list(hist, FV, fvalue)` | value from `run_samc_ex1_R()` | `list(final_x, final_k1, FV)` |
| Prints convergence table | yes | yes (from C) | no |
| Writes results to disk | `hist.txt` | `bb.hist` | no |
| Reports gain-factor error | yes | yes | **no** — the output section is omitted in `SAMC.py` |
| Initial state draw | `round(runif(1) * 10)` | C `rand()` | `round(uniform(0.1, 1.0) * 10)` |

The differing initial draw and the separate RNG streams mean the three will not
reproduce each other run-for-run even with a fixed seed. Only the converged
summaries are comparable.

---

## Installation and use

All four are installed from source. From the repository root:

```r
install.packages("devtools")

devtools::install("R.SamcPackage")
library(R.SamcPackage)
res <- run_simulation()          # ~500k iterations, prints progress every 1000
```

```r
# needs a C++17 toolchain (Rtools on Windows, Xcode CLT on macOS)
devtools::install("C.SamcPackage")
C.SamcPackage::run_samc_simulation()
```

```r
# needs a Python installation with numpy visible to reticulate
devtools::install("P.SamcPackage")
P.SamcPackage::run_samc_simulation()
```

```r
devtools::install("myNimblePackage")
model <- myNimblePackage::createNimbleModel()   # compiles x ~ dnorm(0, 1)
```

Note that a 500,000-iteration run in pure R takes a long while; use
`C.SamcPackage` if you just want the numbers.

---

## Provenance

- The SAMC algorithm and the Example 1 setup are from Liang, Liu & Carroll
  (2007), cited above.
- The original program these packages are ported from is credited in the source
  header as **Cheon, Sooyoung (2010-06-14)**. The R, C and Python versions here
  are translations of that program, wrapped as R packages.
- `C.SamcPackage/src/nrutil.c` and `nrutil.h` are the array-allocation
  utilities from *자체 구현 in C* (Press, Teukolsky, Vetterling &
  Flannery). See [the note below](#open-issues).

## Open issues

- **`nrutil` licensing.** *자체 구현* source is copyrighted and its
  license does not permit redistribution, which conflicts with this
  directory being public and declared `GPL-3`. Only six routines are actually
  used (`ivector`, `dvector`, `dmatrix` and their `free_*` counterparts), all
  thin 1-indexed wrappers over `malloc`, so replacing them with `R_alloc`-based
  equivalents would resolve it.
- `C.SamcPackage` writes its output to a fixed `bb.hist` in the working
  directory, and `R.SamcPackage` to `hist.txt`, in both cases appending rather
  than truncating. Repeated runs accumulate into the same file.
- Neither the number of iterations nor any other parameter is exposed as a
  function argument; they are hard-coded. `Niter` would be the obvious first
  argument to add.

## Repository history

This repo was assembled from four single-package repositories, now archived:

- [`Kim-Da-yeon/R.SamcPackage`](https://github.com/Kim-Da-yeon/R.SamcPackage)
- [`Kim-Da-yeon/C.SamcPackage`](https://github.com/Kim-Da-yeon/C.SamcPackage)
- [`Kim-Da-yeon/P.SamcPackage`](https://github.com/Kim-Da-yeon/P.SamcPackage)
- [`Kim-Da-yeon/myNimblePackage`](https://github.com/Kim-Da-yeon/myNimblePackage)

Each was merged with `git subtree`, so the original commits are preserved in this
repository rather than replayed. They are reachable as the second parent of the
corresponding merge, e.g.

```sh
git log --oneline "$(git log --format=%H --grep='import R.SamcPackage' -1)^2"
```

A plain `git log -- R.SamcPackage/` will *not* list them: those commits recorded
paths at the repository root (`DESCRIPTION`, not `R.SamcPackage/DESCRIPTION`), so
path filtering stops at the merge. There is little to miss either way — each
original repo held only one or two commits.
