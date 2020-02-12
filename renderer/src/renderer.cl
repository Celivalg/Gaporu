typedef struct _camera { 
    float x, y, z, yaw, pitch;
} camera;

float length(float x, float y, float z)
{
	return sqrt(x * x + y * y + z * z);
}



float sphere(float x, float y, float z)
{
	y = fmod(y,1);
	z = fmod(z, 1);
	y -= copysign(0.5f, y);
	z -= copysign(0.5f, z);


	return sqrt(x * x + y * y + z * z) - 0.5;
}

float DE(float x, float y, float z)
{
	int n = 0;
	int iter = 30.0;
	float Scale = 2.0;
	float xc, yc, zc;
	while (n < iter)
	{
		xc = 1;
		yc = 1;
		zc = 1;
		float dist = length(x - 1, y - 1, z - 1);
		float d = length(x + 1, y + 1, z - 1);
		if (d < dist)
		{
			xc = -1;
			yc = -1;
			zc = 1;
			dist = d;
		}
		d = length(x - 1, y + 1, z + 1);
        if (d < dist)
        {
            xc = 1;
            yc = -1;
            zc = -1;
            dist = d;
        }
		d = length(x + 1, y - 1, z + 1);
        if (d < dist)
        {
            xc = -1;
            yc = 1;
            zc = -1;
        }


		x = x * Scale - xc * (Scale - 1.0);
		y = y * Scale - yc * (Scale - 1.0);
		z = z * Scale - zc * (Scale - 1.0);
		n++;
	}

	return length(x, y, z) * pow(Scale, -(float) iter);

}

__kernel void renderer(__global uchar* img, __global float* vec,
														__global camera* cam)
{
	size_t max_steps = 100;
	unsigned long index = get_global_id(0);

	float x = cam->x;
	float y = cam->y;
	float z = cam->z;

	float vec_x = vec[index * 3];
	float vec_y = vec[index * 3 + 1];
	float vec_z = vec[index * 3 + 2];

	float approx = 100.0;
	size_t steps = 0;

	while (steps < max_steps && approx > 0.001 && approx < 10000.0)
	{
		approx = DE(x, y, z);
		steps++;
		x += vec_x * approx;
		y += vec_y * approx;
		z += vec_z * approx;

	}





	unsigned char color = (steps == max_steps || approx >= 10000.0) ? 255 : 255 * -((((float) steps) / 500.0) - 1);
	img[index * 3] = (steps == max_steps || approx >= 10000.0) ? 128 : color;
	img[index * 3 + 1] = (steps == max_steps || approx >= 10000.0) ? 128 : color;
	img[index * 3 + 2] = color;
}


__kernel void camera_vector(__global float* vec, __global camera* cam,
									float fov, ulong height, ulong width)
{

	unsigned long index = get_global_id(0);

	// Finding pixel postition
	unsigned long w = index % width;
	unsigned long h = (index - w) / (width);

	// Determine effective angles for pixel
	float eff_yaw = fov * ((((float) w) / ((float) width )) - 0.5);
	float eff_pitch = ((float) height) / ((float) width) * fov * 
								((((float) h) / ((float) height )) - 0.5);

	// Determine step vectors
	float p = M_PI_2_F - cam->yaw;
	vec[index * 3 + 1] = sin(p) * tan(eff_yaw) + sin(cam->yaw);
	vec[index * 3 + 2] = sin(M_PI_2_F - cam->pitch) * tan(eff_pitch) + sin(cam->pitch);
	vec[index * 3] = (sin(p) + ((sin(p) * tan(eff_yaw)) / tan(p))) * cos(eff_pitch + cam->pitch);
}
