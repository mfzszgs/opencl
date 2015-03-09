/*
 * cl_math.h
 *
 *  Created on: Mar 5, 2015
 *      Author: wing
 */

#ifdef __APPLE__
#include <OpenCL/cl.h>
#else
#include <CL/cl.h>
#endif

const char *cl_vector_add_kernelsource =
		"\n"
				"__kernel void vadd(const int size, __global float *A, __global float *B, __global float*C)\n"
				"{\n"
				"	int gid0 = get_global_id(0);\n"
				"	int gid1 = get_global_id(1);\n"
				"	int gid2 = get_global_id(2);\n"
				"	int gsize0=get_global_size(0);\n"
				"	int gsize1=get_global_size(1);\n"
				"	int gsize2=get_global_size(2);\n"
				"	int tmp=gid2*gsize1*gsize0+gid1*gsize0+gid0;\n"
				"	if(tmp<size) C[tmp] = A[tmp]+B[tmp];\n"
				"}\n"
				"\n";

///
//  Create an OpenCL context on the first available platform using
//  either a GPU or CPU depending on what is available.
//
cl_context CreateContext(cl_device_type device_type) {
	cl_int errNum;
	cl_uint numPlatforms;
//	cl_platform_id firstPlatformId;
	cl_context context = NULL;
	cl_platform_id *platformId;

	// First, select an OpenCL platform to run on.  For this example, we
	// simply choose the first available platform.  Normally, you would
	// query for all available platforms and select the most appropriate one.
	errNum = clGetPlatformIDs(0, NULL, &numPlatforms);
	platformId = (cl_platform_id*) malloc(
			sizeof(cl_platform_id) * numPlatforms);
	errNum = clGetPlatformIDs(numPlatforms, platformId, &numPlatforms);
	if (errNum != CL_SUCCESS || numPlatforms <= 0) {
		std::cerr << "Failed to find any OpenCL platforms." << std::endl;
		return NULL;
	}

	// Next, create an OpenCL context on the platform.  Attempt to
	// create a GPU-based context, and if that fails, try to create
	// a CPU-based context.
	for (size_t i = 0; i < numPlatforms; i++) {
		cl_context_properties contextProperties[] = {
		CL_CONTEXT_PLATFORM, (cl_context_properties) platformId[i], 0 };
		context = clCreateContextFromType(contextProperties, device_type,
		NULL, NULL, &errNum);
		if (errNum == CL_SUCCESS)
			return context;

	}
	std::cerr
			<< "Failed to find any OpenCL platforms with the given device type."
			<< std::endl;
	return NULL;
}
///
//  Create a command queue on the first device available on the
//  context
//
cl_command_queue CreateCommandQueue(cl_context context, cl_device_id *device) {
	cl_int errNum;
	cl_device_id *devices;
	cl_command_queue commandQueue = NULL;
	size_t deviceBufferSize = -1;

	// First get the size of the devices buffer
	errNum = clGetContextInfo(context, CL_CONTEXT_DEVICES, 0, NULL,
			&deviceBufferSize);
	if (errNum != CL_SUCCESS) {
		std::cerr
				<< "Failed call to clGetContextInfo(...,GL_CONTEXT_DEVICES,...)";
		return NULL;
	}

	if (deviceBufferSize <= 0) {
		std::cerr << "No devices available.";
		return NULL;
	}

	// Allocate memory for the devices buffer
	devices = new cl_device_id[deviceBufferSize / sizeof(cl_device_id)];
	errNum = clGetContextInfo(context, CL_CONTEXT_DEVICES, deviceBufferSize,
			devices, NULL);
	if (errNum != CL_SUCCESS) {
		delete[] devices;
		std::cerr << "Failed to get device IDs";
		return NULL;
	}

	// In this example, we just choose the first available device.  In a
	// real program, you would likely use all available devices or choose
	// the highest performance device based on OpenCL device queries
	commandQueue = clCreateCommandQueue(context, devices[0],
			CL_QUEUE_PROFILING_ENABLE, NULL);
	if (commandQueue == NULL) {
		delete[] devices;
		std::cerr << "Failed to create commandQueue for device 0";
		return NULL;
	}

	*device = devices[0];
	delete[] devices;
	return commandQueue;
}

///
//  Create an OpenCL program from the kernel source file
//

cl_program CreateProgram(cl_context context, cl_device_id device,
		const char ** cl_vector_add_kernelsource) {
	cl_int errNum;
	cl_program program;

	program = clCreateProgramWithSource(context, 1, cl_vector_add_kernelsource,
	NULL, NULL);
	if (program == NULL) {
		std::cerr << "Failed to create CL program from source." << std::endl;
		return NULL;
	}

	errNum = clBuildProgram(program, 0, NULL, NULL, NULL, NULL);
	if (errNum != CL_SUCCESS) {
		// Determine the reason for the error
		char buildLog[16384];
		clGetProgramBuildInfo(program, device, CL_PROGRAM_BUILD_LOG,
				sizeof(buildLog), buildLog, NULL);

		std::cerr << "Error in kernel: " << std::endl;
		std::cerr << buildLog;
		clReleaseProgram(program);
		return NULL;
	}

	return program;
}

