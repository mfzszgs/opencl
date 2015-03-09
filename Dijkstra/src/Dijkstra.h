//============================================================================
// Name        : Dijkstra.cpp
// Author      : 
// Version     :
// Copyright   : Your copyright notice
// Description : Hello World in C++, Ansi-style
//============================================================================

#include <fstream>
#include <iostream>
#include <sstream>
#include<string>

#ifdef __APPLE__
#include <OpenCL/cl.h>
#else
#include <CL/cl.h>
#endif

//#define FLT_MAX 1e10
typedef struct {
	int *vertex_array;
	int *edge_array;
	float *weight_array;
	int vertex_count;
	int edge_count;
} Graph_Data;

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

int check_err(int err, char* ch) {
	if (err != CL_SUCCESS) {
		std::cerr << "Error in : " << ch << err << std::endl;
		return CL_FALSE;
	} else {
		return CL_SUCCESS;
	}
}
int enqueue_kernel(cl_kernel kernel, cl_device_id device_id,
		cl_command_queue commands, size_t size, cl_event* prof_event) {
	size_t work_group_size;
	clGetKernelWorkGroupInfo(kernel, device_id, CL_KERNEL_WORK_GROUP_SIZE,
			sizeof(size_t), &work_group_size, NULL);
	size_t local[] = { work_group_size };
	size_t global[] = { (size + work_group_size - 1) / work_group_size
			* work_group_size };
	int nd = 1;
	int err = clEnqueueNDRangeKernel(commands, kernel, nd, NULL, global, local,
			0,
			NULL, prof_event);
	if (err != CL_SUCCESS) {
		std::cerr << "Error queuing kernel for execution." << err << std::endl;
		return CL_FALSE;
	}
	return CL_SUCCESS;
}

