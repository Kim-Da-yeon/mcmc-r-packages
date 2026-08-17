#####################################################################################################################
### Program : Example 1 in Stochastic Approximation in Monte Carlo Computation
###           (Journal of the American Statistical Association, 2007,
###            Vol 102, No 477, pp 305-320)
### Name    : Cheon, Sooyoung
### Date    : 2010/06/14
#####################################################################################################################

#####################################################################################################################
## Name : Metropolis_moves
## Info : sample move
#####################################################################################################################
Metmoves <- function(y, k, fvalue, hist, Q, GAMMA, STPI, NE) {
  # 원본 주석 "y=x, k=k1"를 반영
  x <- y
  k1 <- k

  maxx <- 0
  minn <- 1000
  BTHETA <- 10^20

  un <- runif(1)
  sum_val <- Q[x, 1]   # 내장 함수 sum과 충돌 방지를 위해 변수명 sum_val 사용
  z <- 1
  while (un > sum_val && z < 10) {
    z <- z + 1
    sum_val <- sum_val + Q[x, z]
  }

  if (fvalue[z] == 200) {
    k3 <- 1
  } else if (fvalue[z] == 100) {
    k3 <- 2
  } else if (fvalue[z] == 3) {
    k3 <- 3
  } else if (fvalue[z] == 2) {
    k3 <- 4
  } else {
    k3 <- 5
  }

  ## p(x)=1
  r <- 1.0 * exp(hist[k1, 2] - hist[k3, 2]) * (Q[z, x] / Q[x, z])

  if (r > 1.0) {
    accept <- 1
  } else {
    un <- runif(1)
    if (un < r) {
      accept <- 1
    } else {
      accept <- 0
    }
  }

  if (accept == 1) {
    for (i in 1:NE) {
      if (i == k3) {
        hist[i, 2] <- hist[i, 2] + 1.0 * GAMMA * (1.0 - STPI[i])
      } else {
        hist[i, 2] <- hist[i, 2] - 1.0 * GAMMA * STPI[i]
      }
    }
    hist[k3, 3] <- hist[k3, 3] + 1.0
    x <- z
    k1 <- k3
  } else {
    for (i in 1:NE) {
      if (i == k1) {
        hist[i, 2] <- hist[i, 2] + 1.0 * GAMMA * (1.0 - STPI[i])
      } else {
        hist[i, 2] <- hist[i, 2] - 1.0 * GAMMA * STPI[i]
      }
    }
    hist[k1, 3] <- hist[k1, 3] + 1.0
  }

  for (i in 1:NE) {
    if (hist[i, 2] > maxx) maxx <- hist[i, 2]
    if (hist[i, 2] < minn) minn <- hist[i, 2]
  }
  if (maxx > BTHETA) {
    for (i in 1:NE) {
      hist[i, 2] <- hist[i, 2] + BTHETA/2.0 - maxx
    }
  }
  if (minn < -BTHETA) {
    for (i in 1:NE) {
      hist[i, 2] <- hist[i, 2] - BTHETA/2.0 - minn
    }
  }

  y <- x
  k <- k1

  list(y, k, fvalue, hist)
}

