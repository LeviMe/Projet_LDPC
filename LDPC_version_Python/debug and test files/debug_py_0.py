#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
Created on Tue Feb 21 11:47:00 2023

@author: levi
"""



import os, time

from datetime import datetime
import numpy as np
from LDPC_matrices import LDPC_matrices
from  LDPC_codec import LDPC_encoder, LDPC_decoder
from multiprocessing import Pool

np.set_printoptions(suppress=True)
clear = lambda: os.system('clear')

A = LDPC_matrices("res/m1.alist")
n,k,m = 256, 128, 128
G = A.G_array
H = A.H_array
    
encoder = LDPC_encoder(G)
decoder = LDPC_decoder(H, nb_iter=100)

def simulation_point(sigma,max_f):
    sigma2=sigma*sigma
    nb_failures, nb_Berror, i_trial = 0, 0, 0
    
    np.random.seed(os.getpid())
    index = os.getpid()%10
    while(nb_failures < max_f):
        y = np.random.randint(0,2,[k], dtype = np.uint8)        
        y_coded = encoder.encode(y)
        y_modulated = 2*y_coded-1
        noise = np.random.normal(0,sigma,[n])  
        y_canal = y_modulated + noise
        y_decoded = decoder.decode(y_canal, sigma2)
        errors = sum(y != y_decoded)
        nb_failures += (errors != 0)
        nb_Berror += errors
        i_trial+=1
        if (i_trial%100==0):
            #clear()
            print(str(index)+" "*5+str(nb_failures)+"/"+str(i_trial))
            
        print(nb_failures, index)
                                    
    return i_trial, nb_failures, nb_Berror
    
def start_processes(nb_process,req_f, EB_N0_db, filename):  
    EB_N0=10**(EB_N0_db/10)
    sigma=np.sqrt(1/(EB_N0))
    
    pool = Pool(processes=nb_process)
    map_process = [(sigma,req_f/nb_process) for i in range(1,nb_process+1)]
    
    start_time = time.time()
    results = pool.starmap(simulation_point, map_process)
    tt_trials, tt_failures, tt_Berror = np.sum(results, axis=0)
    CwER = tt_failures/tt_trials
    BER = tt_Berror/(tt_trials*k)
    dt = time.time() - start_time
    
    L = [EB_N0_db, CwER, BER, tt_trials, tt_failures, tt_Berror, dt, dt/tt_trials]
    print(L)
    with open(filename, 'a') as file:
        file.write(str(L)+"\n")
    
    pool.close()
    pool.join()
    
    
    

def simulation():
    nb_process = 8
    required_failures = 200
    EB_N0_range=np.linspace(1, 5.5, 10)
    
    filename ="results/results "+datetime.today().strftime('%m-%d %H:%M:%S')+'.dat'
    for EB_N0_db in EB_N0_range:# [4]: 
        start_processes(nb_process, required_failures, EB_N0_db, filename)   
   
if __name__=="__main__":    
    simulation()
    pass

    

def test_0():
    sigma = .7
    sigma2 = sigma*sigma
    
    for i  in range(190):
        y = np.random.randint(0,2,[k], dtype = np.uint8)        
        y_coded = encoder.encode(y)
        
        y_modulated = np.array(2*y_coded-1, dtype = np.int8)
        noise = np.random.normal(0,sigma,[n])  
        y_canal = y_modulated + noise
        y_decoded = decoder.decode(y_canal, sigma2)
        errors = sum(y != y_decoded)
        
        print(errors)
        
        
        
        
        
        
        
        
        
