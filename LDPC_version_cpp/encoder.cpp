#include "LDPC_matrices.h"
#include "encoder.h"
#include <iostream>
#include <random>

int* random_vec(int n) {
  std::random_device rd;
  std::mt19937 gen(rd());
  std::uniform_int_distribution<> dis(0,1);

  int *vec = new int[n];
  for (int i = 0; i < n; i++) {
    vec[i] = dis(gen);
  }
 return vec; 
}

int* modulate(int *vec, int n) {
    int *res = new int[n];
    for (int i = 0; i < n ; i++) {
      res[i] = 2 * vec[i] - 1;
    }
   return res;
}

double* add_gaussian_noise(int* vec, int n,double sigma, bool set) {

  random_device rd;
  mt19937 gen(rd());
  normal_distribution<> d(0, sigma);

  double* res = new double[n];
  for (int i = 0; i < n; i++) {
    res[i] = vec[i] + set * d(gen);
  }
  return res;
}


Encoder::Encoder(int **G,int k, int n) {
	G_matrix = init_matrix(k,n);
	G_matrix = G;
	this->k = k; this-> n = n;
}

Encoder::Encoder() {
    G_matrix = nullptr;
    k = 0;n = 0;
}

int* Encoder::encode(int* vec) {
	int *res = new int[n];
	for (int i=0;i<k;i++) res[i] = vec[i];
	for (int i=k;i<n;i++) {
	int sum = 0;
		for (int j=0;j<k; j++)
		sum ^= (vec[j] & G_matrix[j][i]);
		res[i] = sum;
	}
	return res;
}

