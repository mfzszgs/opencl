/*
 * cl_create.h
 *
 *  Created on: Mar 10, 2015
 *      Author: wing
 */
#ifndef CL_CREATE_H_
#define CL_CREATE_H_
///
#ifdef __APPLE__
#include <OpenCL/cl.h>
#else
#include <CL/cl.h>
#endif
//  Create an OpenCL context on the first available platform using
//  either a GPU or CPU depending on what is available.
//
cl_context create_context(cl_device_type device_type) ;

///
//  Create a command queue on the first device available on the
//  context
//
cl_command_queue create_command_queue(cl_context context,
		cl_device_id *device) ;

///
//  Create an OpenCL program from the kernel source file
//

///
//  Create an OpenCL program from the kernel source file
//
cl_program create_program(cl_context context, cl_device_id device,
		const char* srcStr) ;

void cleanup(cl_context context, cl_command_queue commandQueue, int num_program,
		cl_program *program, int num_kernel, cl_kernel *kernel,
		int num_memObjects, cl_mem *memObjects) ;
#endif