///
//  Create memory objects used as the arguments to the kernel
//  The kernel takes three arguments: result (output), a (input),
//  and b (input)
//
bool CreateMemObjects(cl_context context, cl_mem memObjects[3], int size) {
	memObjects[0] = clCreateBuffer(context, CL_MEM_READ_ONLY,
			sizeof(float) * size, NULL, NULL);
	memObjects[1] = clCreateBuffer(context, CL_MEM_READ_ONLY,
			sizeof(float) * size, NULL, NULL);
	memObjects[2] = clCreateBuffer(context, CL_MEM_READ_WRITE,
			sizeof(float) * size, NULL, NULL);

	if (memObjects[0] == NULL || memObjects[1] == NULL || memObjects[2] == NULL) {
		std::cerr << "Error creating memory objects." << std::endl;
		return false;
	}

	return true;
}

///
//  Cleanup any created OpenCL resources
//
void Cleanup(cl_context context, cl_command_queue commandQueue,
		cl_program program, cl_kernel kernel, cl_mem memObjects[3]) {
	for (int i = 0; i < 3; i++) {
		if (memObjects[i] != 0)
			clReleaseMemObject(memObjects[i]);
	}
	if (commandQueue != 0)
		clReleaseCommandQueue(commandQueue);

	if (kernel != 0)
		clReleaseKernel(kernel);

	if (program != 0)
		clReleaseProgram(program);

	if (context != 0)
		clReleaseContext(context);

}

int vector_add(int size, float *a, float *b, float *c,cl_device_type device_type) {

	int err;
	cl_context context;
	cl_command_queue commands;
	cl_program program;
	cl_kernel kernel;
	cl_uint nd;
	cl_device_id device_id;
	cl_mem memObjects[3];
	cl_event prof_event;

	context = CreateContext(device_type);
	commands = CreateCommandQueue(context, &device_id);
	CreateMemObjects(context, memObjects, size);
	program = CreateProgram(context, device_id,
			(const char**) &cl_vector_add_kernelsource);
	kernel = clCreateKernel(program, "vadd", &err);
	if (err != CL_SUCCESS) {
		std::cerr << "Error creating kernel" << std::endl;
		return CL_FALSE;
	}
	err |= clSetKernelArg(kernel, 0, sizeof(int), &size);
	err |= clSetKernelArg(kernel, 1, sizeof(cl_mem), &memObjects[0]);
	err |= clSetKernelArg(kernel, 2, sizeof(cl_mem), &memObjects[1]);
	err |= clSetKernelArg(kernel, 3, sizeof(cl_mem), &memObjects[2]);

	if (err != CL_SUCCESS) {
		std::cerr << "Error setting kernel arguments" << std::endl;
		return CL_FALSE;
	}

	size_t work_group_size;
	clGetKernelWorkGroupInfo(kernel, device_id, CL_KERNEL_WORK_GROUP_SIZE,
			sizeof(size_t), &work_group_size, NULL);
	size_t local[] = { work_group_size };
	size_t global[] = { (size + work_group_size - 1) / work_group_size
			* work_group_size };
	nd = 1;
	err = clEnqueueNDRangeKernel(commands, kernel, nd, NULL, global, local, 0,
	NULL, &prof_event);
	if (err != CL_SUCCESS) {
		std::cerr << "Error queuing kernel for execution." << err << std::endl;
		return CL_FALSE;
	}
	clFinish(commands);

	cl_ulong ev_start_time = (cl_ulong) 0;
	cl_ulong ev_end_time = (cl_ulong) 0;

	err = clGetEventProfilingInfo(prof_event, CL_PROFILING_COMMAND_START,
			sizeof(cl_ulong), &ev_start_time, NULL);
	if (err != CL_SUCCESS) {
		std::cerr << "Error getting profile info about start." << err
				<< std::endl;
		return CL_FALSE;
	}
	err = clGetEventProfilingInfo(prof_event, CL_PROFILING_COMMAND_END,
			sizeof(cl_ulong), &ev_end_time, NULL);
	if (err != CL_SUCCESS) {
		std::cerr << "Error getting profile info about end." << err
				<< std::endl;
		return CL_FALSE;
	}
	std::cout << ev_end_time - ev_start_time << "ns"
			<< std::endl;
	err = clEnqueueReadBuffer(commands, memObjects[2],
	CL_TRUE, 0, sizeof(float) * size, c, 0, NULL, NULL);

	Cleanup(context, commands, program, kernel, memObjects);
	return CL_SUCCESS;
}

