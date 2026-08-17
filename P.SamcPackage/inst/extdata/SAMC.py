# SAMC.py

import numpy as np
from math import exp, log, sqrt, ceil
from time import strftime
import os

def Metmoves(x, k1, fvalue, hist, Q):
    # (이전 코드와 동일)
    maxx = 0
    minn = 1000
    BTHETA = 10**20  

    un = np.random.uniform(low=0.0, high=1.0)
    temp_sum = Q[x-1][0]
    z = 1
    while un > temp_sum and z < 10:
        z = z + 1
        temp_sum = temp_sum + Q[x-1][z-1]
        
    if fvalue[z-1] == 200:
        k3 = 1
    elif fvalue[z-1] == 100:
        k3 = 2
    elif fvalue[z-1] == 3:
        k3 = 3
    elif fvalue[z-1] == 2:
        k3 = 4 
    else:
        k3 = 5

    r = 1.0 * exp(hist[k1-1][1] - hist[k3-1][1]) * (Q[z-1][x-1] / Q[x-1][z-1])
    
    if r > 1.0:
        accept = 1
    else:
        un = np.random.uniform(low=0.0, high=1.0)
        if un < r:
            accept = 1
        else:
            accept = 0
    
    if accept == 1:
        for i in range(0, NE):
            if i == k3-1:
                hist[i][1] = hist[i][1] + 1.0 * GAMMA * (1.0 - STPI[i])
            else:
                hist[i][1] = hist[i][1] - 1.0 * GAMMA * STPI[i]
        hist[k3-1][2] = hist[k3-1][2] + 1.0
        x = z
        k1 = k3   
    else:
        for i in range(0, NE):
            if i == k1-1:
                hist[i][1] = hist[i][1] + 1.0 * GAMMA * (1.0 - STPI[i])
            else:
                hist[i][1] = hist[i][1] - 1.0 * GAMMA * STPI[i]
        hist[k1-1][2] = hist[k1-1][2] + 1.0

    for i in range(0, NE):
        if hist[i][1] > maxx:
            maxx = hist[i][1]
        if hist[i][1] < minn:
            minn = hist[i][1]

    if maxx > BTHETA:
        for i in range(0, NE):
            hist[i][1] = hist[i][1] + BTHETA/2.0 - maxx

    if minn < -BTHETA:
        for i in range(0, NE):
            hist[i][1] = hist[i][1] - BTHETA/2.0 - minn

    return (x, k1, fvalue, hist)

def run_simulation(qcsv_path):
    global NE, GAMMA, STPI, GAMMA  # 필요한 전역변수 선언
    NE = 5
    t0 = 10.0
    Niter = 500000
    N = 10
    weight = [1, 1, 2, 2, 4]

    # Mass function
    fvalue = [1, 100, 2, 1, 3, 3, 1, 200, 2, 1]

    # Desired sampling distribution
    STPI = []
    for i in range(0, NE):
        STPI.append(1.0 / NE)

    # Proposal distribution: Q.csv 파일을 qcsv_path에서 읽기
    import csv
    Q = np.zeros((10, 10))
    with open(qcsv_path, "r") as csvfile:
        reader = csv.reader(csvfile, delimiter=',')
        data = list(reader)
    for i in range(0, 10):
        for j in range(0, 10):
            Q[i][j] = float(data[i][j])

    # Initialize weight matrix (hist)
    hist = np.zeros((NE, 3))
    for i in range(0, NE):
        hist[i][0] = i + 1
        hist[i][2] = 0.0
    hist[0][1] = hist[1][1] = log(1)
    hist[2][1] = hist[3][1] = log(2)
    hist[4][1] = log(4)

    # Parameter for the visiting probability: FV
    FV = [0.0] * N

    # Generate subset randomly: x => k1
    x = 11
    while x > 10:
        x = round(np.random.uniform(low=0.1, high=1.0) * 10)
    if fvalue[x-1] == 200:
        k1 = 1
    elif fvalue[x-1] == 100:
        k1 = 2
    elif fvalue[x-1] == 3:
        k1 = 3
    elif fvalue[x-1] == 2:
        k1 = 4
    else:
        k1 = 5

    # Run MH steps
    for iter in range(1, Niter+1):
        GAMMA = (t0 / max(t0, iter)) ** 1.0
        (x, k1, fvalue, hist) = Metmoves(x, k1, fvalue, hist, Q)
        FV[x-1] += 1.0
        if iter % 1000 == 0:
            print("Done " + str(iter) + " iterations : GAMMA " + str(GAMMA))

    # (출력 부분은 생략: 실제로 파일 저장을 원한다면 여기에 추가 가능)
    # 최종 결과를 반환하도록 하겠습니다.
    result = {"final_x": x, "final_k1": k1, "FV": FV}
    return result

# 직접 실행할 때 run_simulation()을 호출하도록 함
if __name__ == "__main__":
    # Q.csv 파일은 SAMC.py와 같은 디렉토리에 있어야 합니다.
    qcsv_path = os.path.join(os.path.dirname(__file__), "Q.csv")
    run_simulation(qcsv_path)
