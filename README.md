# mcmc-r-packages

SAMC (Liang, Liu & Carroll 2007, Example 1)를 R 패키지 세 벌로 구현 — 순수 R, C/Rcpp, Python/reticulate. NIMBLE 최소 예제 포함.

---

## 구성

| 디렉터리 | 백엔드 | 진입점 |
|---|---|---|
| [`R.SamcPackage/`](R.SamcPackage/) | 순수 R | `run_simulation()` |
| [`C.SamcPackage/`](C.SamcPackage/) | C / Rcpp | `run_samc_simulation()` |
| [`P.SamcPackage/`](P.SamcPackage/) | Python / reticulate | `run_samc_simulation()` |
| [`myNimblePackage/`](myNimblePackage/) | nimble | `createNimbleModel()` |

각 디렉터리가 독립 R 패키지. roxygen 주석은 한국어.

## 알고리즘

SAMC는 상태공간을 부분집합으로 나누고 부분집합별 가중치를 적응적으로 갱신해, 목표분포의 모드에 갇히지 않고 각 부분집합을 지정 비율로 방문한다.

| 설정 | 값 |
|---|---|
| 상태공간 `N` | 10 |
| 부분집합 `NE` | 5 |
| 비정규화 질량 `f(x)` | 1, 100, 2, 1, 3, 3, 1, 200, 2, 1 |
| 참 가중치 | 1, 1, 2, 2, 4 |
| 목표 표본분포 `π` | 균등, 부분집합당 1/5 |
| 반복 | 500,000 |
| 이득 계수 | `γ_t = t₀ / max(t₀, t)`, `t₀ = 10` |

`f(x)` 값으로 부분집합 배정 (200→1, 100→2, 3→3, 2→4, 그 외→5). `f`가 두 자릿수 배 차이라 일반 MH는 상태 2·8에 집중되고, SAMC의 가중치 갱신이 이를 균등 목표로 평탄화. 판정 기준은 부분집합별 방문 비율 20% 수렴과 `Estimation Error of g`.

제안 행렬 `Q`는 고정된 비대칭 10×10 확률행렬. R·C는 공백 구분 `Q.tranE`, Python은 콤마 구분 `Q.csv`를 읽으며 **두 파일의 수치는 동일**(자릿수까지 일치 확인).

### 세 구현의 차이

|  | 순수 R | C/Rcpp | Python |
|---|---|---|---|
| 반환값 | `list(hist, FV, fvalue)` | `run_samc_ex1_R()` 반환값 | `list(final_x, final_k1, FV)` |
| 수렴 표 출력 | 있음 | 있음 | 없음 |
| 파일 저장 | `hist.txt` | `bb.hist` | 없음 |
| 이득계수 오차 보고 | 있음 | 있음 | 없음 (`SAMC.py` 출력부 생략) |
| 초기 상태 추출 | `round(runif(1)*10)` | C `rand()` | `round(uniform(0.1,1.0)*10)` |

초기 추출과 난수 스트림이 달라 seed를 고정해도 실행 단위 재현은 불가. 수렴 후 요약값만 비교 가능.

## 설치

```r
install.packages("devtools")

devtools::install("R.SamcPackage");  R.SamcPackage::run_simulation()
devtools::install("C.SamcPackage");  C.SamcPackage::run_samc_simulation()   # C++17 툴체인 필요
devtools::install("P.SamcPackage");  P.SamcPackage::run_samc_simulation()   # numpy 가시 필요
devtools::install("myNimblePackage"); myNimblePackage::createNimbleModel()
```

순수 R로 500,000 반복은 오래 걸림. 수치만 필요하면 `C.SamcPackage`.

## 출처

- 알고리즘·예제 설정: Liang, F., Liu, C., & Carroll, R. J. (2007). Stochastic Approximation in Monte Carlo Computation. *JASA*, 102(477), 305–320.
- 이식 원본: 소스 헤더에 `Cheon, Sooyoung (2010-06-14)` 명시
- `C.SamcPackage/src/nrutil.c`, `nrutil.h`: 1-based 배열·행렬 할당용 자체 구현 (malloc 래퍼, 외부 코드 아님)

## 미해결

- 출력이 append 모드. 반복 실행 시 `bb.hist`·`hist.txt`에 누적
- 반복 수 등 파라미터가 하드코딩. `Niter`부터 인자화 필요

## 이력

단일 패키지 저장소 4개를 `git subtree`로 병합, 원본은 archive. 원 커밋은 각 병합의 두 번째 부모로 보존.

```sh
git log --oneline "$(git log --format=%H --grep='import R.SamcPackage' -1)^2"
```

원 커밋은 저장소 루트 기준 경로를 기록하므로 `git log -- R.SamcPackage/`로는 조회되지 않음.
