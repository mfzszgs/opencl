#include"Dijkstra.h"
#include<time.h>
#include<stdio.h>
#include<stdlib.h>
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
int main() {
	//init
	int vertex_count = 100000;
	int edge=5;
	int num_start=100;

	srand((int) time(0));
	int edge_count = edge * vertex_count;
	int *vertex_array = (int*) malloc(sizeof(int) * vertex_count);
	int *edge_array = (int*) malloc(sizeof(int) * edge_count);
	float *weight_array = (float*) malloc(sizeof(float) * edge_count);
	vertex_array[0] = 0;
	for (int i = 1; i < vertex_count; i++) {
//		vertex_array[i] = random((edge_count + 1));
		vertex_array[i] = edge_count / vertex_count * i;
	}
//	selectSort(vertex_array, vertex_count);

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

	Graph_Data graph_data;
	graph_data.vertex_array = vertex_array;
	graph_data.edge_array = edge_array;
	graph_data.weight_array = weight_array;
	graph_data.vertex_count = vertex_count;
	graph_data.edge_count = edge_count;
	float *cost_array;
	cost_array = (float*) malloc(sizeof(float) * vertex_count);
	int *start_array;
	start_array = (int*) malloc(sizeof(int) * num_start);
	for (int i = 0; i < num_start; i++) {
		start_array[i] = i;
	}
	std::cout << "GPU\t" << std::endl;
	dijkstra(graph_data, num_start, start_array, cost_array,
			CL_DEVICE_TYPE_GPU);
	std::cout << "CPU\t"<<std::endl;
	dijkstra(graph_data, num_start, start_array, cost_array,
			CL_DEVICE_TYPE_CPU);
//	if (dijkstra(graph_data, 0, cost_array) == CL_SUCCESS)
//		std::cout<<"The cost array is  "<<std::endl;
//		for (int i = 0; i < vertex_count; i++) {
//			std::cout << cost_array[i];
//			if(i<vertex_count-1)std::cout << "\t";
//		}

//	std::cout<<"."<<std::endl;
	return 0;
}

