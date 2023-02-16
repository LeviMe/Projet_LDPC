#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
Created on Thu Nov 17 11:35:08 2022

@author: levi

Reproduire produit matricielle pour exporter en VHDL.
Matrice systématique soit. 
    produit par une matrice carré circulante.  + terme d'indice i.
    
    La concatenation de matrices circulantes n'est pas circulante à cause des termes limites.
    
produit à droite pour la matrice génératrice. 
    = produit à gauche par sa transposé. 
        la transposé d'une matrice circulante est circulante. Pas de problème.
        4 vecteurs à connaitre. Pas ceux du doc du CCDS car transposé.
"""
import numpy as np

from LDPC_matrices import LDPC_matrices
from  LDPC_codec import LDPC_encoder

A = LDPC_matrices("res/m1.alist")
G = A.G_array
H = A.H_array
encoder = LDPC_encoder(G)

BASE = 128

def to_integer(x_vec):
    return sum([x_vec[i]*(1<<(BASE-1-i)) for i in range(BASE)])


def to_integer_BE(x_vec):
    s=0
    for i in range(len(x_vec)):
        s+=int(x_vec[i]*(1<<i))
    return s
    
def to_vec(L):
    res = []
    for i in range(BASE-1,-1,-1):
        bi = (L & (1<<i))>>i
        res += [bi]
    return res
    
def sum_bits(x):
    res = 0
    for i in range(BASE): res^=( (x & (1<<i))>>i)
    return res
    
def circular_perm(L, index):
    res = 0
    for i in range(BASE):
        bi = (L & (1<<i))>>i
        exp = 1<<((i-index)%BASE)
        res += (bi*exp)
    return res

def circular_perm4(L, index, base = BASE):  
    base2 = base//4
    mask = (1 << base2) - 1
    res = 0
    for i in range(4):
        mot = (L & mask)>>(i*base2)
        mask <<= base2

        mot_per = 0
        for k in range(base2):
            bk = (mot & (1<<k))>>k
            mot_per|= bk<<((k-index)%base2)
        res|= (mot_per << (i*base2))            

    return res


def prod_vhdl(x_vec):     
    L0 = 0x4e0bd7e781a409ae377e0fdfa66f62ae
    L32 =0x45398220cf61259e8e197fd251c66983
    L64 =0x4b8b5bb60a4db0f1b9581b38c62b3785
    L96 =0xa346bce71d48482d2476bd209ec457af
    
    assert(len(x_vec)==BASE)
    x = to_integer(x_vec)
    
    y1 = np.zeros([BASE],  dtype = np.uint8)
    for i in range(BASE//4):
        y1[i] =    sum_bits(x & circular_perm4(L0,  i))
        y1[i+32] = sum_bits(x & circular_perm4(L32, i))
        y1[i+64] = sum_bits(x & circular_perm4(L64, i))
        y1[i+96] = sum_bits(x & circular_perm4(L96, i))
    
    y_vec = np.concatenate((x_vec, y1))
    return y_vec 


# for i in range(10):    
#     print( hex(to_integer_BE(G[:,128+i])) )
    
y2 = encoder.encode(G[:,128+96])
print(hex(to_integer_BE(y2)))


# for i_trial in range(100):    
#     x_vec = np.random.randint(0,2,[BASE], dtype = np.uint8) 

#     y1 = prod_vhdl(x_vec)
#     y2 = encoder.encode(x_vec)
#     assert(np.array_equal(y1, y2))
    
# print("test completed successfully")
