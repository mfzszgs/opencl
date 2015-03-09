#include"MyOpenCL.h"
#include<fstream>

int main(int argc, char **argv) {
	float *A;
	float *B;
	float *C;
	int Mdim, Ndim, Pdim;
	//initialize
	Ndim = NUM;
	Pdim = NUM;
	Mdim = NUM;
	A = (float*) malloc(sizeof(float) * Ndim * Pdim);
	B = (float*) malloc(sizeof(float) * Pdim * Mdim);
	C = (float*) malloc(sizeof(float) * Ndim * Mdim);
	for (int i = 0; i < Ndim * Pdim; i++) {
		A[i] =  1;
	}
	for (int i = 0; i < Mdim * Pdim; i++) {
		B[i] =  1;
	}

	clock_t Tstart = clock();
	mul(A, B, C, Mdim, Ndim, Pdim);
	clock_t Tstop = clock();
	cout << (double) (Tstop - Tstart) / CLOCKS_PER_SEC << endl;

	ofstream fout;
	fout.open("c_result");
	for (int i = 0; i < Ndim; i++) {
		for (int j = 0; j < Mdim; j++) {
			fout << C[i * Mdim + j] << "\t";
		}
		fout << endl;
	}
	fout.close();
}
