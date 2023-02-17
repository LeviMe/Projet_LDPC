#include "LDPC_decoder.h"
#include "LDPC_matrices.h" // fonctions de debug, a retirer dans la version finale

bool DEBUG = false;


VN::VN() : index_(0), ind_CN(nullptr),
ind_connex(nullptr), ind_connex_vector(), degre(0), total_value(0.0) {}

VN::VN(int index, int ind_CN_e[], int degre) : index_(index), degre(degre) {
	ind_CN = new int[degre]; // Added brackets to initialize array with 0s;
	std::copy(ind_CN_e, ind_CN_e + degre, ind_CN);
	ind_connex_vector.reserve(degre);
	ind_connex = new int[degre];
	total_value = 0;

}

VN& VN::operator=(const VN &other) {
	if (this != &other) {
		index_ = other.index_;
		degre = other.degre;
		total_value = other.total_value;

		//cout << "appel VN& VN::operator=" << endl;
		delete[] ind_CN;
		ind_CN = new int[degre];
		std::copy(other.ind_CN, other.ind_CN + degre, ind_CN);

		//ind_connex_vector.clear(); // on suppose ind_connex déjà rempli
		//std::copy(other.ind_connex_vector.begin(), other.ind_connex_vector.end(),
		  //        back_inserter(ind_connex_vector));

		delete[] ind_connex;
		ind_connex = new int[degre];
		std::copy(other.ind_connex, other.ind_connex + degre, ind_connex);
	}
	return *this;
}

VN::~VN() {
	delete[] ind_CN;
	delete[] ind_connex;
}

double* VN::update(double* value_CN, double llr_value) {
	double *output;
	output = new double[degre];
	total_value = accumulate(value_CN, value_CN + degre, llr_value);
	for (int i = 0; i < degre; ++i) {
		output[i] = total_value - value_CN[i];
	}
	return output;
}

void VN::print() {
	cout << index_ << ": ";
	for (int i = 0; i < degre; i++) {
		cout << ind_CN[i] << " ";
	}
	if (0) {
		cout <<" : ";
		for (int i = 0; i < degre; i++) {
			cout << ind_connex_vector[i] << " ";
		}
	cout <<" : ";
	for (int i = 0; i < degre; i++) {
		cout << ind_connex[i] << " ";
	}}
	cout <<": "<< degre << endl;
}

void VN::copy_vec() {
	copy(ind_connex_vector.begin(), ind_connex_vector.end(), ind_connex);
}



CN::CN(): index_(0), degre(0), ind_VN(nullptr),
ind_connex(nullptr), ind_connex_vector() {}

CN::CN(int index, int ind_VN_e[], int degre) : index_(index),
		degre(degre) {
	ind_VN = new int[degre];
	ind_connex = new int[degre];
	ind_connex_vector.reserve(degre);
	std::copy(ind_VN_e,ind_VN_e+degre, ind_VN);

}

CN& CN::operator=(const CN& other) {
	if (this != &other) {
		index_ = other.index_;
		degre = other.degre;

		delete[] ind_VN;
		delete[] ind_connex;

		ind_VN = new int[degre];
		ind_connex = new int[degre];

		for (int i = 0; i < degre; ++i) {
			ind_VN[i] = other.ind_VN[i];
			ind_connex[i] = other.ind_connex[i];
		}
	//	std::copy(other.ind_connex_vector.begin(), // on suppose ind_connex déjà rempli, pas besoin de faire cette copie.
		//		other.ind_connex_vector.end(), back_inserter(ind_connex_vector));
	}
return *this;
}

CN::~CN() {
	delete[] ind_VN;
	delete[] ind_connex;
}

template <typename T>
int sign(T val) {
	return (T(0) < val) - (val < T(0));
}

double* CN::update(double* value_VN) {
	double *output;
	output =new double[degre];
	for (int i = 0; i < degre; ++i) {
		double min_val = numeric_limits<double>::max();
		double product = 1.0f;
		for (int j = 0; j < degre; ++j) {
			if (i != j) {
				product *= value_VN[j];
				min_val = min(min_val, abs(value_VN[j]));
			}
		}
		output[i] = sign(product) * 0.7f * min_val;
	}
	return output;
}

