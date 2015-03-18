/*
 * cl_math.h
 *
 *  Created on: Mar 5, 2015
 *      Author: wing
 */

#include"cl_create.h"

int vector_add(int size, float *a, float *b, float *c,
		cl_device_type device_type) {

	int err;
	cl_context context;
	cl_command_queue commands;
	cl_program program;
	cl_kernel kernel;
	cl_uint nd;
	cl_device_id device_id;
	cl_mem memObjects[3];
	cl_event prof_event;

	context = create_context(device_type);
	commands = create_command_queue(context, &device_id);

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

	program = create_program(context, device_id, "src/vector_add.cl");
	kernel = clCreateKernel(program, "vector_add", &err);
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
	std::cout << ev_end_time - ev_start_time << "ns" << std::endl;
	err = clEnqueueReadBuffer(commands, memObjects[2],
	CL_TRUE, 0, sizeof(float) * size, c, 0, NULL, NULL);

	cleanup(context, commands, 1, &program, 1, &kernel, 3, memObjects);
	return CL_SUCCESS;
}