#####################################################################################################################
## Name : save_results
## Info : 결과 출력 및 파일 저장 함수 (원본 "Check for convergence ..." 부분 그대로)
#####################################################################################################################
save_results <- function(hist, FV, fvalue, weight, NE) {
  # Check for convergence of estimated sampling distribution
  sum_val <- 0.0
  for (i in 1:NE) {
    if (hist[i, 3] != 0) sum_val <- sum_val + hist[i, 3]
  }
  msum <- 1.0 * sum_val / NE

  for (i in 1:NE) {
    if (hist[i, 3] != 0) {
      cat(cbind(hist[i, 1], hist[i, 2], exp(hist[i, 2]), hist[i, 3],
                hist[i, 3] / sum_val, (hist[i, 3] / msum) * 100.0), "\n")
    }
  }

  sum_val <- 0.0
  for (i in 1:10) sum_val <- sum_val + FV[i]
  for (i in 1:10) {
    cat(i, fvalue[i], 1.0 * FV[i] / sum_val, "\n")
  }

  # Check for convergence of estimated weight
  sum_val <- 0.0
  for (i in 1:NE) {
    if (hist[i, 3] != 0) {
      sum_val <- sum_val + ((exp(hist[i, 2]) - weight[i])^2) / weight[i]
    }
  }

  cat("Estimation Error of g(gain factor)= ", sqrt(sum_val), "\n")

  ###############################################
  ## save to files

  sum_val <- 0.0
  for (i in 1:NE) {
    if (hist[i, 3] != 0) sum_val <- sum_val + hist[i, 3]
  }
  msum <- 1.0 * sum_val / NE

  write(file = "hist.txt", "Subset - Theta - g - #(Visit) - Prob(pi) - Relative-Rate", append = TRUE)
  for (i in 1:NE) {
    if (hist[i, 3] != 0) {
      write(file = "hist.txt", paste(hist[i, 1],
                                     round(hist[i, 2], 4),
                                     round(exp(hist[i, 2]), 4),
                                     hist[i, 3],
                                     round(hist[i, 3] / sum_val, 4),
                                     round((hist[i, 3] / msum) * 100.0, 4)),
            append = TRUE)
    }
  }
  write(file = "hist.txt", append = TRUE, "\n")

  write(file = "hist.txt", append = TRUE, "x - p(x) - f(x)")
  sum_val <- 0.0
  for (i in 1:10) sum_val <- sum_val + FV[i]
  for (i in 1:10) {
    write(file = "hist.txt", append = TRUE, paste(i,
                                                  fvalue[i],
                                                  round(1.0 * FV[i] / sum_val, 4)))
  }
  write(file = "hist.txt", "\n", append = TRUE)

  sum_val <- 0.0
  for (i in 1:NE) {
    if (hist[i, 3] != 0) {
      sum_val <- sum_val + ((exp(hist[i, 2]) - weight[i])^2) / weight[i]
    }
  }

  write(file = "hist.txt", append = TRUE,
        paste("Estimation Error of g(gain factor)= ", round(sqrt(sum_val), 4)))
}

#####################################################################################################################
## Name : run_simulation (Main Program)
#####################################################################################################################
#' Run SAMC Simulation
#'
#' 이 함수는 SAMC 시뮬레이션 전체를 실행하고 결과를 반환합니다.
#'
#' @return 시뮬레이션 결과를 담은 리스트.
#' @export

run_simulation <- function() {

  # Set parameters
  Nrep <- 1
  Niter <- 500000
  N <- 10
  NE <- 5
  t0 <- 10.0
  weight <- c(1, 1, 2, 2, 4)

  # Generate parameters
  fvalue <- rep(0, N)
  hist <- matrix(0, NE, 3)
  Q <- matrix(0, 10, 10)
  FV <- rep(0, N)
  STPI <- rep(0, NE)

  # Mass function
  fvalue[1] <- 1; fvalue[2] <- 100; fvalue[3] <- 2; fvalue[4] <- 1; fvalue[5] <- 3;
  fvalue[6] <- 3; fvalue[7] <- 1; fvalue[8] <- 200; fvalue[9] <- 2; fvalue[10] <- 1;

  # Desired sampling distribution
  for (i in 1:NE) STPI[i] <- 1.0 / NE;

  # Proposal distribution
  Q_path <- system.file("extdata", "Q.tranE", package = "R.SamcPackage")
  if (Q_path == "") {
    stop("Q.tranE 파일이 inst/extdata 폴더에 없습니다.")
  }
  Q <- as.matrix(read.csv(file = Q_path, header = FALSE, sep = " "))

  # Initialize weight (theta)
  for (i in 1:NE) {
    hist[i, 1] <- i
    hist[i, 3] <- 0.0
  }
  hist[1, 2] <- hist[2, 2] <- log(1)
  hist[3, 2] <- hist[4, 2] <- log(2)
  hist[5, 2] <- log(4)

  for (i in 1:10) FV[i] <- 0.0

  # Generate subset randomly: x => k1
  x <- 11
  while (x > 10) x <- round(runif(1) * 10)
  if (fvalue[x] == 200) {
    k1 <- 1
  } else if (fvalue[x] == 100) {
    k1 <- 2
  } else if (fvalue[x] == 3) {
    k1 <- 3
  } else if (fvalue[x] == 2) {
    k1 <- 4
  } else {
    k1 <- 5
  }

  # Run MH steps
  for (iter in 1:Niter) {
    GAMMA <- (t0 / max(t0, iter))^1.0
    fv <- Metmoves(x, k1, fvalue, hist, Q, GAMMA, STPI, NE)
    x <- fv[[1]]
    k1 <- fv[[2]]
    fvalue <- fv[[3]]
    hist <- fv[[4]]

    FV[x] <- FV[x] + 1.0
    if (iter %% 1000 == 0) {
      cat(iter, " ", GAMMA, "\n")
    }
  }

  # Call save_results to output results and save to files
  save_results(hist, FV, fvalue, weight, NE)

  # Return simulation results as a list
  list(hist = hist, FV = FV, fvalue = fvalue)
}
