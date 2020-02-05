#ifndef OPENCL_LOAD_H
#define OPENCL_LOAD_H

#define CL_USE_DEPRECATED_OPENCL_1_2_APIS

#include <CL/cl.h>
#include <gdk/gdk.h>
#include <err.h>

#define MAX_SRC_SIZE (0x100000)

typedef struct _coordinates {
	double x, y, z;
} coordinates;

typedef struct _angle {
	double yaw, pitch, roll;
} angle;


typedef struct _environment {
	
	coordinates* camera_pos;
	angle* camera_ang;
	double fov;
	int size_h;
	int size_w;
	cl_context context;
	cl_device_id device_id;
	cl_platform_id platform_id;
	cl_command_queue command_queue;

} environment;


GdkPixbuf* render(environment* env);

environment* create_environment();

void destroy_environment(environment* env);

#endif
