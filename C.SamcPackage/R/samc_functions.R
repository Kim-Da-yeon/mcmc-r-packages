#' Run SAMC Simulation using C code via Rcpp
#'
#' 이 함수는 C 코드로 구현된 SAMC 시뮬레이션을 실행합니다.
#' Q.tranE 파일의 경로는 inst/extdata 폴더 내에 있어야 합니다.
#'
#' @return C 코드에서 생성된 결과를 반환합니다.
#' @export
run_samc_simulation <- function() {
  # system.file()을 사용해 Q.tranE 파일의 경로를 가져옵니다.
  qpath <- system.file("extdata", "Q.tranE", package = "C.SamcPackage")
  if(qpath == "") stop("Q.tranE 파일이 inst/extdata 폴더에 없습니다.")

  result <- run_samc_ex1_R(qpath)
  return(result)
}
