#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
Created on Thu Oct 20 19:23:29 2022
@author: levi
"""
import numpy as np
import matplotlib.pyplot as plt
import timeit, time

LOG2 = 0.693147180559945309417232121458 

def sci_notation(x):
    assert(x>0)
    if x<1:
        coeff,expn = x, 0
        while(coeff<1):
            coeff*=2
            expn-=1
    else:
        x_int=int(x)
        i=0
        while(x_int!=0):
            x_int>>=1
            i+=1
        expn = i-1
        coeff = x/(1<<(expn))
    
    return coeff, expn

# x est ici nécessairement un float
def sci_notation2(x):
    expn = int(x.hex().split('p')[1])
    if expn>0:
        coeff = x/(1<<(expn))
    else:
        coeff = x * (1<<(-expn))
    return  coeff, expn


def closest_to_1(coeff,expn):
    if coeff> 1.3333333333333333:
        coeff, expn = coeff/2, expn+1        
    return coeff,expn
    

log_coeffs = [1/(1+i) for i in range(10)]
def log(x):
    coeff, expn = sci_notation2(x)
    coeff, expn = closest_to_1(coeff, expn)   
    factor = expn*LOG2     
    order = 7
    coeff_m_1 = (coeff-1)
    s = 0
    prod = coeff_m_1
    for i in range(order):
        s+=log_coeffs[i] * prod
        prod*=-coeff_m_1
    return factor + s

log_atanh_coeffs = [2/(1+2*i) for i in range(10)]

def log_atanh(x,order):
    coeff, expn = sci_notation2(x)
    coeff, expn = closest_to_1(coeff, expn)    
    factor = expn*LOG2
    s = 0
    p0 = (coeff-1)/(coeff+1)
    prod = p0
    p02 = p0*p0
    for i in range(order):
        s+=log_atanh_coeffs[i] * prod
        prod*=p02
    return factor + s


if 1:
    X  = np.linspace(0.0000001,2,1000000)
    
    start_time = time.time()
    for x in X:
        log(x)
    #[log_atanh(x,7) for x in X]
    
    # for order in range(7,8):    
    #     Y = [np.log(x) - log_atanh(x,order) for x in X]
    #     plt.plot(X,np.abs(Y))
    #     print(order,":", np.mean(np.abs(Y)))
    #     plt.show()
    
    t2 = time.time() - start_time
    print(t2)    

