#ifndef OPENCL_LOAD_H
#define OPENCL_LOAD_H

#define CL_USE_DEPRECATED_OPENCL_1_2_APIS

#include <CL/cl.h>
#include <gdk/gdk.h>
#include <err.h>

#define MAX_SRC_SIZE (0x100000)

typedef struct _camera {
	cl_float x, y, z, yaw, pitch;
} camera;

typedef struct _environment {
	
	camera * cam;
	float fov;
	size_t size_h;
	size_t size_w;
	cl_context context;
	cl_device_id device_id;
	cl_platform_id platform_id;
	cl_command_queue command_queue;

} environment;


GdkPixbuf* render(environment* env);

environment* create_environment();

void destroy_environment(environment* env);

cl_program compile_program(environment* env, char* src, size_t source_size);

cl_program compile_program_from_source(environment* env, char* source_path);

#endif
