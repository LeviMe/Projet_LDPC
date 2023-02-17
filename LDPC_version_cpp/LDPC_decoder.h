#include <iostream>
#include <random>
#include <algorithm>
#include <cmath>
#include <numeric>
#include <vector>

using namespace std;

class VN {
private:
 int index_;

 public:
    int* ind_CN;
    int* ind_connex;
    vector<int> ind_connex_vector;
    int degre;
    double total_value;

 public:
    VN();
    VN(int index, int ind_CN_e[], int degre);
    VN& operator=(const VN &other);
    ~VN();
    double* update(double* value_CN, double llr_value);
    void print();
    void copy_vec();
};


class CN {
 public:
	  int index_;
      int degre;
	  int* ind_VN;
      int* ind_connex;
	  vector<int> ind_connex_vector;

 public:
	  CN();
	  CN(int index, int ind_VN_e[], int degre);
	  CN& operator=(const CN& other);
	  ~CN();
	  double* update(double* value_VN);
	  void print();
	  void copy_vec();
};


class LDPC_decoder {
private:
	 int** H_matrix_; int m_; int n_;int k_; int nb_iter_;
	 double *LLR_0_;double *VN_to_CN_;double *CN_to_VN_;double *VN_to_out_;
	 VN* list_VN_; CN* list_CN_;
	 int nb_conn_;
	  void build_connection_map();
	  void init_dec_vectors();
	  void compute_LLR_0(double* y_canal, double sigma2);
	  void init_VN();
	  void update_CN();
	  void update_VN();
	  bool check_codeword();
	  void debug_print();
public:
	  LDPC_decoder(int** H_matrix, int m, int n, int nb_iter = 100);
	  LDPC_decoder();
	  LDPC_decoder(const LDPC_decoder& other);
	  int* decode(double* y_canal, double sigma2);


};

