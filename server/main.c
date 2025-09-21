#include <racer.h>

double map_step;
double map_width;
double* map_height = NULL;

int main(){
	FILE* f = fopen("map.txt", "r");
	if(f != NULL){
		double w, d;
		int i;

		fscanf(f, "%lf %lf", &map_width, &map_step);

		w = map_width / map_step;

		printf("map_width=%f, map_step=%f\n", map_width, map_step);
		printf("expecting %d datas\n", (int)(w * w));
		for(i = 0; i < w * w; i++){
			fscanf(f, "%lf", &d);
			arrput(map_height, d);
		}
		printf("loaded\n");

		fclose(f);
	}else{
		fprintf(stderr, "map.txt missing\n");
		return 1;
	}

	printf("server booting\n");

	physics_init();
	net_init();
	net_loop();
}
