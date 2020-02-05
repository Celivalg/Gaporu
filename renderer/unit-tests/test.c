#include "../src/opencl_load.h"

#include <stdlib.h>
#include <stdio.h>
#include <gtk/gtk.h>

int main(){


	environment* env = create_environment();
	env->size_h = 1000;
	env->size_w = 1000;

	GdkPixbuf* img = render(env);

	gdk_pixbuf_save(img, "out.bmp", "bmp", NULL, NULL);
	destroy_environment(env);
	return 0;
}
