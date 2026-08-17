#include <Rcpp.h>
extern "C" {
  // run_samc_ex1는 이제 Q.tranE 경로를 인자로 받습니다.
  SEXP run_samc_ex1(SEXP qpathSEXP);
}

// [[Rcpp::export]]
SEXP run_samc_ex1_R(SEXP qpathSEXP) {
  return run_samc_ex1(qpathSEXP);
}
