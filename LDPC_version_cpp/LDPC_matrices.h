
#ifndef LDPC_MATRICES
#define LDPC_MATRICES


#include <iostream>
#include <fstream>
#include <vector>
#include <sstream>

using namespace std;

int **init_matrix(int k, int n);
int **transpose(int **M, int k, int n) ;
int **prod(int **M, int **N, int n);
void swap(int **A, int i, int j) ;
int** inverse(int** A, int m)  ;
int nbDiff(int* array1, int* array2, int n);
void dispTabD(double *vec, int n);
void dispTabI(int *vec, int n);
void displayVec2D(vector<vector<int>> vec) ;
void print_mat(int **mat, int n, int k);  
void print_mat(int **mat, int dim);
void print_mat_hex(int **mat, int n, int k);
void print_tab_hex(int *vec, int n);
    
class LDPC_matrices
{
public:
    vector<vector<int>> col_vec, line_vec;
    int** H;
    int** G;
    int m, n;

public:
    LDPC_matrices(string matrix_file);
    void build_H();
    void build_G() ;    
}; 


#endif
