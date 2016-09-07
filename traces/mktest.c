#include <stdio.h>
#include <stdlib.h>
#include <time.h>

int main (int argc, char** argv) {
	int i, num, var;
	float t0, dt, dl;
	num = 100;
	var = 0;

	/* numero de iterações */
	if(argc > 1) {
		num = atof(argv[1]);
	}

	/* variador de intervalo */
	if(argc > 2) {
		var = atof(argv[2]);
	}

	srand(time(NULL));
	for (i = 0; i < num; i++) {
		t0 = rand() % (20 + var*5);
		dt = rand() % (50 + var*5);
		dl = t0 + dt + (rand () % (50 + var*5));
		printf ("%.2f proc%d %.2f %.2f\n", t0, i, dt, dl);	
	}
	return 0;
}
