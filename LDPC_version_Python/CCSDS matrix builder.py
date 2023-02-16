#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
Created on Sat Oct 22 21:56:33 2022

@author: levi
"""

import numpy as np
import cv2

# générer la matrice 128, 256 (page 15)

M = 128//4

#permutation circulaire à droite de l'identité
def phi(index):
    I = np.eye(M)
    Ph = np.zeros([M,M])
    for i in range(M):
        Ph[:,(i+index)%M] = I[:,i]
    return Ph

# la matrice de parité du code 128,256. Pour les autres refaire le même travail. 
def generate_H():
    I = np.eye(32)
    O = np.zeros([32,32])
    B = [[(I+phi(31))%2, phi(15),            phi(25), phi(0),  O,    phi(20),    phi(12),    I],
         [phi(28),       (I+phi(30))%2,      phi(29), phi(24), I,    O,          phi(1),     phi(20)],
         [phi(8),        phi(0),         (I+phi(28))%2, phi(1), phi(29), I,      O,      phi(21)], 
         [phi(18),       phi(30),         phi(0),   (I+phi(30))%2, phi(25), phi(26), I, O]]
    
    
    H = np.zeros([128,256], dtype = np.uint8)
    for i in range(4):
        assert(len(B[i])==8)
        for j in range(8):
            H[ 32*i: 32*(i+1), 32*j:32*(j+1)] = B[i][j]
    return H
 
#tenir compte du fait que les indices commence à 1 dans la Alist
def build_A_list(H):
    def to_string(L):
        s =""
        for l in L:
            s+=str(l)+" "
        return s[:-1]+"\n"
    
    s =""
    m, n = H.shape
    s+=to_string(H.shape)
    
    d_ligne, d_colonnes = np.sum(H, axis=1), np.sum(H, axis=0)    
    d0, d1 = d_ligne[0], d_colonnes[1]
    s+=str(d0)+" "+str(d1)+"\n"
    
    s+=to_string(d_ligne)
    s+=to_string(d_colonnes)    
    
    for i in range(m):
        s+=to_string(np.where(H[i]==1)[0] + 1)
    for i in range(n):
        s+=to_string(np.where(H[:,i]==1)[0] + 1)
        
    s=s[:-1]
    f = open('m1.alist','w')
    f.write(s)
    f.close()
    

H = generate_H()    
build_A_list(H)


# cv2.imshow("matrice", cv2.resize(H, [256*4, 128*4]))   
# cv2.waitKey(0) 
# cv2.destroyAllWindows()  


    