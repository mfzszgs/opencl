/*
 * cl_create.h
 *
 *  Created on: Mar 10, 2015
 *      Author: wing
 */
#include <fstream>
#include <iostream>
#include <sstream>

#ifdef __APPLE__
#include <OpenCL/cl.h>
#else
#include <CL/cl.h>
#endif

///
//  Create an OpenCL context on the first available platform using
//  either a GPU or CPU depending on what is available.
//
cl_context create_context(cl_device_type device_type) {
	cl_int errNum;
	cl_uint numPlatforms;
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
cl_command_queue create_command_queue(cl_context context,
		cl_device_id *device) {
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

///
//  Create an OpenCL program from the kernel source file
//
cl_program create_program(cl_context context, cl_device_id device,
		const char* fileName) {
	cl_int errNum;
	cl_program program;

	std::ifstream kernelFile(fileName, std::ios::in);
	if (!kernelFile.is_open()) {
		std::cerr << "Failed to open file for reading: " << fileName
				<< std::endl;
		return NULL;
	}
//	assetion error line
	std::ostringstream oss;

	oss << kernelFile.rdbuf();
	std::string srcStdStr = oss.str();
	const char *srcStr = srcStdStr.c_str();
	program = clCreateProgramWithSource(context, 1, (const char**) &srcStr,
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

void cleanup(cl_context context, cl_command_queue commandQueue, int num_program,
		cl_program *program, int num_kernel, cl_kernel *kernel,
		int num_memObjects, cl_mem *memObjects) {

	for (int i = 0; i < num_memObjects; i++) {
		if (memObjects[i] != 0)
			clReleaseMemObject(memObjects[i]);
	}
	if (commandQueue != 0)
		clReleaseCommandQueue(commandQueue);

	for (int i = 0; i < num_kernel; i++) {
		if (kernel[i] != 0)
			clReleaseKernel(kernel[i]);
	}

	for (int i = 0; i < num_program; i++) {
		if (program[i] != 0)
			clReleaseProgram(program[i]);
	}

	if (context != 0)
		clReleaseContext(context);

}
