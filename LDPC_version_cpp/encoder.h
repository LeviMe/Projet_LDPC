#ifndef ENCODER_H
#define ENCODER_H

int* random_vec(int n);
int* modulate(int *vec, int n);
double* add_gaussian_noise(int* vec, int n,double EB_N0_db, bool set=true);

class Encoder {
    public :
    int **G_matrix;
    int k, n;
    public:
    Encoder(int **G,int k, int n);
    Encoder();
    int* encode(int* vec);
};


#endif // ENCODER_H


