#include "opencl_load.h"

#define CL_USE_DEPRECATED_OPENCL_1_2_APIS


GdkPixbuf* render(environment* env)
{

	GdkPixbuf* img = gdk_pixbuf_new(GDK_COLORSPACE_RGB, FALSE, 8, env->size_w,  
                                                                env->size_h);   
                                                                                
    size_t img_rowstride = gdk_pixbuf_get_rowstride(img);                       
    uint8_t* img_pixels = gdk_pixbuf_get_pixels(img);
	for (size_t i = 0; i < env->size_h * img_rowstride; i++)
	{
		img_pixels[i] = 128;
	}

	// Extract source from opencl source file into a buffer
	FILE *fp;
	char *source_str;
	size_t source_size;

	fp = fopen("src/renderer.cl", "r");
	if (!fp)
	{
		err(1, "render: Couldn't load source code \"renderer.cl\"\n");
	}
	source_str = malloc(MAX_SRC_SIZE);
	source_size = fread(source_str, 1, MAX_SRC_SIZE, fp);
	fclose(fp);

	cl_int ret;
	// Allocate memory buffers on the device the size of the image
	cl_mem mem = clCreateBuffer(env->context, CL_MEM_WRITE_ONLY, 
				env->size_h * img_rowstride * sizeof(char), NULL, &ret);

	// Build program from source
	cl_program program = clCreateProgramWithSource(env->context, 1,
			(const char **)&source_str, (const size_t *) &source_size, &ret);
	ret = clBuildProgram(program, 1, &(env->device_id), NULL, NULL, NULL);
	if (ret == CL_BUILD_PROGRAM_FAILURE) 
	{ 
		size_t log_size;
		clGetProgramBuildInfo(program, env->device_id, CL_PROGRAM_BUILD_LOG, 
														0, NULL, &log_size);
		char* log = malloc(log_size);
		clGetProgramBuildInfo(program, env->device_id, CL_PROGRAM_BUILD_LOG,
														log_size, log, NULL);
		err(1, "render: Failed to build: \n%s", log);


	}
	if (ret != CL_SUCCESS) { err(ret, "render: CL error");}
	// Create opencl kernel
	cl_kernel kernel = clCreateKernel(program, "render", &ret);
	if (ret != CL_SUCCESS) { err(ret, "render: CL error");}
	// Set arguments for kernel
	
	cl_ulong rowstride = img_rowstride;
	cl_ulong size_h = env->size_h;
	cl_ulong size_w = env->size_w;

	ret = clSetKernelArg(kernel, 0, sizeof(cl_mem), (void *)&mem);
	if (ret != CL_SUCCESS) { err(ret, "render: CL error");}
	ret = clSetKernelArg(kernel, 1, sizeof(cl_ulong), &rowstride);
	if (ret != CL_SUCCESS) { err(ret, "render: CL error");}
	ret = clSetKernelArg(kernel, 2, sizeof(cl_ulong), &size_h);
	if (ret != CL_SUCCESS) { err(ret, "render: CL error");}
	ret = clSetKernelArg(kernel, 3, sizeof(cl_ulong), &size_w);
	if (ret != CL_SUCCESS) { err(ret, "render: CL error");}

	// Execute kernel
	size_t global_item_size = env->size_h * env->size_w;
	size_t local_item_size = 64;
	ret = clEnqueueNDRangeKernel(env->command_queue, kernel, 1, NULL,
						&global_item_size, &local_item_size, 0, NULL, NULL);
	if (ret != CL_SUCCESS) { err(ret, "render: CL error");}

	
	ret = clEnqueueReadBuffer(env->command_queue, mem, CL_TRUE, 0,
								env->size_h * img_rowstride * sizeof(char), 
								img_pixels, 0, NULL, NULL);
	if (ret != CL_SUCCESS) { err(ret, "render: CL error");}
	//Read the memory buffer
	ret = clFlush(env->command_queue);
	ret = clFinish(env->command_queue);
	ret = clReleaseKernel(kernel);
	ret = clReleaseProgram(program);
	ret = clReleaseMemObject(mem);
	
	return img;
}

environment* create_environment()
{
	environment* env = malloc(sizeof(environment));

	env->camera_pos = malloc(sizeof(coordinates));
	
	env->camera_pos->x = 0.0;
	env->camera_pos->y = 0.0;
	env->camera_pos->z = 0.0;

	env->camera_ang = malloc(sizeof(angle));
	
	env->camera_ang->pitch = 0.0;
	env->camera_ang->yaw = 0.0;
	env->camera_ang->roll = 0.0;

	env->fov = 70;

	env->size_h = 1080;
	env->size_w = 1920;
	


	env->platform_id = NULL;
	env->device_id = NULL;
	cl_uint num_devices, num_platforms;
	
	// Getting platform and device informations
	cl_int error = clGetPlatformIDs(1, &(env->platform_id), &num_platforms);
	if (error != CL_SUCCESS)
	{
		err(error, "create_environment: clGetPlatformIDs "
					"failed with error code %d", error);
	}
	
	error = clGetDeviceIDs(env->platform_id, CL_DEVICE_TYPE_ALL, 1,
											&(env->device_id), &num_devices);
	if (error != CL_SUCCESS)                                                    
    {                                                                           
        err(error, "create_environment: clGetDevicesIDs "                      
                    "failed with error code %d", error);                        
    }

	// Creating an opencl context
	env->context = clCreateContext(NULL, 1, &(env->device_id), NULL, NULL,
																	&error);
	if (error != CL_SUCCESS)                                                    
    {                                                                           
        err(error, "create_environment: clCreateContext "                      
                    "failed with error code %d", error);                        
    }
	
	// Creating command queue
	env->command_queue = clCreateCommandQueue(env->context, env->device_id,
																	0, &error);
	if (error != CL_SUCCESS)                                                    
    {                                                                           
        err(error, "create_environment: clCreateCommandQueue "                      
                    "failed with error code %d", error);                        
    }

	return env;
}

void destroy_environment(environment* env)
{
	clFlush(env->command_queue);
	clFinish(env->command_queue);
	clReleaseCommandQueue(env->command_queue);
	clReleaseContext(env->context);

	free(env->camera_pos);
	free(env->camera_ang);
	free(env);
}
