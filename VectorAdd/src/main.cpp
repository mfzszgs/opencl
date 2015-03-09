#include<time.h>
#include<fstream>
#include<iostream>
#include<stdlib.h>

#include "cl_vector_add.h"

#define NUM 10
#define DIM 3
using namespace std;

int main(int argc, char **argv) {
	float *A;
	float *B;
	float *C;
	int size;
	//initialize
	size = 1e6;
	A = (float*) malloc(sizeof(float) * size);
	B = (float*) malloc(sizeof(float) * size);
	C = (float*) malloc(sizeof(float) * size);
	for (int i = 0; i < size; i++) {
		A[i] = 1;
		B[i] = 1;
	}
	std::cout<<"GPU:"<<size<<"\t";
	vector_add(size,A, B, C,CL_DEVICE_TYPE_GPU);
	std::cout<<"CPU:"<<size<<"\t";
	vector_add(size,A, B, C,CL_DEVICE_TYPE_CPU);

//	ofstream c_out;
//	c_out.open("c_out");
//	for (int i = 0; i < size; i++)
//		c_out << C[i] << "\t";
//	c_out.close();

}
