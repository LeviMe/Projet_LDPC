# -*- coding: utf-8 -*-
import numpy as np, cv2

''''
n bits_
m taille encodé. 
k = m-n bits de parités.

Distinguer matrice de parité et d'encodage. Prendre un encodage simple systématique. 
encodage: matrice "tour" m * n. Parité matrice 'bloc immeuble' k*m
'''
def swap(A, i, j):
    Lj, Li = A[j].copy(), A[i].copy()
    A[i],A[j] = Lj, Li
            
def inverse(A):
    assert(A.shape[0] == A.shape[1])
    m = A.shape[0]
    A_copy = A.copy()*1
    I_copy = np.eye(m, dtype = np.uint16)
    for i in range(m):
        if (A_copy[i,i]==0):
            trouve = False
            for j in range(i+1,m):
                if (A_copy[j,i]!=0):
                    swap(A_copy, i, j)
                    swap(I_copy, i, j)
                    trouve = True
                    break
            if (not trouve):
                print("non inversible")
        for j in range(m):
            if (i!=j and A_copy[j,i]!=0):
                A_copy[j] ^= A_copy[i]
                I_copy[j] ^= I_copy[i]
                        
    assert(np.array_equal(A_copy, np.eye(m))) # verif id
    pr = np.dot(I_copy*1, A)%2
    assert(np.array_equal(pr, np.eye(m))) # verif inverse
    return I_copy 


class LDPC_matrices():
    def __init__(self, matrix_file):
    
        self.col_vec, self.line_vec = [], []
        i=0
        with open(matrix_file, "r") as f:
            while(1):
                line = f.readline().split()
                line = [int(a) for a in line]
                if (len(line)==0): break
 
                if  (i==0): self.m,self.n = line
                if (i>3 and i<4+self.m): self.line_vec+=[line]
                if (i>=4+self.m): self.col_vec +=[line]
                i+=1
                
        self.build_H()
    
        self.build_G()
            
        
    def build_H(self):
        # indexation Alist à partir de 1
        self.H_array  = np.zeros([self.m, self.n],dtype=np.uint16)
        for i in range(self.m):
            for j in self.line_vec[i]:
                self.H_array[i,j-1] = 1
                
    def prod_H_matrix(self,vec):
        assert(len(vec)==self.n)
        return (self.H_array.dot(vec))%2
    
    def prod_H_sparse(self, vec):
        assert(len(vec)==self.n)
        res = [0]*self.m
        for i in range(self.m):
            for j in self.line_vec[i]:
                res[i] ^=  vec[j-1]
                
        return np.array(res)
    
# rester sur un code 1/2
# voir si la méthode d'encodage fonctionne pour d'autre rendement
#G = [Ik | ^t(P⁻1 Q)] avec H=[Q|P] Q étant une matrice carrée. 
#calcul inverse dans GF(2)

    def build_G(self):
        Q = self.H_array[:, :self.m]
        P = self.H_array[:,self.n-self.m:]
        inv_P = inverse(P)
        G1 = np.eye(self.m)
        G2 = np.transpose(np.dot(inv_P,Q)%2)
        G = np.concatenate([G1,G2],axis=1)
        
        G_prod_H = np.dot(G,np.transpose(self.H_array))%2
        assert(np.array_equiv(G_prod_H, np.zeros([self.m,self.m]))) 
        self.G_array = np.array(G, dtype = np.uint16)
        
          
def test_prod():
    v = [1,0,1,1,0,1,1,1,0,0,1,0,1,0,0,0]
    print(A.prod_H_matrix(v))
    print(A.prod_H_sparse(v))

def disp_array_img(A):
    a,b = 4*np.array(A.shape)
    A = np.array(255*(1-A), dtype = np.uint8)
    img = cv2.resize(A, (b,a),\
        interpolation=cv2.INTER_AREA)
    cv2.imshow("matrice", img )   
    cv2.waitKey(0) 
    cv2.destroyAllWindows() 
    cv2.imwrite("matrix.png", img)
    
# Verifier que la matrice G a les mêmes propriétés que
# celle décrites dans la table 2-2 du document. 
# 4 lignes principales, le reste est obtenu par circulation de celles-ci
# caractère circulant de ces 16 sous-matrices observables avec l'affichage image
# Etape validée.
def disp_array_hexa(G):
    G2 = G[:, 128:]
    L = [G2[0], G2[32], G2[64], G2[96]]
    
    for line in L:
        size = len(line) # 128
        s=""
        for i in range(4):
            word = line[i*size//4: (i+1)*size//4]
            word_d_vec = np.resize(word,(size//16, 4))
            
            for digit in word_d_vec:
                s+=hex(int(np.dot(digit,[8,4,2,1])))[2]
           # s+="  "
        print(s)
        
        

def disp_transp_G2_hexa(G):
    G2 = G[:, 128:].transpose()
    L = [G2[0], G2[32], G2[64], G2[96]]
    
    for line in L:
        size = len(line) # 128
        s=""
        for i in range(4):
            word = line[i*size//4: (i+1)*size//4]
            word_d_vec = np.resize(word,(size//16, 4))
            
            for digit in word_d_vec:
                s+=hex(int(np.dot(digit,[8,4,2,1])))[2]
           # s+="  "
        print(s)

        
def disp_G2_hexa_full(G):
    G2 = G[:, 128:]
    size = 128
    for i_ligne in range(size):
        line = G2[i_ligne]
        s=""
        for i in range(4):
            word = line[i*size//4: (i+1)*size//4]
            word_d_vec = np.resize(word,(size//16, 4))
            
            for digit in word_d_vec:
                s+=hex(int(np.dot(digit,[8,4,2,1])))[2]
            s+=" "
        print(s)
        

def disp_H(H):
    n,p = np.shape(H)
    for i in range(n):
        s=""
        for j in range(p):
            s+=str(H[i,j])
        print(s)
        

    

if __name__ == "__main__":    
    test1 = True
    if test1:
        
        A = LDPC_matrices("res/m1.alist")
        
        
        P = A.H_array[:,128:]
      
        inv_P = inverse(P)
        
        disp_H(inv_P)
       # disp_transp_G2_hexa(A.G_array)
      #  disp_array_img(A.H_array)
      #  disp_array_img(A.G_array[:,128:])
        #disp_G2_hexa_full(A.G_array)
        
      
        