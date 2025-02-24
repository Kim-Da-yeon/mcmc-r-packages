
#' Create and compile a simple NIMBLE model
#'
#' 이 함수는 단순한 정규분포 모델을 정의하고, 이를 NIMBLE로 컴파일한 후 반환합니다.
#'
#' @return 컴파일된 nimble 모델 객체.
#' @export
createNimbleModel <- function() {
  # nimble 패키지가 설치되어 있지 않으면 에러 메시지 출력
  if (!requireNamespace("nimble", quietly = TRUE)) {
    stop("Please install the 'nimble' package.")
  }
  library(nimble)

  # NIMBLE 모델 코드 정의: x가 표준정규분포를 따름
  code <- nimbleCode({
    x ~ dnorm(0, 1)
  })

  # 모델 객체 생성 및 초기화
  model <- nimbleModel(code)

  # 모델 컴파일 (속도 향상을 위해 C++ 코드로 변환)
  compiledModel <- compileNimble(model)

  return(compiledModel)
}

