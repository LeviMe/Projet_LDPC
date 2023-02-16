#include "LDPC_decoder.h"
#include "LDPC_matrices.h"
#include "encoder.h"




using namespace std;



//is the following program going to cause memory leaks
void simulation_point(double sigma, int max_f,
		LDPC_matrices &A, Encoder &E, LDPC_decoder &D) {

	double sigma2 = sigma*sigma;
	int nb_failures =0; int nb_Berror = 0; int i_trial = 0;
	int m, n; m = A.m; n = A.n; int k = n-m;

	int * y, * y_coded, *y_modulated, *y_decoded;
	double *y_canal;

	while(nb_failures < max_f) {

		y = random_vec(k);
		y_coded = E.encode(y);

		y_modulated = modulate(y_coded,n);
		y_canal = add_gaussian_noise(y_modulated, n, sigma, true);
		y_decoded = D.decode(y_canal, sigma2);

		int nb_err = nbDiff(y,y_decoded,k);
		nb_Berror+=nb_err;
		nb_failures+= (nb_err!=0);
		i_trial+=1;

		if  (i_trial%100==0) {
			cout << nb_failures << "  " << i_trial << endl;
		}
		delete[] y;
		delete[] y_coded;
		delete[] y_modulated;
		delete[] y_canal;
		delete[] y_decoded;
	}
	cout << "Exit: "<< nb_failures << "  " << i_trial << endl;
}




int main_Dernier() {

	LDPC_matrices A = LDPC_matrices("m1.alist");
	int m, n, k; m = A.m;
	n = A.n; k = n-m;

    LDPC_decoder decoder = LDPC_decoder(A.H,m,n,  100 );
	Encoder encoder = Encoder(A.G, k, n);

	double EB_N0_db = 3;
	double EB_N0 = pow(10, EB_N0_db / 10);
	double sigma = sqrt(1 / EB_N0);


	simulation_point(sigma, 2000, A, encoder, decoder);

	return 0;
}



/*
 * Test comparatif sur un code LDPC simple (4,8)
 * avec le même code en Python. Utilisé pour debugger des erreurs à
 * l'intérieur de cette implémentation
 */
int test_4_8_vs_Python() {
	LDPC_matrices A = LDPC_matrices("m2.alist");
	int m, n, k; m = A.m;
	n = A.n; k = n-m;
    LDPC_decoder decoder = LDPC_decoder(A.H,m,n,  100 );
    double y_canal[8] = {  0.911, -1.028, -0.955,  1.134,  0.881,  1.011, -0.937, -1.074};
    cout << "y_canal:    ";
    dispTabD(y_canal, 8);
    double sigma = .1;
	double sigma2= sigma*sigma;
	int *y_decoded;
	y_decoded = decoder.decode(y_canal, sigma2);
	 cout << "y_decoded:  ";
	dispTabI(y_decoded, 4);
	delete[] y_decoded;
	return 0;
}


int main_A() {
	LDPC_matrices A = LDPC_matrices("m1.alist");
	int m, n, k; m = A.m;
	n = A.n; k = n-m;

	int * y, * y_coded, *y_modulated, *y_decoded;
	double *y_canal;

    LDPC_decoder D = LDPC_decoder(A.H,m,n,  2 );
	Encoder E = Encoder(A.G, k, n);

	double EB_N0_db = 2;
	double EB_N0 = pow(10, EB_N0_db / 10);
	double sigma = sqrt(1 / EB_N0);
	double sigma2 = sigma*sigma;

	y = random_vec(k);
	y_coded = E.encode(y);

	y_modulated = modulate(y_coded,n);
	y_canal = add_gaussian_noise(y_modulated, n, sigma, true);
	y_decoded = D.decode(y_canal, sigma2);

	int nb_err = nbDiff(y,y_decoded,k);
	print_tab_hex(y, k);
	print_tab_hex(y_decoded, n);

	cout << "fin" << endl;
	return 0;
}
