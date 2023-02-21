# -*- coding: utf-8 -*-
"""
Created on Thu Oct 20 19:23:29 2022
@author: levi
"""
import numpy as np, math
import matplotlib.pyplot as plt
import time

def sci_notation(x):
    coeff, expn = math.frexp(x)
    if coeff<0.666666666666:
        coeff, expn = 2*coeff, expn-1
    return  coeff, expn

LOG2 = 0.693147180559945309417232121458

class LUT:
    def __init__(self,N):
        self.N = N
        self.xmin, self.xmax = 2/3, 4/3
        X = np.linspace(self.xmin,self.xmax,N)
        self.step = (self.xmax-self.xmin)/(N-1)
        self.step_inv = 1/self.step
        self.L = np.log(X)
        
    def compute_log(self,x): # arrondi
        coeff, expn = sci_notation(x)
        factor = expn*LOG2     
        idx = int(np.round((coeff-self.xmin)*self.step_inv))
        s = self.L[idx] 
        return factor + s

    def compute_log_2(self,x): #interpolation linéaire
        coeff, expn = sci_notation(x)
        factor = expn*LOG2     
        idx = int((coeff-self.xmin)*self.step_inv)
        a = (self.L[idx+1] - self.L[idx])*self.step_inv
        s = a*(coeff-(idx*self.step+self.xmin)) + self.L[idx] 
        return factor + s


#meilleur LUT: réduire l'argument sur l'image de 2/3, 4/3 par la transformée de ATANH.
#Composer la lUT de ATANH plutôt que log. 



exec_time = []
error = []
X  = np.linspace(0.0000001,2,1000000)
Y2 = np.log(X)

for size in range(5,19):    
    A = LUT(1<<size)
    
    start_time = time.time()
    Y = [A.compute_log_2(x) for x in X]
    t2 = time.time() - start_time
    
    mn = np.mean(np.abs(np.array(Y) - Y2))
    exec_time+=[t2]
    error+=[mn]

print(exec_time, error)
plt.plot(exec_time)
#plt.plot(error)
