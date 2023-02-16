#!/usr/bin/env python3
# -*- coding: utf-8 -*-
import numpy as np
import timeit
import math

LOG2 = 0.693147180559945309417232121458 

def sci_notation3(x):
    coeff, expn = math.frexp(x)
    if coeff<0.666666666666:
        coeff, expn = 2*coeff, expn-1
    return  coeff, expn
    
log_coeffs = [1/(1+i) for i in range(10)]
def log(x):
    coeff, expn = sci_notation3(x)
    factor = expn*LOG2     
    order = 10
    coeff_m_1 = (coeff-1)
    s = 0
    prod = coeff_m_1
    for i in range(order):
        s+=log_coeffs[i] * prod
        prod*=-coeff_m_1
    return factor + s

log_atanh_coeffs = [2/(1+2*i) for i in range(10)]
def log_atanh(x,order):
    coeff, expn = sci_notation3(x)
    factor = expn*LOG2
    s = 0
    p0 = (coeff-1)/(coeff+1)
    prod = p0
    p02 = p0*p0
    for i in range(order):
        s+=log_atanh_coeffs[i] * prod
        prod*=p02
    return factor + s

X  = np.linspace(0.0000001,2,1000000)
def test1():
    for x in X: math.log(x)

if 1:
    t1 = timeit.timeit(test1, number = 1)
    print(t1)