void CN::print() {
	cout << index_ << ": ";
	for (int i = 0; i < degre; i++) {
		cout << ind_VN[i] << " ";
	}
	if (0) {
	cout <<" : ";
	for (int i = 0; i < degre; i++) {
		cout << ind_connex_vector[i] << " ";
	}}
	cout <<" : ";
	for (int i = 0; i < degre; i++) {
		cout << ind_connex[i] << " ";
	}
	cout <<" : "<< degre << endl;
}

void CN::copy_vec() {
	ind_connex = ind_connex_vector.data();
}



LDPC_decoder::LDPC_decoder(int** H_matrix, int m, int n, int nb_iter)
: H_matrix_(H_matrix), m_(m), n_(n), nb_iter_(nb_iter) {
	k_ = n_ - m_;
	list_VN_ = new VN[n];
	list_CN_ = new CN[m];
	vector<vector<int>> line_pos(k_);
	vector<vector<int>> col_pos(n_);

	for (int i = 0; i < k_; ++i) {
		line_pos[i] = vector<int>();
		for (int j = 0; j < n_; ++j) {
			if (H_matrix_[i][j] == 1)  line_pos[i].push_back(j);
		}
	}
	for (int i = 0; i < n_; ++i) {
		col_pos[i] = vector<int>();
		for (int j = 0; j < m_; ++j) {
			if (H_matrix_[j][i] == 1) col_pos[i].push_back(j);
		}
	}

	for (int i_VN = 0; i_VN < n_; ++i_VN) {
		VN VN_i = VN(i_VN, col_pos[i_VN].data(), col_pos[i_VN].size());
		list_VN_[i_VN] =VN_i; // ne change rien de faire directement le passage:
	}
	for (int i_CN = 0; i_CN < m_; ++i_CN) {
		CN CN_i =CN(i_CN, line_pos[i_CN].data(), line_pos[i_CN].size());
		list_CN_[i_CN] = CN_i;
	}
	build_connection_map();
	init_dec_vectors();

	if (0) {
		for (int i_VN = 0; i_VN < n_; ++i_VN) list_VN_[i_VN].print();
		for (int i_CN = 0; i_CN < m_; ++i_CN) list_CN_[i_CN].print();}
}


LDPC_decoder::LDPC_decoder() : H_matrix_(nullptr), m_(0), n_(0), k_(0), nb_iter_(0),
							   LLR_0_(nullptr), VN_to_CN_(nullptr), CN_to_VN_(nullptr), VN_to_out_(nullptr),
							   list_VN_(nullptr), list_CN_(nullptr), nb_conn_(0) {}

LDPC_decoder::LDPC_decoder(const LDPC_decoder& other) :
		m_(other.m_), n_(other.n_), k_(other.k_), nb_iter_(other.nb_iter_),
					nb_conn_(other.nb_conn_) {

	H_matrix_ = init_matrix(m_,n_);
	for (int i = 0; i < m_; i++) {
		for (int j = 0; j < n_; j++) {
			H_matrix_[i][j] = other.H_matrix_[i][j];
		}}
	init_dec_vectors();
	list_VN_ = new VN[n_];
	list_CN_ = new CN[m_];

	for (int i = 0; i < n_; i++) {
		list_VN_[i] = other.list_VN_[i];
		LLR_0_[i] = other.LLR_0_[i];
		VN_to_out_[i] = other.VN_to_out_[i];
	}
	for (int i = 0; i < nb_conn_; i++) {
		VN_to_CN_[i] = other.VN_to_CN_[i];
		CN_to_VN_[i] = other.CN_to_VN_[i];
	}
	for (int i = 0; i < m_; i++) {
		list_CN_[i] = other.list_CN_[i];
	}
}


void LDPC_decoder::build_connection_map(){
	int i_conn = 0;
	for (int i_VN = 0; i_VN < n_; ++i_VN) {
		int degre = list_VN_[i_VN].degre;
		for (int i = 0; i < degre; i++) {
			int i_CN = list_VN_[i_VN].ind_CN[i];
			list_VN_[i_VN].ind_connex_vector.push_back(i_conn);
			list_CN_[i_CN].ind_connex_vector.push_back(i_conn);
			i_conn++;
		}
	}
	for (int i_CN = 0; i_CN < m_; ++i_CN) list_CN_[i_CN].copy_vec();
	for (int i_VN = 0; i_VN < n_; ++i_VN) list_VN_[i_VN].copy_vec();
	nb_conn_ = i_conn;
}

