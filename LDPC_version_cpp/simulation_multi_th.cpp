#include <pthread.h>
#include <thread>
#include <iostream>
#include <chrono>


#include "LDPC_decoder.h"
#include "LDPC_matrices.h"
#include "encoder.h"


using namespace std;


// Structure to hold the arguments for each thread
struct ThreadArgs {
  double sigma;
  double req_f;
  int m;
  int n;
  LDPC_decoder decoder;
  Encoder encoder;
};


// Fonction d'un thread qui prend un 8eme de la charge, pour atteindre un 8eme des echecs nécessaires.
void* run_simulation_point(void* arg) {
  ThreadArgs* args = static_cast<ThreadArgs*>(arg);
  double sigma = args->sigma;
  double req_f = args->req_f;
  int m = args->m; int n = args->n; int k=n-m;
  
  LDPC_decoder D = LDPC_decoder(args->decoder); // PLACER UN COPY CONSTRUCTOR SUR CES DEUX ARGUMENTS
  Encoder E = args->encoder;

  double sigma2 = sigma*sigma;
  int nb_failures =0; int nb_Berror = 0; int i_trial = 0;
  
  	int * y, * y_coded, *y_modulated, *y_decoded;
	double *y_canal;

	while(nb_failures < req_f) {

		y = random_vec(k);
		y_coded = E.encode(y);
		y_modulated = modulate(y_coded,n);
		y_canal = add_gaussian_noise(y_modulated, n, sigma, true);
		y_decoded = D.decode(y_canal, sigma2);

		int nb_err = nbDiff(y,y_decoded,k);
		nb_Berror+=nb_err;
		nb_failures+= (nb_err!=0);
		i_trial+=1;

		size_t id = hash<thread::id>{}(this_thread::get_id());

		if  (i_trial%100000==0) {
			cout << id%10000 <<":  "<< nb_failures << "  " << i_trial << endl;
		}
		delete[] y; delete[] y_coded; delete[] y_modulated;
		delete[] y_canal; delete[] y_decoded;
	}
  
  int* result = new int[3];
  result[0] = i_trial;
  result[1] = nb_failures;
  result[2] = nb_Berror;
  return result;
}


// Lance 8 threads parralèles sur un point de SNR
void start_processes(int nb_process, int required_failures, double EB_N0_db,
		const string& filename, Encoder &encoder, LDPC_decoder &decoder, int n, int k) {
  double EB_N0 = pow(10, EB_N0_db / 10);
  double sigma = sqrt(1 / EB_N0);

  vector<pthread_t> threads(nb_process);
  vector<ThreadArgs> args(nb_process);

  for (int i = 0; i < nb_process; i++) {
    args[i].sigma = sigma;
    args[i].req_f = required_failures / nb_process;
    args[i].n = n;
    args[i].m = n-k;
    args[i].encoder = encoder;
    args[i].decoder = decoder;
  }

  auto start_time = chrono::high_resolution_clock::now();

  for (int i = 0; i < nb_process; i++) {
    int result = pthread_create(&threads[i], NULL, run_simulation_point, &args[i]);
    if (result != 0) {
      cout << "echec creation thread "<< i << endl;
    }
  }
  // Wait for all threads to finish
  double tt_trials = 0;
  double tt_failures = 0;
  double tt_Berror = 0;
  for (int i = 0; i < nb_process; i++) {
    int* result;
    pthread_join(threads[i], (void**)&result);
    tt_trials += result[0];
    tt_failures += result[1];
    tt_Berror += result[2];
    delete[] result;
  }

  auto end_time = chrono::high_resolution_clock::now();
  float elapsed_time = chrono::duration_cast<chrono::microseconds>(end_time - start_time).count();
  elapsed_time/=1000000;
  double CwER = static_cast<double>(tt_failures) / tt_trials;
  double BER = static_cast<double>(tt_Berror) / (tt_trials * k);
  vector<double> L(8);
  L ={EB_N0_db, CwER, BER, tt_trials, tt_failures,
   tt_Berror, elapsed_time, elapsed_time / tt_trials};

  ofstream file(filename, ios_base::app);
  for (auto value : L) {
	  cout << value <<"\t";
	  file << value <<"\t";}
  cout << endl;
  file << endl;
  file.close();
}


void print_1st_line(string filename) {
	  ofstream file(filename, ios_base::app); // Le dossier de destination doit exister pour eviter une erreur
	  if (!file) cout << "failed to open file" << endl;

	  vector<string> L(8);
	  L ={"EB_N0_db", "CwER", "BER", "tt_trials", "tt_failures",
	   "tt_Berror", "elapsed_time", "avg_trial_time"};
	  for (auto value:L) {
		  cout << value <<"\t"; file << value <<"\t";}
	  	  cout << endl; file << endl;
	  file.close();
}

// Lance toutes les simulations
void simulation() {
  LDPC_matrices A = LDPC_matrices("m1.alist");
  int m, n, k; m = A.m;
  n = A.n; k = n-m;

  LDPC_decoder decoder = LDPC_decoder(A.H,m,n,100);
  Encoder encoder = Encoder(A.G, k, n);

  int nb_process = 8;
  int required_failures = 200;
  
  string filename = "results/results " + to_string(time(nullptr)) + ".dat";
  print_1st_line(filename);

  vector<double> EB_N0_range = {1,1.5, 2, 2.5, 3, 3.5,4,4.5};
  for (double EB_N0_db : EB_N0_range) {
	//  cout <<"_________demarage simulation sur "<<EB_N0_db<<" dB__________"<< endl;
    start_processes(nb_process, required_failures, EB_N0_db, filename, encoder, decoder, n,k);
  }
}


/**
int main() {
	simulation();
return 0;
}**/


/**
1. Add a copy constructor for the ThreadArgs struct to prevent shallow copies of the LDPC_decoder and Encoder objects.
2. Replace the raw pointers for storing data with std::unique_ptr or std::shared_ptr.
3. Replace the pthread library with the C++11 thread library.
5. Change the variable names to be more descriptive and consistent.
6. Add appropriate comments for clarity.
7. Use the C++ string library instead of the C string library.
9. Use the C++11 chrono library instead of sys/wait.h.
10. Use the C++11 type safety features instead of type casting.
**/


