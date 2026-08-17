#' Run SAMC Simulation using Python Code
#'
#' 이 함수는 inst/extdata 폴더에 저장된 SAMC.py 파일과 Q.csv 파일을 불러와
#' reticulate를 통해 run_simulation() 함수를 실행합니다.
#'
#' @return run_simulation()의 실행 결과를 반환합니다.
#' @export
run_samc_simulation <- function() {
  if (!requireNamespace("reticulate", quietly = TRUE)) {
    install.packages("reticulate")
  }
  library(reticulate)

  # SAMC.py 파일 경로 (패키지 내부 inst/extdata 폴더)
  py_file <- system.file("extdata", "SAMC.py", package = "P.SamcPackage")
  if (py_file == "") {
    stop("SAMC.py 파일이 inst/extdata 폴더에 없습니다.")
  }

  # Q.csv 파일 경로 (패키지 내부 inst/extdata 폴더)
  qcsv_path <- system.file("extdata", "Q.csv", package = "P.SamcPackage")
  if (qcsv_path == "") {
    stop("Q.csv 파일이 inst/extdata 폴더에 없습니다.")
  }

  # SAMC.py 파일을 불러옵니다.
  source_python(py_file)

  # run_simulation() 함수는 이제 qcsv_path 인자를 받도록 수정되었습니다.
  result <- run_simulation(qcsv_path)

  return(result)
}
