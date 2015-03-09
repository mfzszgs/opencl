#include<iostream>
#include<fstream>
#include<time.h>
#ifdef __APPLE__
#include <OpenCL/cl.h>
#else
#include <CL/cl.h>
#endif
#define NUM 60 
#define DIM 3
using namespace std;

const char *C_elem_KernelSource = "\n"\
"__kernel void vadd(const int Mdim, const int Ndim, const int Pdim, __global float *A, __global float *B, __global float*C) \n"\
"{\n"\
"	int i = get_global_id(0);\n"\
"	int j = get_global_id(1);\n"\
"	int k = get_global_id(2);\n"\
"	C[k*Mdim*Ndim+j*Mdim+i] = A[k*Mdim*Ndim+j*Mdim+i]+B[k*Mdim*Ndim+j*Mdim+i];\n"\
"}\n"\
"\n";

int vadd(float *A,float *B,float *C,int Mdim, int Ndim, int Pdim)
{
	
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

	cl_uint numPlatforms;
	cl_platform_id firstPlatformId;
	err = clGetPlatformIDs(1, &firstPlatformId, &numPlatforms);

    size_t size;
    err = clGetPlatformInfo(firstPlatformId,CL_PLATFORM_NAME,0,NULL,&size);
    char*name=(char*)alloca(sizeof(char)*size);
    err = clGetPlatformInfo(firstPlatformId,CL_PLATFORM_NAME,size,name,&size);
    cout<<name<<endl;

	err = clGetDeviceIDs(firstPlatformId, CL_DEVICE_TYPE_GPU, 1, &device_id, NULL);
	cl_context_properties properties[] =
	{
		CL_CONTEXT_PLATFORM, (cl_context_properties)firstPlatformId, 0
	};
	context = clCreateContext(properties, 1, &device_id, NULL, NULL, &err);
	commands = clCreateCommandQueue(context, device_id, CL_QUEUE_PROFILING_ENABLE, &err);
	szA = Ndim*Pdim*Mdim; szB =Ndim*Pdim*Mdim; szC =Ndim*Pdim*Mdim;
	
	//initmat

	
	//initmat
	a_in = clCreateBuffer(context, CL_MEM_READ_ONLY, sizeof(float)*szA, NULL, NULL);
	b_in = clCreateBuffer(context, CL_MEM_READ_ONLY, sizeof(float)*szB, NULL, NULL);
	c_out = clCreateBuffer(context, CL_MEM_WRITE_ONLY, sizeof(float)*szC, NULL, NULL);
	//program

//set the program like the helloword
	/*
	program = CreateProgram(context, "mulm.cl");
	if (program == NULL)
	{
	//	Cleanup(context, commandQueue, program, kernel, memObjects);
		return 1;
	}
	*/

//set the program deault
	program = clCreateProgramWithSource(context, 1, (const char**)& C_elem_KernelSource, NULL, &err);
	err = clBuildProgram(program, 0, NULL, NULL, NULL, NULL);
	if (err != CL_SUCCESS)
	{
		size_t len;
		char buffer[2048];
		cout<<"Error:Failed to build program executable!"<<endl;
		clGetProgramBuildInfo(program, device_id, CL_PROGRAM_BUILD_LOG, sizeof(buffer), buffer, &len);
		cout<<buffer<<endl;
		return CL_FALSE;
	}

	
	//kernel
	kernel = clCreateKernel(program, "vadd", &err);
	err = 0;
	err = clSetKernelArg(kernel, 0, sizeof(int), &Mdim);
	err |= clSetKernelArg(kernel, 1, sizeof(int), &Ndim);
	err |= clSetKernelArg(kernel, 2, sizeof(int), &Pdim);
	err |= clSetKernelArg(kernel, 3, sizeof(cl_mem), &a_in);
	err |= clSetKernelArg(kernel, 4, sizeof(cl_mem), &b_in);
	err |= clSetKernelArg(kernel, 5, sizeof(cl_mem), &c_out);

	err = clEnqueueWriteBuffer(commands, a_in, CL_TRUE, 0, sizeof(float)*szA, A, 0, NULL, NULL);
	err = clEnqueueWriteBuffer(commands, b_in, CL_TRUE, 0, sizeof(float)*szB, B, 0, NULL, NULL);
	cl_event prof_event;

	global[0] = (size_t)Mdim; global[1] = (size_t)Ndim;global[2]=(size_t)Pdim; nd = DIM;
    local[0]=1; local[1]=1;local[2]=1;
	err = clEnqueueNDRangeKernel(commands, kernel, nd, NULL,global,local, 0, NULL, &prof_event);
	clFinish(commands);

	cl_ulong ev_start_time = (cl_ulong)0;
	cl_ulong ev_end_time = (cl_ulong)0;
	size_t ret_size;

	err = clGetEventProfilingInfo(prof_event, CL_PROFILING_COMMAND_START, sizeof(cl_ulong), &ev_start_time, NULL);
	err = clGetEventProfilingInfo(prof_event, CL_PROFILING_COMMAND_END, sizeof(cl_ulong), &ev_start_time, NULL);

	err = clEnqueueReadBuffer(commands, c_out, CL_TRUE, 0, sizeof(float)*szC, C, 0, NULL, NULL);
    cout<<"OK"<<endl;
    ofstream fout;
    fout.open("result_gpu.txt");
    for(int i=0;i<szC;i++)
        if(!(i%(NUM*NUM)))fout<<C[i]<<endl;
    fout.close();
	clReleaseProgram(program);
	clReleaseKernel(kernel);
	clReleaseMemObject(a_in);
	clReleaseMemObject(b_in);
	clReleaseMemObject(c_out);
	clReleaseCommandQueue(commands);
	clReleaseContext(context);
//    static int count=0;
//    cout<<count++<<endl;
}


int main(int argc, char **argv)
{
//    ofstream fout;
//    fout.open("result_gpu.txt");
for(int j=NUM;j<=NUM;j++)
{
    float *A;
	float *B;
	float *C;
	int Mdim, Ndim, Pdim;
	//initialize
	Ndim = NUM; 
	Pdim = NUM; 
	Mdim = NUM;
	A = (float*)malloc(sizeof(float)*Ndim*Pdim*Mdim);
	B = (float*)malloc(sizeof(float)*Ndim*Pdim*Mdim);
	C = (float*)malloc(sizeof(float)*Ndim*Pdim*Mdim);
	for(int i=1;i<Ndim*Pdim*Mdim;i++)
	{
		A[i]=1;
        B[i]=1;
	}
	clock_t Tstart=clock();
	vadd(A, B, C, Mdim, Ndim, Pdim);
	clock_t Tstop=clock();
//    fout<<j<<"\t"<<(double)(Tstop-Tstart)/CLOCKS_PER_SEC<<endl;
//    fout.close();
}
}
