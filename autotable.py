alg = ["NRU","LRU","NOVO"]
log = ["compilador","compressor","matriz","simulador"]
pag = ["8","16","32"]

M = dict()
for p in pag:
    M[p] = dict()
    for l in log:
        M[p][l] = dict()
        for a in alg:
            M[p][l][a] = dict()

with open("autoresults.txt") as f:
    alg_c = 0
    for a in alg:
        log_c = 0
        f.readline() # algorithm name
        for l in log:
            pag_c = 0
            f.readline() # log file
            for p in pag:
                f.readline() # page size
                M[pag[pag_c]][log[log_c]][alg[alg_c]]["pf"] = f.readline()[:-1]
                M[pag[pag_c]][log[log_c]][alg[alg_c]]["pw"] = f.readline()[:-1]
                pag_c += 1
            log_c += 1
        alg_c += 1

for p in pag:
    print("\n===== Page size = %s KB" % (p))
    print()
    print("%s|" % (' '*14),end = '')
    for a in alg:
        print("-"*7,end = '-')
        print("-"*7,end = '|')
    print()
    print("%s|" % (' '*14),end = '')
    for a in alg:
        print(a.center(15),end = '|')
    print()
    print("%s|" % (' '*14),end = '')
    for a in alg:
        print("-"*7,end = '-')
        print("-"*7,end = '|')
    print()
    print("%s|" % (' '*14),end = ' ')
    for a in alg:
        print("PF".center(6)+"|",end = ' ')
        print("PW".center(6)+"|",end = ' ')
    print()
    print("%s|" % ('-'*14),end = '')
    for a in alg:
        print("-"*7,end = '|')
        print("-"*7,end = '|')
    print()
    for l in log:
        print("|%13s|" % (l),end=' ')
        for a in alg:
            print("%7s" % (M[p][l][a]["pf"]+"|"),end = ' ')
            print("%7s" % (M[p][l][a]["pw"]+"|"),end = ' ')
        print()
    print("%s" % ('-'*14),end = '-')
    for a in alg:
        print("-"*7,end = '-')
        print("-"*7,end = '-')
    print()