void LDPC_decoder::init_dec_vectors() {
	LLR_0_ = new double[n_];
	VN_to_CN_ = new double[nb_conn_];
	CN_to_VN_ = new double[nb_conn_];
	VN_to_out_ = new double[n_];
}

void LDPC_decoder::compute_LLR_0(double* y_canal, double sigma2) {
	for (int i_VN = 0; i_VN < n_; ++i_VN) {
		LLR_0_[i_VN] = 2 * y_canal[i_VN] / sigma2;
	}}

void LDPC_decoder::debug_print() {
	for (int i_VN = 0; i_VN < 5; ++i_VN)
		list_VN_[i_VN].print();
}

void LDPC_decoder::init_VN() {
	for (int i_VN = 0; i_VN < n_; ++i_VN) {
		for (int i_connex = 0; i_connex < list_VN_[i_VN].degre; ++i_connex) {
			VN_to_CN_[list_VN_[i_VN].ind_connex[i_connex]] = LLR_0_[i_VN];
		}}
}

void LDPC_decoder::update_CN() {
	double *inputs, *outputs;
	for (int i_CN = 0; i_CN < m_; ++i_CN) {

		CN &CN_i = list_CN_[i_CN];
		inputs = new double[CN_i.degre];
		for (int i_connex = 0; i_connex < CN_i.degre; ++i_connex) {
			inputs[i_connex] = VN_to_CN_[CN_i.ind_connex[i_connex]];
		}
		outputs = CN_i.update(inputs);
		for (int i_connex = 0; i_connex < CN_i.degre; ++i_connex) {
			CN_to_VN_[CN_i.ind_connex[i_connex]] = outputs[i_connex];
	}
	delete[] inputs;
	delete[] outputs;
	}}




void LDPC_decoder::update_VN(){
	for (int i_VN = 0; i_VN < n_ ;i_VN++) {

		VN &VN_i = list_VN_[i_VN];
		double* inputs = new double[VN_i.degre];
		for (int i = 0; i < VN_i.degre; ++i) {
			inputs[i] = CN_to_VN_[VN_i.ind_connex[i]];
		}
		double* outputs = VN_i.update(inputs, LLR_0_[i_VN]);

		VN_to_out_[i_VN] = VN_i.total_value;
		for (int i = 0; i < VN_i.degre; ++i) {
			VN_to_CN_[VN_i.ind_connex[i]] =  outputs[i];
		}
		delete[] inputs;
		delete[] outputs;
	}
}


bool LDPC_decoder::check_codeword() {
	int vec[n_];
	for (int i = 0; i < n_; i++) {
		vec[i] = (VN_to_out_[i] > 0) * 1;
	}

	int spt[m_];
	for (int i = 0; i < m_; i++) {
		spt[i] = 0;
		for (int j = 0; j < n_; j++) {
			spt[i] ^= (H_matrix_[i][j] & vec[j]);
		}
	}
	int sum = 0;
	for (int i = 0; i < m_; i++) sum += spt[i];
	return sum == 0;
}

int* LDPC_decoder::decode(double* y_canal, double sigma2) {
	compute_LLR_0(y_canal, sigma2);
	init_VN();

	if (DEBUG) {cout << "LLR0:    	 "; dispTabD(this->LLR_0_, 8);
				cout << "init_VN:    "; dispTabD(this->VN_to_CN_, 15);}

	for (int i_iter = 0; i_iter < nb_iter_; i_iter++) {
		update_CN();
		update_VN();
		if (check_codeword()) break;

		if (DEBUG) {cout <<"iteration" << i_iter << endl;
					cout << "CN_to_VN:     ";dispTabD(this->CN_to_VN_, 15);
					cout << "VN_to_CN:     ";dispTabD(this->VN_to_CN_, 15);
					cout << "VN_to_out:    ";dispTabD(this->VN_to_out_, 8);}
	}

	int *decoded_cw;
	decoded_cw = new int[k_];
	for (int i = 0; i < k_; i++) {
		decoded_cw[i] = (VN_to_out_[i] > 0) * 1;
	}

	return decoded_cw;
}

