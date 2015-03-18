#include<stdio.h>
#include"cl_create.h"

#define ORDER 2
#define DIM 2

const char *C_elem_KernelSource =
		"\n"
				"__kernel void mmul(const int Mdim, const int Ndim, const int Pdim, __global float *A, __global float *B, __global float*C) \n"
				"{\n"
				"	int k;\n"
				"	int i = get_global_id(0);\n"
				"	int j = get_global_id(1);\n"
				"	float tmp;\n"
				"	if ((i<Ndim) && (j<Mdim))\n"
				"	{\n"
				"		tmp = 0.0;\n"
				"		for (k = 0; k<Pdim; k++)\n"
				"			tmp += A[i*Pdim + k] * B[k*Mdim + j];\n"
				"		C[i*Mdim + j] = tmp;\n"
				"	}\n"
				"}\n"
				"\n";
const char *srcStr=
"__kernel void matrix_mul(const int Mdim, const int Ndim, const int Pdim, __global float *A, __global float *B, __global float*C)\n"
"{\n"
    "int k;\n"
    "int i = get_global_id(0);\n"
    "int j = get_global_id(1);\n"
    "float tmp;\n"
    "if ((i<Ndim) && (j<Mdim))\n"
    "{\n"
        "tmp = 0.0;\n"
        "for (k = 0; k<Pdim; k++)\n"
            "tmp += A[i*Pdim + k] * B[k*Mdim + j];\n"
        "C[i*Mdim + j] = tmp;\n"
    "}\n"
"}\n"
"\n";

int mul(float *A, float *B, float *C, int Mdim, int Ndim, int Pdim,
		cl_device_type device_type) {

	int err;
	int szA, szB, szC;
	size_t global[DIM];
	size_t local[DIM];
	cl_device_id device_id;
	cl_context context;
	cl_command_queue commands;
	cl_program program;
	cl_kernel kernel;
	cl_uint nd;
	cl_mem a_in;
	cl_mem b_in;
	cl_mem c_out;
	int i;

	context = create_context(device_type);
	commands = create_command_queue(context, &device_id);

	szA = Ndim * Pdim;
	szB = Pdim * Mdim;
	szC = Ndim * Mdim;

	//initmat
	a_in = clCreateBuffer(context, CL_MEM_READ_ONLY, sizeof(float) * szA, NULL,
	NULL);
	b_in = clCreateBuffer(context, CL_MEM_READ_ONLY, sizeof(float) * szB, NULL,
	NULL);
	c_out = clCreateBuffer(context, CL_MEM_WRITE_ONLY, sizeof(float) * szC,
	NULL, NULL);


//set the program deault
	program=create_program(context,device_id,srcStr);

	//kernel
	kernel = clCreateKernel(program, "matrix_mul", &err);
	err = 0;
	err = clSetKernelArg(kernel, 0, sizeof(int), &Mdim);
	err |= clSetKernelArg(kernel, 1, sizeof(int), &Ndim);
	err |= clSetKernelArg(kernel, 2, sizeof(int), &Pdim);
	err |= clSetKernelArg(kernel, 3, sizeof(cl_mem), &a_in);
	err |= clSetKernelArg(kernel, 4, sizeof(cl_mem), &b_in);
	err |= clSetKernelArg(kernel, 5, sizeof(cl_mem), &c_out);

	err = clEnqueueWriteBuffer(commands, a_in, CL_TRUE, 0, sizeof(float) * szA,
			A, 0, NULL, NULL);
	err = clEnqueueWriteBuffer(commands, b_in, CL_TRUE, 0, sizeof(float) * szB,
			B, 0, NULL, NULL);
	cl_event prof_event;

	global[0] = (size_t) Ndim;
	global[1] = (size_t) Mdim;
	nd = 2;
	err = clEnqueueNDRangeKernel(commands, kernel, nd, NULL, global, NULL, 0,
	NULL, &prof_event);
	clFinish(commands);

	cl_ulong ev_start_time = (cl_ulong) 0;
	cl_ulong ev_end_time = (cl_ulong) 0;
	size_t ret_size;

	err = clGetEventProfilingInfo(prof_event, CL_PROFILING_COMMAND_START,
			sizeof(cl_ulong), &ev_start_time, NULL);
	err = clGetEventProfilingInfo(prof_event, CL_PROFILING_COMMAND_END,
			sizeof(cl_ulong), &ev_end_time, NULL);
	printf("time:%d\n",ev_end_time-ev_start_time);
	err = clEnqueueReadBuffer(commands, c_out, CL_TRUE, 0, sizeof(float) * szC,
			C, 0, NULL, NULL);

	clReleaseProgram(program);
	clReleaseKernel(kernel);
	clReleaseMemObject(a_in);
	clReleaseMemObject(b_in);
	clReleaseMemObject(c_out);
	clReleaseCommandQueue(commands);
	clReleaseContext(context);

}

