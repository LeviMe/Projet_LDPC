#include <pthread.h>
#include <thread>// necessaire pour thread id
#include <iostream>
#include <chrono>
#include "LDPC_decoder.h"
#include "LDPC_matrices.h"
#include "encoder.h"


using namespace std;



// penser à fixer k et n pour des variables global et éviter ainsi l'usage de mémoire dynamique.
int run_trial(Encoder &E, LDPC_decoder &D, int n, int k, double sigma, double sigma2) {
	int * y, * y_coded, *y_modulated, *y_decoded;
	double *y_canal;
	y = random_vec(k);
	y_coded = E.encode(y);
	y_modulated = modulate(y_coded,n);
	y_canal = add_gaussian_noise(y_modulated, n, sigma, true);
	y_decoded = D.decode(y_canal, sigma2);
	int nb_err = nbDiff(y,y_decoded,k);

	delete[] y;
	delete[] y_coded;
	delete[] y_modulated;
	delete[] y_decoded;
	delete[] y_canal;

	return nb_err;
}

// Les structs peuvent avoir des constructeurs. A placer dans la declaration comme suit
struct results {
	vector<int> TT_failure;
	vector<int> TT_trials;
	vector<int> TT_Berror;
	vector<int> nb_completed;
	vector<double> cumul_time;

	results() {}; // default constructor
	results(int N) {
		TT_failure = std::vector<int>(N, 0);
		TT_trials = std::vector<int>(N, 0);
		TT_Berror = std::vector<int>(N, 0);
		nb_completed = std::vector<int>(N, 0);
		cumul_time = std::vector<double>(N, 0.0);
	}
	void print_line(int i, int k, int nb_process, double SNR, string filename) {
		double CwER = static_cast<double>(TT_failure[i]) / TT_trials[i];
		double BER = static_cast<double>(TT_Berror[i]) / (TT_trials[i] * k);
		vector<double> L(8);
		L ={SNR, CwER, BER, TT_trials[i], TT_failure[i],
				TT_Berror[i], cumul_time[i]/nb_process,
				cumul_time[i]/( TT_trials[i]*nb_process)};

		ofstream file(filename, ios_base::app);
		for (auto value : L) {
			cout << value <<"\t";
			file << value <<"\t";}
		cout << endl;
		file << endl;
		file.close();
	}
};

pthread_mutex_t mutex1;
int status = pthread_mutex_init(&mutex1, nullptr);


struct params {
	vector<double> range;
	double req_f;
	int m;
	int n;
	int nb_process;
	LDPC_decoder decoder;
	Encoder encoder;
	results *res;
	string filename;
};


//return type 'void*' nécessaire pour passage dans pthread_create
void* simulation_t_function(void* arg) {
	size_t id = hash<thread::id>{}(this_thread::get_id());

	//cout << "starting thread " << id%10000 << endl;
	// extraction paramètres:
	params* args = static_cast<params*>(arg);
	vector<double> Eb_N0_range = args->range;
	int nb_points = Eb_N0_range.size();
	double req_f_total = args->req_f;
	int m = args->m;
	int n = args->n;
	int k = n-m;
	int nb_process = args->nb_process;
	string filename = args-> filename;
	LDPC_decoder decoder = args->decoder;
	Encoder encoder = args->encoder;
	results *res = args-> res; /// seule variable partagée dont pointeur.

	// lancement simulation sur l'ensemble du plage de SNR
	for (int i=0; i < nb_points; i++) {

		double EB_N0_db = Eb_N0_range[i];
		double EB_N0 = pow(10, EB_N0_db / 10);
		double sigma = sqrt(1 / EB_N0);
		double sigma2 = sigma*sigma;
		int i_trial = 0;
		int residual_failures=0;
		int Berror = 0;

		auto start_time = chrono::high_resolution_clock::now();
		while (res->TT_failure[i] < req_f_total) {

			int nb_err = run_trial(encoder, decoder, n, k, sigma, sigma2);
			Berror += nb_err;
			residual_failures+=(nb_err!=0);

			if (i_trial%1000==0 and i_trial>0) {// changer
				pthread_mutex_lock(&mutex1);
				res->TT_failure[i]+= residual_failures;
				residual_failures=0;
			//	cout << "passage sec critique " << id%10000 <<"  sur p#" <<i<< " avec "<< res->TT_failure[i] <<" echecs" << endl;
				pthread_mutex_unlock(&mutex1);
			}
			i_trial++;
		}
		auto end_time = chrono::high_resolution_clock::now();
		float elapsed_time = chrono::duration_cast<chrono::microseconds>(end_time - start_time).count();
		elapsed_time/=1000000;

		pthread_mutex_lock(&mutex1);
		res->TT_failure[i]+= residual_failures;
		res->TT_trials[i] += i_trial;
		res->TT_Berror[i] += Berror;
		res->cumul_time[i] += elapsed_time;
		res->nb_completed[i] += 1;
		//cout << "adresse vecteur nb_compl " <<&(res->nb_completed) << endl;
		//cout << "point " << i <<"   "<< id%10000 << " completed: " << res->nb_completed[i] << endl;

		if (res->nb_completed[i] == nb_process) {
			res->print_line(i, k, nb_process, EB_N0_db, filename);}
		pthread_mutex_unlock(&mutex1);

	}
	return nullptr;
}


// Lance toutes les simulations
void simulation_mutex() {
	LDPC_matrices A = LDPC_matrices("m1.alist");
	int m, n, k; m = A.m;
	n = A.n; k = n-m;

	LDPC_decoder decoder = LDPC_decoder(A.H,m,n,100);
	Encoder encoder = Encoder(A.G, k, n);

	int nb_process = 8; // POUR TESTS
	int required_failures = 200;

	string filename = "results/results " + to_string(time(nullptr)) + ".dat";
	//print_1st_line(filename);

	vector<double> EB_N0_range = {1,1.5, 2, 2.5};//, 3, 3.5,4,4.5};
	int nb_points = EB_N0_range.size();

	vector<pthread_t> threads(nb_process);
	vector<params> args(nb_process);

	results *res = new results(nb_points);
	for (int i = 0; i < nb_process; i++) {
		args[i].range = EB_N0_range;
		args[i].req_f = required_failures;
		args[i].n = n;
		args[i].m = n-k;
		args[i].encoder = encoder;
		args[i].decoder = LDPC_decoder(decoder);
		args[i].res = res;
		args[i].nb_process = nb_process;
		args[i].filename = filename;
	}


	for (int i = 0; i < nb_process; i++) {
		int result = pthread_create(&threads[i], NULL, simulation_t_function, &args[i]);
		if (result != 0) {
			cout << "failed to create thread "<< i << endl;
		}
	}
	// toujours nécessaire de faire deux boucles séparées, autrement erreur:
	for (int i = 0; i < nb_process; i++) {
		pthread_join(threads[i], nullptr);
	}
}


int main() {
	simulation_mutex();
	return 0;
}

