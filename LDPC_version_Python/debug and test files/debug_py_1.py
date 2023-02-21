#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
Created on Tue Feb 21 11:47:00 2023

@author: levi

Objectif: calculer la vitesse relative des différents types entiers de numpy pour les tâches
du programme LDPC.
"""



import os, time

import numpy as np

from multiprocessing import Pool

np.set_printoptions(suppress=True)
clear = lambda: os.system('clear')


# générer 1 million de vec aléatoire
#np.uint8 : 31.21228861808777
#np.int8 : 30.34966731071472
#np.int64 (défaut) : 35.02563977241516



#générer 200k vec aléatoires + produit matrice + modulo 2
#int8: 26.217875480651855
#uint8: 27.32948398590088
    
#uint8 + recast int8 : 26.5369930267334
#int8 + recast uint8 : 29.260288953781128
#uint64 + recast int8 : 27.42381763458252

# répéter le même produit matrice vecteur 200k fois
#int64 + recast int8 : 20.536830186843872
#int64 sans recast : 20.99481225013733

G = np.random.randint(0,2,[128,256])
y = np.random.randint(0,2,[128],np.int64)

def simulation_point(N):
    for i in range(N):
        v = y.dot(G)%2 # np.array(y.dot(G)%2, dtype = np.int8)
    
def start_processes(nb_process,N):  
    pool = Pool(processes=nb_process)
    map_process = [(N,) for _ in range(nb_process)]
    
    start_time = time.time()
    pool.starmap(simulation_point, map_process)
    dt = time.time() - start_time
    print(dt)
    
    pool.close()
    pool.join()


   
if __name__=="__main__":    
    start_processes(8,200000)
    