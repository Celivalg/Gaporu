#include "opencl_load.h"

#define M_PI	3.14159265358979323846

GdkPixbuf* render(environment* env)
{

	GdkPixbuf* img = gdk_pixbuf_new(GDK_COLORSPACE_RGB, FALSE, 8, env->size_w,  
                                                                env->size_h);   
                                                                                
    size_t img_rowstride = gdk_pixbuf_get_rowstride(img);                       
    uint8_t* img_pixels = gdk_pixbuf_get_pixels(img);
	for (size_t i = 0; i < env->size_h * img_rowstride; i++)
	{
		img_pixels[i] = 0;
	}



	cl_int ret;
	// Allocate memory buffers on the device the size of the image
	cl_mem pixel_buf = clCreateBuffer(env->context, CL_MEM_WRITE_ONLY, 
				env->size_h * env->size_w * 3 * sizeof(char), NULL, &ret);
	cl_mem vectors = clCreateBuffer(env->context, CL_MEM_READ_WRITE,
				env->size_h * env->size_w * 3 * sizeof(float), NULL, &ret);


	cl_mem cam = clCreateBuffer(env->context, CL_MEM_COPY_HOST_PTR,
				sizeof(camera), env->cam, &ret);
	if (ret != CL_SUCCESS) { err(ret, "render: building memory error: %d", ret);}
	// Build program from source
	cl_program program = compile_program_from_source(env, "src/renderer.cl");


	cl_ulong rowstride = img_rowstride;                                         
    cl_ulong size_h = env->size_h;                                              
    cl_ulong size_w = env->size_w;	
	cl_float fov = env->fov * M_PI / 180.0;

	// Build vectors
	cl_kernel vec_kernel = clCreateKernel(program, "camera_vector", &ret);

	
	ret = clSetKernelArg(vec_kernel, 0, sizeof(cl_mem), (void *)&vectors);
	if (ret != CL_SUCCESS) { err(ret, "render: 0 CL error: %d", ret);}
	ret = clSetKernelArg(vec_kernel, 1, sizeof(cl_mem), (void *)&cam);
	if (ret != CL_SUCCESS) { err(ret, "render: 1 CL error: %d", ret);} 
	ret = clSetKernelArg(vec_kernel, 2, sizeof(float), &fov);
	if (ret != CL_SUCCESS) { err(ret, "render: 2 CL error: %d", ret);}
	ret = clSetKernelArg(vec_kernel, 3, sizeof(cl_ulong), &size_h);
	if (ret != CL_SUCCESS) { err(ret, "render: 3 CL error: %d", ret);}
	ret = clSetKernelArg(vec_kernel, 4, sizeof(cl_ulong), &size_w);
	if (ret != CL_SUCCESS) { err(ret, "render: 4 CL error: %d", ret);}

	size_t global_item_size = env->size_h * env->size_w;
	ret = clEnqueueNDRangeKernel(env->command_queue, vec_kernel, 1, NULL,
							&global_item_size, NULL, 0, NULL, NULL);
	clFlush(env->command_queue);
	clFinish(env->command_queue);


	// Create opencl kernel
	cl_kernel kernel = clCreateKernel(program, "renderer", &ret);
	if (ret != CL_SUCCESS) { err(ret, "render: CL error");}
	// Set arguments for kernel

	ret = clSetKernelArg(kernel, 0, sizeof(cl_mem), (void *)&pixel_buf);
	if (ret != CL_SUCCESS) { err(ret, "render: 0 CL error: %d", ret);}
	ret = clSetKernelArg(kernel, 1, sizeof(cl_mem), (void *)&vectors);
	if (ret != CL_SUCCESS) { err(ret, "render: 1 CL error: %d", ret);}
	ret = clSetKernelArg(kernel, 2, sizeof(cl_mem), (void *)&cam);
	if (ret != CL_SUCCESS) { err(ret, "render: 2 CL error: %d", ret);}

	// Execute kernel
	ret = clEnqueueNDRangeKernel(env->command_queue, kernel, 1, NULL,
						&global_item_size, NULL, 0, NULL, NULL);
	if (ret != CL_SUCCESS) { err(ret, "render: CL error: %d", ret);}

	for (size_t i = 0; i < env->size_h; i++)
	{
		ret = clEnqueueReadBuffer(env->command_queue, pixel_buf, CL_TRUE, 
			i * env->size_w * 3, env->size_w * 3, 
									&(img_pixels[i * rowstride]), 0, NULL, NULL);


		if (ret != CL_SUCCESS) { err(ret, "render: CL error: %d", ret);}
	}
	// Read the memory buffer
	ret = clFlush(env->command_queue);
	ret = clFinish(env->command_queue);
	ret = clReleaseKernel(kernel);
	ret = clReleaseProgram(program);
	ret = clReleaseMemObject(pixel_buf);
	
	return img;
}

environment* create_environment()
{
	environment* env = malloc(sizeof(environment));
	
	env->cam = malloc(sizeof(camera));

	env->cam->x = 0.0;
	env->cam->y = 0.0;
	env->cam->z = 0.0;

	env->cam->pitch = 0;
	env->cam->yaw = 0;

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

	free(env->cam);
	free(env);
}

cl_program compile_program(environment* env, char* source, size_t source_size)
{
	cl_int error;
	cl_program program = clCreateProgramWithSource(env->context, 1, 
								(const char**) &source, &source_size, &error);
	if (error != CL_SUCCESS)                                                    
    {                                                                           
        err(error, "compile_program: clCreateProgramWithSource "                  
                    "failed with error code %d", error);                        
    }


	error = clBuildProgram(program, 1, &(env->device_id), NULL, NULL, NULL);
	
	if (error == CL_BUILD_PROGRAM_FAILURE)
    {
        size_t log_size;
        error = clGetProgramBuildInfo(program, env->device_id,
									CL_PROGRAM_BUILD_LOG, 0, NULL, &log_size);
		if (error != CL_SUCCESS)
		{
			err(error, "compile_program: clGetProgramBuildInfo failed to "
							"recover log size with error code: %d ", error);
		}


        char* log = malloc(log_size);
        error = clGetProgramBuildInfo(program, env->device_id,
									CL_PROGRAM_BUILD_LOG, log_size, log, NULL);

		if (error != CL_SUCCESS)                                                
        {                                                                       
            err(error, "compile_program: clGetProgramBuildInfo failed to "
								"recover log with error code: %d ", error);                              
        }


        printf("render: Failed to build: \n%s\n", log);
		clReleaseProgram(program);
		return NULL;
		
    }
	else if (error != CL_SUCCESS)                                                    
    {                                                                           
        err(error, "compile_program: clBuildProgram "                  
                    "failed with error code %d", error);                        
    }

	return program;
}

cl_program compile_program_from_source(environment* env, char* source_path)
{
	FILE *fp;

    fp = fopen("src/renderer.cl", "r");
    if (!fp)
    {
        err(1, "compile_program_from_source: Couldn't load source code %s\n",
																source_path);
    }
    char* source_str = malloc(MAX_SRC_SIZE);
    size_t source_size = fread(source_str, 1, MAX_SRC_SIZE, fp);
    fclose(fp);

	if (source_size >= MAX_SRC_SIZE)
	{
		printf("compile_program_from_source: MAX_SRC_SIZE exceeded: %d\n",
																MAX_SRC_SIZE);
		free(source_str);
		return NULL;
	}

	cl_program program = compile_program(env, source_str, source_size);
	free(source_str);

	return program;
}
