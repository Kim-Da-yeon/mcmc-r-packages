//////////////////////////////////////////////////////////////////////////////////////////////////
//
// Name : Example 1 of 3. Two Demonstartion Examples in 'Stochastic Approximation in Monte Carlo Computation'
// Inf  : Convergence of SAMC
// Date : 01/25/08
// Name : Sooyoung Cheon
//
//////////////////////////////////////////////////////////////////////////////////////////////////

// C++ 코드에서는 표준 C 헤더 대신 <cstdio>, <cstdlib>, <cmath>, <ctime> 사용
#include <R.h>             // R 인터페이스 추가
#include <Rinternals.h>    // R 객체 반환을 위해 추가
#include <cstdio>
#include <cstdlib>
#include <cmath>
#include <ctime>

// 필요한 경우 std 네임스페이스 지정
using std::fopen;
using std::fclose;
using std::fscanf;
using std::fprintf;

#define pi 3.14159265

#define Nrep            1
#define Niter      500000    // 원본과 동일: 500000회 반복
#define N            10
#define NE           5
#define t0           10.0
#define BTHETA       pow(10,20)
#define MAXIMUM(a,b) ((a)>(b)?(a):(b))

double GAMMA, *STPI, maxx, minn;
int weight[6] = {0, 1, 1, 2, 2, 4};

#include "nrutil.h"
#include "nrutil.c"

/*
 [수정 포인트 1]
 Metropolis_moves 함수를 ANSI C 스타일로 선언 및 정의
 */
int Metropolis_moves(int *y, int *k, int *fvalue, double **hist, double **Q) {
  double r, un, sum;
  int i, x, k1, z, k3, accept;

  x = *y;
  k1 = *k;
  un = rand() * 1.0 / RAND_MAX;
  sum = Q[x][1];
  z = 1;
  while (un > sum && z < 10) {
    z++;
    sum += Q[x][z];
  }

  if (fvalue[z] == 200)
    k3 = 1;
  else if (fvalue[z] == 100)
    k3 = 2;
  else if (fvalue[z] == 3)
    k3 = 3;
  else if (fvalue[z] == 2)
    k3 = 4;
  else
    k3 = 5;

  r = 1.0 * exp(hist[k1][2] - hist[k3][2]) * (Q[z][x] / Q[x][z]);

  if (r > 1.0)
    accept = 1;
  else {
    un = rand() * 1.0 / RAND_MAX;
    if (un < r)
      accept = 1;
    else
      accept = 0;
  }

  if (accept == 1) {
    for (i = 1; i <= NE; i++) {
      if (i == k3)
        hist[i][2] += 1.0 * GAMMA * (1.0 - STPI[i]);
      else
        hist[i][2] -= 1.0 * GAMMA * STPI[i];
    }
    hist[k3][3] += 1.0;
    x = z;
    k1 = k3;
  } else {
    for (i = 1; i <= NE; i++) {
      if (i == k1)
        hist[i][2] += 1.0 * GAMMA * (1.0 - STPI[i]);
      else
        hist[i][2] -= 1.0 * GAMMA * STPI[i];
    }
    hist[k1][3] += 1.0;
  }

  for (i = 1; i <= NE; i++) {
    if (hist[i][2] > maxx) maxx = hist[i][2];
    if (hist[i][2] < minn) minn = hist[i][2];
  }
  if (maxx > BTHETA) {
    for (i = 1; i <= NE; i++) {
      hist[i][2] = hist[i][2] + BTHETA / 2.0 - maxx;
    }
  }
  if (minn < -BTHETA) {
    for (i = 1; i <= NE; i++) {
      hist[i][2] = hist[i][2] - BTHETA / 2.0 - minn;
    }
  }

  *y = x;
  *k = k1;
  return 0;
}

/*
 [수정 포인트 2]
 원본 main() 함수는 독립 실행형 프로그램용이므로, R 패키지에서는 사용하지 않도록 주석 처리합니다.
 */
#if 0
main()
{
  // 원본 main() 함수 코드 (파일 입출력 등 포함)
  // ... (생략)
  return 0;
}
#endif

/*
 [수정 포인트 3]
 R에서 호출할 수 있도록 Q.tranE 파일의 경로를 인자로 받도록 수정합니다.
 run_samc_ex1() 함수는 SEXP 타입 인자를 받아, R에서 전달한 Q.tranE 파일 경로를 사용합니다.
 */
