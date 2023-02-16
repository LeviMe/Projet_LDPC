#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
Created on Tue Nov  8 23:45:06 2022

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
    def __init__(self,N=2**6, xmin = 2/3, xmax =4/3, f = np.log):
        self.N = N
        self.xmin, self.xmax = xmin, xmax
        X = np.linspace(self.xmin,self.xmax,N)
        self.step = (self.xmax-self.xmin)/(N-1)
        self.step_inv = 1/self.step
        self.L = f(X)
        
    def compute_log(self,x): #interpolation linéaire
        coeff, expn = sci_notation(x)
        coeff2 = (coeff-1)/(coeff+1)
        factor = expn*LOG2     
        idx = int((coeff2-self.xmin)*self.step_inv)
        a = (self.L[idx+1] - self.L[idx])*self.step_inv
        s = a*(coeff2-(idx*self.step+self.xmin)) + self.L[idx] 
        return factor + s

class LUT_4(LUT):
    def __init__(self,N=2**6):
        f0 = lambda  x : 2*np.arctanh(x)/x
        xmin = -0.2/(32*N)
        super().__init__(N,xmin, 0.2,f0)

    def compute_log(self,x): #interpolation linéaire
        coeff, expn = sci_notation(x)
        coeff2 = (coeff-1)/(coeff+1)
        coeff3, sn = abs(coeff2), np.sign(coeff2)
        
        factor = expn*LOG2  
        idx = int((coeff3-self.xmin)*self.step_inv)
        a = (self.L[idx+1] - self.L[idx])*self.step_inv
        s = a*(coeff3-(idx*self.step+self.xmin)) + self.L[idx] 
        return factor + sn*coeff3*s
        
def test1():
    exec_time = []
    error = []
    X  = np.linspace(0.0000001,2,1000000)
    Y2 = np.log(X)

    for size in range(15,19):    
        A = LUT_4(N=1<<size)
        
        start_time = time.time()
        Y = [A.compute_log(x) for x in X]
        t2 = time.time() - start_time
        
        mn = np.mean(np.abs(np.array(Y) - Y2))
        print(size, mn)
        exec_time+=[t2]
        error+=[mn]
    
    print(exec_time, error)
    plt.plot(exec_time)
    plt.plot(error)

test1()


def test0():
    exec_time = []
    error = []
    X  = np.linspace(0.0000001,2,1000000)
    Y2 = np.log(X)
    
    
    f0 = lambda  x : 2*np.arctanh(x)
    for size in range(6,7):    
        A = LUT(N=1<<size, xmin=-1/5, xmax=1/7, f=f0)
        
        start_time = time.time()
        Y = [A.compute_log(x) for x in X]
        t2 = time.time() - start_time
        
        mn = np.mean(np.abs(np.array(Y) - Y2))
        exec_time+=[t2]
        error+=[mn]
    
    print(exec_time, error)
    plt.plot(exec_time)
    plt.plot(error)





