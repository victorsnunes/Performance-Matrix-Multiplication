import time
import numpy as np

def OnMult(m_ar, m_br):
    pha = []
    phb = []
    phc = []
    # for i in range(m_ar):
    #     for j in range(m_ar):
    #         type(m_ar)
    #         print(i*m_ar+j)
    #         pha[i*m_ar + j] = 1.0
    pha = np.ones((m_ar,m_ar), dtype=float)
    print(pha)
    print(" \n")
    aux = np.arange(m_br)
    for i in range(len(aux)):
        aux[i] = aux[i]+1
    phb = np.full((m_br,m_br), aux)
    phb = phb.transpose()
    print(phb)
    # phb.reshape((m_br,m_br))
    # for i in range(m_br):
    #     for j in range(len(m_br)):
    #         phb[i*m_br + j] = (i+1)
    
    t1 = time.process_time()
    res = [[0 for x in range(m_ar)] for y in range(m_br)]
    for i in range(len(pha)):
        for j in range(len(phb[0])):
            for k in range(len(phb)):
                res[i][j] += pha[i][k]*phb[k][j]
    t2= time.process_time()
    result = (t2-t1)
    print('Time {:.4f} milliseconds \n'.format(result*1000))

    print('Result matrix:')
    print(res)
    pha = []
    phb = []
    phc = []        
    return result
col = 0
lin = 0
opt = 1
while opt != 0:
    print("1. Multiplication")
    print("2. Line Multiplication")
    print("3 . Block")
    opt =  int(input ("Selection?:"))
    print(opt)
    if opt == 0:
        break
    if opt == 1 or opt == 2:
        print("hi")
        dimension = int(input("Dimensions: lins=cols?"))
        lin = dimension
        col = lin
        OnMult(lin, col)
    print(opt)



