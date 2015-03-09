#include<stdio.h>
#include<stdlib.h>
#include<time.h>
#include<iostream>
#define random(x) (rand()%x)
#define FLT 1000

void selectSort(int arr[], int len) {
	int i, j, temp;
	for (i = 0; i < len; i++) {
		for (j = i + 1; j < len; j++) {
			if (arr[i] > arr[j]) {
				temp = arr[i];
				arr[i] = arr[j];
				arr[j] = temp;
			}
		}
	}
}

void array_output(int* array, int num) {
	for (int i = 0; i < num; i++) {
		std::cout << array[i] << "\t";
	}
	std::cout << std::endl;
}
void array_output(float* array, int num) {
	for (int i = 0; i < num; i++) {
		std::cout << array[i] << "\t";
	}
	std::cout << std::endl;
}
int main() {
	srand((int) time(0));
	int vertex_count = 10000;
	int edge_count = 1000000;
	int *vertex_array = (int*) malloc(sizeof(int) * vertex_count);
	int *edge_array = (int*) malloc(sizeof(int) * edge_count);
	float *weight_array=(float*)malloc(sizeof(float)*edge_count);
	vertex_array[0] = 0;
	for (int i = 1; i < vertex_count; i++) {
		vertex_array[i] = random(edge_count+1);
//		std::cout<<vertex_array[i]<<std::endl;
	}
	selectSort(vertex_array, vertex_count);

	for (int vertex_index = 0; vertex_index < vertex_count; vertex_index++) {
		int edge_start = vertex_array[vertex_index];
		int edge_end;
		if (vertex_index < vertex_count - 1)
			edge_end = vertex_array[vertex_index + 1];
		else
			edge_end = edge_count;
		for (int edge_index = edge_start; edge_index < edge_end; edge_index++) {
			edge_array[edge_index] = random((vertex_count - 1));
			if (edge_array[edge_index] == vertex_index)
				edge_array[edge_index] = vertex_count - 1;
		}
	}
	for (int edge_index = 0; edge_index < edge_count; edge_index++) {
		weight_array[edge_index] = random(FLT);
		weight_array[edge_index] /= FLT;
	}

//	array_output(vertex_array, vertex_count);
//	array_output(edge_array, edge_count);
//	array_output(weight_array, edge_count);
	std::cout << "OK" << std::endl;
	return 0;
}