void Cleanup(cl_context context, cl_command_queue commandQueue, int num_program,
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

void output(cl_command_queue commands, cl_mem* mem_objects,
		Graph_Data graph_data) {
	int num[] = { graph_data.vertex_count, graph_data.edge_count,
			graph_data.vertex_count, graph_data.edge_count,
			graph_data.vertex_count, graph_data.vertex_count };
	int *int_array[3];
	float *float_array[3];

	for (int i = 0; i < 6; i++) {
		if (i < 3) {
			int_array[i] = (int*) malloc(sizeof(int) * num[i]);
			clEnqueueReadBuffer(commands, mem_objects[i], CL_TRUE, 0,
					sizeof(int) * num[i], int_array[i], 0, NULL, NULL);
			for (int j = 0; j < num[i]; j++) {
				std::cout << int_array[i][j] << "\t";
			}
		} else {
			float_array[i - 3] = (float*) malloc(sizeof(float) * num[i]);
			clEnqueueReadBuffer(commands, mem_objects[i], CL_TRUE, 0,
					sizeof(float) * num[i], float_array[i - 3], 0, NULL, NULL);
			for (int j = 0; j < num[i]; j++) {
				std::cout << float_array[i - 3][j] << "\t";
			}
		}
		std::cout << std::endl;
	}
	std::cout << "------------------------------" << std::endl;
}

int dijkstra(Graph_Data graph_data, int num_start, int *start_array,
		float *cost_array, cl_device_type device_type) {
	int err;
	cl_context context;
	cl_command_queue commands;
	cl_program program[3];
	cl_kernel kernel[3];
	cl_device_id device_id;
	cl_mem mem_objects[6];
	cl_event event[3];
	cl_ulong sum_time = 0;
	context = create_context(device_type);
	commands = create_command_queue(context, &device_id);

//create mem object
//vertex_array,edge_array,mask_array,weight_array,cost_array,updating_cost_array;
	mem_objects[0] = clCreateBuffer(context,
	CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR,
			sizeof(int) * graph_data.vertex_count, graph_data.vertex_array,
			&err);
	mem_objects[1] = clCreateBuffer(context,
	CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR,
			sizeof(int) * graph_data.edge_count, graph_data.edge_array, &err);

	mem_objects[2] = clCreateBuffer(context, CL_MEM_READ_WRITE,
			sizeof(int) * graph_data.vertex_count, NULL, &err);
	mem_objects[3] = clCreateBuffer(context,
	CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR,
			sizeof(float) * graph_data.edge_count, graph_data.weight_array,
			&err);
	mem_objects[4] = clCreateBuffer(context, CL_MEM_READ_WRITE,
			sizeof(float) * graph_data.vertex_count, NULL, &err);
	mem_objects[5] = clCreateBuffer(context, CL_MEM_READ_WRITE,
			sizeof(float) * graph_data.vertex_count, NULL, &err);
	if (err != CL_SUCCESS) {
		std::cerr << "Error in creating memObject, the error number is " << err
				<< std::endl;
		return CL_FALSE;
	}

	//create program 0
	program[0] = create_program(context, device_id, "src/initializeBuffers.cl");

	//create kernel 0
	kernel[0] = clCreateKernel(program[0], "initializeBuffers", &err);
	if (err != CL_SUCCESS) {
		std::cerr << "Error in creating kernel 0, the error number is " << err
				<< std::endl;
		return CL_FALSE;
	}
	err |= clSetKernelArg(kernel[0], 0, sizeof(cl_mem), &mem_objects[2]);
	err |= clSetKernelArg(kernel[0], 1, sizeof(cl_mem), &mem_objects[4]);
	err |= clSetKernelArg(kernel[0], 2, sizeof(cl_mem), &mem_objects[5]);
	err |= clSetKernelArg(kernel[0], 4, sizeof(int), &graph_data.vertex_count);
	if (err != CL_SUCCESS) {
		std::cerr << "Error in setting kernel0 parameter, the error number is "
				<< err << std::endl;
		return CL_FALSE;
	}

	//initialize ND
	size_t work_group_size;
	clGetKernelWorkGroupInfo(kernel[0], device_id,
	CL_KERNEL_WORK_GROUP_SIZE, sizeof(size_t), &work_group_size,
	NULL);
	size_t local[] = { work_group_size };
	size_t global[] = { (graph_data.vertex_count + work_group_size - 1)
			/ work_group_size * work_group_size };
	int nd = 1;

	//create program 1
	program[1] = create_program(context, device_id, "src/DijkstraKernel1.cl");
	//create kernel 1
	kernel[1] = clCreateKernel(program[1], "DijkstraKernel1", &err);
	if (err != CL_SUCCESS) {
		std::cerr << "Error in creating kernel1, the error number is " << err
				<< std::endl;
		return CL_FALSE;
	}
	//set kernel1 parameter
	err |= clSetKernelArg(kernel[1], 0, sizeof(cl_mem), &mem_objects[0]);
	err |= clSetKernelArg(kernel[1], 1, sizeof(cl_mem), &mem_objects[1]);
	err |= clSetKernelArg(kernel[1], 2, sizeof(cl_mem), &mem_objects[2]);
	err |= clSetKernelArg(kernel[1], 3, sizeof(cl_mem), &mem_objects[3]);
	err |= clSetKernelArg(kernel[1], 4, sizeof(cl_mem), &mem_objects[4]);
	err |= clSetKernelArg(kernel[1], 5, sizeof(cl_mem), &mem_objects[5]);
	err |= clSetKernelArg(kernel[1], 6, sizeof(int), &graph_data.vertex_count);
	err |= clSetKernelArg(kernel[1], 7, sizeof(int), &graph_data.edge_count);
	if (err != CL_SUCCESS) {
		std::cerr << "Error in setting kernel1 parameter, the error number is "
				<< err << std::endl;
		return CL_FALSE;
	}

	//create program 2
	program[2] = create_program(context, device_id, "src/DijkstraKernel2.cl");
	//create kernel 2
	kernel[2] = clCreateKernel(program[2], "DijkstraKernel2", &err);
	if (err != CL_SUCCESS) {
		std::cerr << "Error in creating kernel2, the error number is " << err
				<< std::endl;
		return CL_FALSE;
	}
	//set kernel2 parameter
	err |= clSetKernelArg(kernel[2], 0, sizeof(cl_mem), &mem_objects[2]);
	err |= clSetKernelArg(kernel[2], 1, sizeof(cl_mem), &mem_objects[4]);
	err |= clSetKernelArg(kernel[2], 2, sizeof(cl_mem), &mem_objects[5]);
	err |= clSetKernelArg(kernel[2], 3, sizeof(int), &graph_data.vertex_count);

	if (err != CL_SUCCESS) {
		std::cerr << "Error in setting kernel2 parameter, the error number is "
				<< err << std::endl;
		return CL_FALSE;
	}

	for (int start_index = 0; start_index < num_start; start_index++) {

//set kernel0 parameter
		err |= clSetKernelArg(kernel[0], 3, sizeof(int),
				&start_array[start_index]);
		if (err != CL_SUCCESS) {
			std::cerr
					<< "Error in setting kernel0 parameter about start, the error number is "
					<< err << std::endl;
			return CL_FALSE;
		}

//enqueue kernel0
		err = clEnqueueNDRangeKernel(commands, kernel[0], nd, NULL, global,
				local, 0,
				NULL, &event[0]);
		if (err != CL_SUCCESS) {
			std::cerr
					<< "Error in queuing kernel0 for execution, the error number is "
					<< err << std::endl;
			return CL_FALSE;
		}

		//enqueue kernel1
		err = clEnqueueNDRangeKernel(commands, kernel[1], nd, NULL, global,
				local, 1, &event[0], &event[1]);
		if (err != CL_SUCCESS) {
			std::cerr
					<< "Error in queuing kernel1 for execution, the error number is "
					<< err << std::endl;
			return CL_FALSE;
		}

		int *mask_array = (int*) malloc(sizeof(int) * graph_data.vertex_count);
		size_t times = graph_data.vertex_count;
		while (times--) {
			//enqueue kernel2
			err = clEnqueueNDRangeKernel(commands, kernel[2], nd, NULL, global,
					local, 1, &event[1], &event[2]);
			if (err != CL_SUCCESS) {
				std::cerr
						<< "Error in queuing kernel2 for execution,the error number is "
						<< err << std::endl;
				return CL_FALSE;
			}

			//check the result;
			if (times % 10 == 0) {
				clFinish(commands);
				err = clEnqueueReadBuffer(commands, mem_objects[2],
				CL_TRUE, 0, sizeof(int) * graph_data.vertex_count, mask_array,
						0,
						NULL, NULL);
				clFinish(commands);
				int tmp = 0;
				for (int i = 0; i < graph_data.vertex_count; i++)
					tmp += mask_array[i];
				if (tmp == 0) {
//					std::cout << "countTimes:"
//							<< graph_data.vertex_count - times << "\t";
					break;
				}
			}
			//enqueue kernel1
			err = clEnqueueNDRangeKernel(commands, kernel[1], nd, NULL, global,
					local, 1, &event[2], &event[1]);
			if (err != CL_SUCCESS) {
				std::cerr
						<< "Error in queuing kernel1 for execution, the error number is "
						<< err << std::endl;
				return CL_FALSE;
			}
		}

		err = clEnqueueReadBuffer(commands, mem_objects[4],
		CL_TRUE, 0, sizeof(float) * graph_data.vertex_count, cost_array, 0,
		NULL,
		NULL);

//get time
		cl_ulong ev_start_time = (cl_ulong) 0;
		cl_ulong ev_end_time = (cl_ulong) 0;
		err = clGetEventProfilingInfo(event[0], CL_PROFILING_COMMAND_START,
				sizeof(cl_ulong), &ev_start_time, NULL);
		if (err != CL_SUCCESS) {
			std::cerr
					<< "Error getting profile info about start, the error number is "
					<< err << std::endl;
			return CL_FALSE;
		}
		err = clGetEventProfilingInfo(event[2], CL_PROFILING_COMMAND_END,
				sizeof(cl_ulong), &ev_end_time, NULL);
		if (err != CL_SUCCESS) {
			std::cerr
					<< "Error getting profile info about end, the error number is "
					<< err << std::endl;
			return CL_FALSE;
		}

		sum_time+= (ev_end_time - ev_start_time);
//		std::cout << "Time :" << ev_end_time - ev_start_time << " ns"
//				<< std::endl;

	}
	Cleanup(context, commands, 3, program, 3, kernel, 6, mem_objects);
			std::cout << "Time :" << sum_time << " ns"
					<< std::endl;
	return CL_SUCCESS;

}

