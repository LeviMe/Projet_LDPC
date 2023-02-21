# -*- coding: utf-8 -*-

import numpy as np
import operator

DEBUG = False

class LDPC_encoder:
    def __init__(self, G_matrix):
        self.G_matrix = G_matrix
        self.k, self.n = G_matrix.shape
                     
    def encode(self,vec):
        res = vec.dot(self.G_matrix)%2
        return np.array(res, dtype = np.int8)  # nécessaire int8 pour modulation

class VN():
    def __init__(self, index, ind_CN):
        self.index = index
        self.ind_CN = ind_CN
        self.degre = len(ind_CN)
        self.ind_connex = []
        self.output = np.zeros([self.degre])
        
    def update(self, value_CN, llr_value):
        self.total_value = np.sum(value_CN) + llr_value
        for i in range(self.degre):
            self.output[i] = self.total_value - value_CN[i]   
        return self.output
    
                           
class CN():
    def __init__(self, index, ind_VN):
        self.index = index
        self.ind_VN = ind_VN
        self.degre = len(ind_VN) 
        self.ind_connex = []
        self.output = np.zeros([self.degre])
        
    def update(self, value_VN):
        for i in range(self.degre):
            tt = np.delete(value_VN,i)
            #res+=[2*np.arctanh(np.prod(np.tanh(tt/2)))]  
            self.output[i] = np.prod(np.sign(tt)) * .7*np.min(np.abs(tt))
        return self.output
        
class LDPC_decoder():
    def __init__(self, H_matrix, nb_iter = 100):
        self.list_VN, self.list_CN = [], []
        self.H_matrix = H_matrix
        self.m, self.n = H_matrix.shape
        self.k = self.n - self.m
        self.nb_iter = nb_iter
        
        line_pos = [np.where(H_matrix[i]==1)[0] \
                    for i in range(self.k)]
        col_pos = [np.where(np.transpose(H_matrix)[i]==1)[0] \
                   for i in range(self.n)] 
       
        for i_VN in range(self.n):
            VN_i = VN(i_VN, col_pos[i_VN])
            self.list_VN += [VN_i]
            
        for i_CN in range(self.m):
            CN_i = CN(i_CN, line_pos[i_CN])
            self.list_CN += [CN_i]
        
        self.build_connection_map()
        self.init_dec_vectors()
            
    def build_connection_map(self):
        connections = []
        i_conn = 0
        for i_VN in range(self.n): 
            CN_connexes = self.list_VN[i_VN].ind_CN
            for i_CN in CN_connexes:    
                connections += [[i_VN,i_CN, i_conn]]
                self.list_VN[i_VN].ind_connex += [i_conn]
                i_conn += 1
                
        connections.sort(key = operator.itemgetter(1, 0))   
        for cnx in connections:
            i_CN, i_conn = cnx[1], cnx[2]
            self.list_CN[i_CN].ind_connex += [i_conn]
        self.nb_conn = len(connections)
        
    def init_dec_vectors(self):
        self.LLR_0 = np.zeros([self.n])
        self.VN_to_CN = np.zeros([self.nb_conn]) 
        self.CN_to_VN = np.zeros([self.nb_conn])
        self.VN_to_out = np.zeros([self.n])
    
    def compute_LLR_0(self,y_canal, sigma2):
        for i_VN in range(self.n):
            self.LLR_0[i_VN] = 2*y_canal[i_VN]/sigma2
        
        if (DEBUG):
            print("LLR 0")
            print(self.LLR_0)
    
    def init_VN(self):
        for i_VN in range(self.n):            
            for i_connex in self.list_VN[i_VN].ind_connex:
                self.VN_to_CN[i_connex] = self.LLR_0[i_VN]
        if (DEBUG):
            print("VN_to_CN init")
            print(self.VN_to_CN)
        
    def update_CN(self):
        for  i_CN in range(self.m):
            CN = self.list_CN[i_CN]
            i_connex = CN.ind_connex
            inputs = [self.VN_to_CN[i] for i in i_connex]
            outputs = CN.update(inputs)
            for i in range(CN.degre):
                self.CN_to_VN[i_connex[i]] = outputs[i]
        if (DEBUG):
            print("CN to VN")
            print(self.CN_to_VN)
            
   
    def update_VN(self):
        for i_VN in range(self.n):
            VN = self.list_VN[i_VN]
            i_connex = VN.ind_connex
            inputs = [self.CN_to_VN[i] for i in i_connex]
            outputs = VN.update(inputs, self.LLR_0[i_VN])
            self.VN_to_out[i_VN] = VN.total_value
            for i in range(VN.degre):
                self.VN_to_CN[i_connex[i]] = outputs[i]
        if (DEBUG):
            print("VN to CN")
            print(self.VN_to_CN)
            print("VN to out")
            print(self.VN_to_out)
                
    def check_codeword(self):
        vec = (self.VN_to_out>0)*1
        spt = (self.H_matrix.dot(vec))%2
        return  np.sum(spt) == 0         
    
    def decode(self, y_canal, sigma2):
        self.compute_LLR_0(y_canal, sigma2)
        self.init_VN()
    
        for i_iter in range(self.nb_iter):
            self.update_CN()
            self.update_VN() 
            if (self.check_codeword()): break
            
        decoded_cw = (self.VN_to_out>0)*1 
        return decoded_cw[0:self.k]
        