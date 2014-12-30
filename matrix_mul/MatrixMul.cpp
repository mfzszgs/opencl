#include<stdio.h>
#ifdef __APPLE__
#include <OpenCL/cl.h>
#else
#include <CL/cl.h>
#endif
#define ORDER 2
#define DIM 2



const char *C_elem_KernelSource = "\n"\
"__kernel void mmul(const int Mdim, const int Ndim, const int Pdim, __global float *A, __global float *B, __global float*C) \n"\
"{\n"\
"	int k;\n"\
"	int i = get_global_id(0);\n"\
"	int j = get_global_id(1);\n"\
"	float tmp;\n"\
"	if ((i<Ndim) && (j<Mdim))\n"\
"	{\n"\
"		tmp = 0.0;\n"\
"		for (k = 0; k<Pdim; k++)\n"\
"			tmp += A[i*Pdim + k] * B[k*Mdim + j];\n"\
"		C[i*Mdim + j] = tmp;\n"\
"	}\n"\
"}\n"\
"\n";

/*
cl_program CreateProgram(cl_context context, const char* fileName)
{
	cl_int errNum;
	cl_program program;

	std::ifstream kernelFile(fileName, std::ios::in);
	if (!kernelFile.is_open())
	{
		std::cerr << "Failed to open file for reading: " << fileName << std::endl;
		return NULL;
	}

	std::ostringstream oss;
	oss << kernelFile.rdbuf();

	std::string srcStdStr = oss.str();
	const char *srcStr = srcStdStr.c_str();
	program = clCreateProgramWithSource(context, 1,
		(const char**)&srcStr,
		NULL, NULL);
	if (program == NULL)
	{
		std::cerr << "Failed to create CL program from source." << std::endl;
		return NULL;
	}

	errNum = clBuildProgram(program, 0, NULL, NULL, NULL, NULL);
	if (errNum != CL_SUCCESS)
	{
		// Determine the reason for the error
		char buildLog[16384];
		std::cerr << "Error in kernel: " << std::endl;
		std::cerr << buildLog;
		clReleaseProgram(program);
		return NULL;
	}

	return program;
}

*/
/*
void output(float *F){
	for (int r = 0; r < 8; r++)
		std::cout << F[r] << std::endl;
}
*/
int mul(float *A,float *B,float *C,int Mdim, int Ndim, int Pdim)
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
	err = clGetDeviceIDs(firstPlatformId, CL_DEVICE_TYPE_GPU, 1, &device_id, NULL);
	cl_context_properties properties[] =
	{
		CL_CONTEXT_PLATFORM, (cl_context_properties)firstPlatformId, 0
	};
	context = clCreateContext(properties, 1, &device_id, NULL, NULL, &err);
	commands = clCreateCommandQueue(context, device_id, CL_QUEUE_PROFILING_ENABLE, &err);
	szA = Ndim*Pdim; szB = Pdim*Mdim; szC = Ndim*Mdim;
	
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
		printf("Error:Failed to build program executable!\n");
		clGetProgramBuildInfo(program, device_id, CL_PROGRAM_BUILD_LOG, sizeof(buffer), buffer, &len);
		printf("%s\n", buffer);
		return CL_FALSE;
	}

	
	//kernel
	kernel = clCreateKernel(program, "mmul", &err);
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

	global[0] = (size_t)Ndim; global[1] = (size_t)Mdim; nd = 2;
	err = clEnqueueNDRangeKernel(commands, kernel, nd, NULL, global, NULL, 0, NULL, &prof_event);
	clFinish(commands);

	cl_ulong ev_start_time = (cl_ulong)0;
	cl_ulong ev_end_time = (cl_ulong)0;
	size_t ret_size;

	err = clGetEventProfilingInfo(prof_event, CL_PROFILING_COMMAND_START, sizeof(cl_ulong), &ev_start_time, NULL);
	err = clGetEventProfilingInfo(prof_event, CL_PROFILING_COMMAND_END, sizeof(cl_ulong), &ev_start_time, NULL);

	err = clEnqueueReadBuffer(commands, c_out, CL_TRUE, 0, sizeof(float)*szC, C, 0, NULL, NULL);

	clReleaseProgram(program);
	clReleaseKernel(kernel);
	clReleaseMemObject(a_in);
	clReleaseMemObject(b_in);
	clReleaseMemObject(c_out);
	clReleaseCommandQueue(commands);
	clReleaseContext(context);

}


int main(int argc, char **argv)
{
	float *A;
	float *B;
	float *C;
	int Mdim, Ndim, Pdim;
	//initialize
	Ndim = 2; 
	Pdim = 3; 
	Mdim = 4;
	A = (float*)malloc(sizeof(float)*Ndim*Pdim);
	B = (float*)malloc(sizeof(float)*Pdim*Mdim);
	C = (float*)malloc(sizeof(float)*Ndim*Mdim);
	for(int i=1;i<Ndim*Pdim;i++)
	{
		A[i]=i+1;
	}
	for(int i=1;i<Mdim*Pdim;i++)
	{
		B[i]=i+1;
	}
	mul(A, B, C, Mdim, Ndim, Pdim);
//	output(C);
}