extern "C" SEXP run_samc_ex1(SEXP qpathSEXP) {
  const char *qpath = CHAR(asChar(qpathSEXP));  // R 문자열을 C 문자열로 변환

  long ltime;
  int stime;
  int x, k1, i, rep, iter, *fvalue;
  double sum, msum, **hist, **Q, *FV;
  FILE *ins;

  // Random generator 초기화
  ltime = time(NULL);
  stime = (int)(ltime / 2);
  srand(stime);

  // 파라미터 생성
  fvalue = ivector(1, N);
  hist = dmatrix(1, NE, 1, 3);
  Q = dmatrix(1, 10, 1, 10);
  FV = dvector(1, 10);
  STPI = dvector(1, NE);

  // Mass function
  fvalue[1] = 1;  fvalue[2] = 100; fvalue[3] = 2;  fvalue[4] = 1;  fvalue[5] = 3;
  fvalue[6] = 3;  fvalue[7] = 1;  fvalue[8] = 200; fvalue[9] = 2;  fvalue[10] = 1;

  // Desired sampling distribution
  for (i = 1; i <= NE; i++) STPI[i] = 1.0 / NE;

  // Proposal distribution: Q.tranE 파일을 지정된 경로에서 읽습니다.
  ins = fopen(qpath, "r");
  if (ins == NULL) {
    error("Q.tranE 파일을 열 수 없습니다.");
  }
  for (i = 1; i <= N; i++) {
    for (int j = 1; j <= N; j++) {
      fscanf(ins, " %lf", &Q[i][j]);
    }
  }
  fclose(ins);

  // Repetition (원본 주석 그대로)
  for (rep = 1; rep <= Nrep; rep++) {
    for (i = 1; i <= NE; i++) {
      hist[i][1] = i;
      hist[i][3] = 0.0;
    }
    hist[1][2] = hist[2][2] = log(1);
    hist[3][2] = hist[4][2] = log(2);
    hist[5][2] = log(4);

    // Parameter for the visiting probability of each subset
    for (i = 1; i <= 10; i++) FV[i] = 0.0;

    // Generate subset randomly: x => k1
    x = 11;
    while (x > 10) x = (int) ceil(rand() * 10.0 / RAND_MAX);
    if (fvalue[x] == 200) k1 = 1;
    else if (fvalue[x] == 100) k1 = 2;
    else if (fvalue[x] == 3) k1 = 3;
    else if (fvalue[x] == 2) k1 = 4;
    else k1 = 5;

    // Run MH step 5x10^5 times (원본 그대로)
    for (iter = 1; iter <= Niter; iter++) {
      GAMMA = 1.0 * pow(t0 / MAXIMUM(t0, iter), 1.0);
      Metropolis_moves(&x, &k1, fvalue, hist, Q);
      FV[x] += 1.0;
      if (iter % 50000 == 0) {
        Rprintf("%d  %f\n", iter, GAMMA);
      }
    }

    // Check for convergence of estimated sampling distribution (원본 그대로)
    ins = fopen("bb.hist", "w");
    if (ins == NULL) {
      error("bb.hist 파일을 열 수 없습니다.");
    }
    for (sum = 0.0, i = 1; i <= NE; i++) {
      if (hist[i][3] != 0) sum += hist[i][3];
    }
    msum = 1.0 * sum / NE;
    fprintf(ins, "Subset    Theta       g      #(Visit) Prob(pi)  Relative-Rate\n");
    for (i = 1; i <= NE; i++) {
      if (hist[i][3] != 0) {
        fprintf(ins, "%6.0f  %8.4f  %8.4f  %8.0f    %4.2f     %8.2f\n",
                hist[i][1], hist[i][2], exp(hist[i][2]), hist[i][3],
                                                                hist[i][3] / sum, (hist[i][3] / msum) * 100.0);
      }
    }
    fprintf(ins, "\n");

    fprintf(ins, "  x   p(x)    f(x)\n");
    for (sum = 0.0, i = 1; i <= 10; i++) sum += FV[i];
    for (i = 1; i <= 10; i++) {
      fprintf(ins, "%3d   %3d  %8.4f\n", i, fvalue[i], 1.0 * FV[i] / sum);
    }
    fprintf(ins, "\n");

    // Check for convergence of estimated weight
    for (sum = 0.0, i = 1; i <= NE; i++) {
      if (hist[i][3] != 0)
        sum += pow(exp(hist[i][2]) - weight[i], 2) / weight[i];
    }
    fprintf(ins, "Estimation Error of g(gain factor)= %4.8f\n", sqrt(sum));
    fclose(ins);
  } /*end rep*/

free_ivector(fvalue, 1, N);
  free_dmatrix(hist, 1, NE, 1, 3);
  free_dmatrix(Q, 1, 10, 1, 10);
  free_dvector(STPI, 1, NE);
  free_dvector(FV, 1, 10);

  return R_NilValue;
}
