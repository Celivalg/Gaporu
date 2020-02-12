#include "../src/opencl_load.h"

#include <stdlib.h>
#include <stdio.h>
#include <gtk/gtk.h>


#define M_PI    3.14159265358979323846

int main(){


	environment* env = create_environment();

	env->cam->yaw = M_PI * 1;
	env->cam->pitch = M_PI * -0.1;
	env->cam->x = 10;
	env->cam->z = 5;
	env->cam->y = 0;
	env->fov = 70;

	GdkPixbuf* img = render(env);

	gdk_pixbuf_save(img, "out.bmp", "bmp", NULL, NULL);
	destroy_environment(env);
	return 0;
}
