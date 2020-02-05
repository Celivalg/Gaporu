double sphere(double x, double y, double z)
{
	return sqrt(x * x + y * y + z * z) - 0.5;
}


__kernel void render(__global uchar* img, ulong rowstride, ulong height, ulong width)
{	
	unsigned long i = get_global_id(0);
	unsigned long w = i % height;
	unsigned long h = (i - w) / width;
	
	double x = -1;
	double y = ((height / 2.0) - h) / 100;
	double z = ((width / 2.0) - w) / 100;
	

	double y_ang_r = M_PI_F / 4 * (y / (height/(2 * 100)));
	double z_ang_r = M_PI_F / 4 * (z / (width/(2 * 100)));

	double y_step = sin(y_ang_r);
	double z_step = sin(z_ang_r);
	double x_step = cos(y_ang_r) * cos(z_ang_r);

	double l = 0;

	double radius = 0.5;
	
	while (l < 100){
		double r = sphere(x, y, z);
		
		x += x_step * r;
		y += y_step * r;
		z += z_step * r;
		l += r;
		if (r < 0.0001){
			break;
		}
	}
	if (l < 100)
	{
		uchar n = 255 - ((l - 1 + radius)/radius) * 255;
		img[h * rowstride + w * 3] = n;
		img[h * rowstride + w * 3 + 1] = n;
        img[h * rowstride + w * 3 + 2] = 0;
	} 
	else
	{
		img[h * rowstride + w * 3] = 255;
		img[h * rowstride + w * 3 + 1] = 255;
		img[h * rowstride + w * 3 + 2] = 255;
	}
	

}
