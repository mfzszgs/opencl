#include"cl_matrix.h"

int main(int argc, char **argv) {
	int*A;
	int*B;
	int*C;
	int Mdim, Ndim, Pdim;
	//initialize
	Ndim = 2;
	Pdim = 3;
	Mdim = 4;
	A = (int*) malloc(sizeof(int) * Ndim * Pdim);
	B = (int*) malloc(sizeof(int) * Pdim * Mdim);
	C = (int*) malloc(sizeof(int) * Ndim * Mdim);
	for (int i = 1; i < Ndim * Pdim; i++) {
		A[i] = i + 1;
	}
	for (int i = 1; i < Mdim * Pdim; i++) {
		B[i] = i + 1;
	}
	mul(A, B, C, Mdim, Ndim, Pdim,CL_DEVICE_TYPE_GPU);
}
