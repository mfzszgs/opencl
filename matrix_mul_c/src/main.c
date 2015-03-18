#include"matrix_mul.h"

int main(int argc, char **argv) {
	float *A;
	float *B;
	float *C;
	int Mdim, Ndim, Pdim;
	//initialize
	Ndim = 2;
	Pdim = 3;
	Mdim = 4;
	A = (float*) malloc(sizeof(float) * Ndim * Pdim);
	B = (float*) malloc(sizeof(float) * Pdim * Mdim);
	C = (float*) malloc(sizeof(float) * Ndim * Mdim);
	for (int i = 1; i < Ndim * Pdim; i++) {
		A[i] = i + 1;
	}
	for (int i = 1; i < Mdim * Pdim; i++) {
		B[i] = i + 1;
	}
	mul(A, B, C, Mdim, Ndim, Pdim,CL_DEVICE_TYPE_GPU);
//	output(C);
}
