#include <iostream>
#include <fstream>
#include <vector>
#include <sstream>

#include "LDPC_matrices.h"

using namespace std;

int **init_matrix(int k, int n)
{
	int **M = new int *[k];
	for (int i = 0; i < k; i++)
		M[i] = new int[n];
	return M;
}

int **transpose(int **M, int k, int n) {
	int **M_tr = init_matrix(n,k);
	for (int i = 0; i < n; i++) {
		for (int j = 0; j < k; j++) {
			M_tr[j][i] = M[i][j];
		}
	}
	return M_tr;
}

int **prod(int **M, int **N, int n) {
	int **P = init_matrix(n,n);
	for (int i = 0; i < n; i++) {
		for (int j = 0; j < n; j++) {
			P[i][j] = 0;
			for (int k = 0; k < n; k++) {
				P[i][j] ^= (M[i][k] * N[k][j]);
			}
		}
	}
	return P;
}


// Function to swap two elements in an array
void swap(int **A, int i, int j) { 
	int *temp = A[i];
	A[i] = A[j];
	A[j] = temp;
} 

// Function to calculate inverse of a matrix in GF(2)
int** inverse(int** A, int m)  { 
	int** A_copy = new int*[m];
	int** I_copy = new int*[m];
	for (int i = 0; i < m; i++) {
		A_copy[i] = new int[m];
		I_copy[i] = new int[m];
		for (int j = 0; j < m; j++) {
			A_copy[i][j] = A[i][j];
			I_copy[i][j] = (i==j);
		}
	}

	for (int i = 0; i < m; i++) {
		if (A_copy[i][i] == 0) {
			int j;
			for (j = i + 1; j < m; j++) {
				if (A_copy[j][i] != 0) break;
			}

			if (j != m) {
				swap(A_copy, i, j);
				swap(I_copy, i, j);
			}
			else {
				cout << "Matrix is not invertible \n";
				return NULL;
			}
		}

		for (int k = 0; k < m; k++)  {
			if (k != i and A_copy[k][i]==1) {
				for (int j = 0; j < m; j++) {
					A_copy[k][j] ^= A_copy[i][j];
					I_copy[k][j] ^= I_copy[i][j];
				} }
		}
	}

	return I_copy;
}

int nbDiff(int* array1, int* array2, int n) {
  int s= 0;
  for (int i = 0; i < n; i++) {
    if (array1[i] != array2[i]) s++;
  }
  return s;
}


void dispTabD(double *vec, int n) {
	for (auto i=0;i<n;i++)
		cout << vec[i] <<" ";
	cout << endl;
}
void dispTabI(int *vec, int n) {
	for (auto i=0;i<n;i++)
			cout << vec[i] <<" ";
		cout << endl;
}

void displayVec2D(vector<vector<int>> vec) {
	for (auto i = 0; i < vec.size(); i++) {
		for (auto j = 0; j < vec[i].size(); j++) {
			if (vec[i][j]==0) cout << "·";
			else cout << vec[i][j];
		}
		cout << endl;
	}
}

void print_mat(int **mat, int n, int k) {
	for(int i = 0; i < n; i++){
		for(int j = 0; j < k; j++){
			if (mat[i][j]==0) cout << "·";
			else cout << mat[i][j];
		}
		cout << endl;
	}
	cout << endl;
}

void print_mat (int **mat, int dim) {
	for(int i = 0; i < dim; i++){
		for(int j = 0; j < dim; j++){
			if (mat[i][j]==0) cout << "·";
			else cout << mat[i][j];
		}
		cout << endl;
	}
	cout << endl;
}

//Cas non-réglé: dimension pas mutliple de 4.
void print_mat_hex(int **mat, int n, int k) {
	for(int i = 0; i < n; i++){
		for(int j = 0; j < k; j+=4){
			int num = mat[i][j] * 8 + mat[i][j+1] * 4 + mat[i][j+2] * 2 + mat[i][j+3];
			if (num==0) cout << "·";
			else cout << hex << num;
			if (j%32==0) cout << " ";
		}
		cout << endl;
	}
	cout << endl;
}

//idem ici
void print_tab_hex(int *vec, int n) {
	int j;
	for( j = 0; j < n; j+=4){
		int num = vec[j] * 8 + vec[j+1] * 4 + vec[j+2] * 2 + vec[j+3];
		if (j%32==0 and j>0) cout << " ";
		if (num==0) cout << "·";
		else cout << hex << num;

	}
	cout << endl;
}



LDPC_matrices::LDPC_matrices(string matrix_file) {
	ifstream fin;
	fin.open(matrix_file);
	int i = 0;
	string line;
	int num;
	vector<int> lin;
	while (1) {
		getline(fin, line);
		lin.clear();

		std::stringstream ss(line);
		while (ss >> num)
			lin.push_back(num);

		if (i == 0) {
			m = lin[0];
			n = lin[1];
		}
		if (i > 3 && i < 4 + m)
			line_vec.push_back(lin);
		if (i >= 4 + m)
			col_vec.push_back(lin);
		if (line.empty() || fin.eof())
			break;
		i++;
	}
	fin.close();
	build_H();
	build_G();
}

void LDPC_matrices::build_H() {
	H = init_matrix(m,n);
	for (int i = 0; i < m; i++) {
		for (int j=0; j < n; j++) H[i][j] = 0;
		for (int j : line_vec[i]) H[i][j-1] = 1;
	}
}


void LDPC_matrices::build_G() {
	int **P = init_matrix(m, m);
	int **Q = init_matrix(m, m);
	int **inv_P = init_matrix(m, m);
	int **t_inv_PxQ = init_matrix(m, m);
	G  = init_matrix(m, 2*m);

	for (int i = 0; i < m; i++) {
		for (int j = 0; j < m; j++) {
			Q[i][j] = H[i][j];
			P[i][j] = H[i][j + m];
		}
	}
	inv_P = inverse(P,m);
	t_inv_PxQ = transpose(prod(inv_P,Q,m), m,m);
	for (int i = 0; i < m; i++) {
		for (int j = 0; j < m; j++){
			G[i][j] = (i==j);
			G[i][j + m] = t_inv_PxQ[i][j] ;
		}}
	// print_mat(G, m, n);

}
