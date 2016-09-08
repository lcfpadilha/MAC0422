#include <stdio.h>
#include <stdlib.h>
#include <time.h>


/*  Algoritmo que gera um arquivo-teste para
	o escalonador. Pode receber ate 2 params:
	- o numero de processos (default 100)
	- um valor que aumenta ou diminui a duraçao
	  e o t0 de cada processo (default 5)
	- o resultado é impress no stdout.
*/

int main (int argc, char** argv) {
	int i, num, var1, var2, N;
	float t0, dt, dl;

	/* numero de iterações */
	if(argc > 1) {
		num = atoi(argv[1]);
	}

	/* variador de intervalo */
	if(argc > 2) {
		var1 = atoi(argv[2]);
		var2 = atoi(argv[3]);
	}

	srand(time(NULL));
	for (i = 0; i < num; i++) {
		t0 = ((float)rand()/(float)(RAND_MAX)) * var1;
		dt = ((float)rand()/(float)(RAND_MAX)) * var2;
		dl = t0 + dt + ((float)rand()/(float)(RAND_MAX)) * num;
		printf ("%.2f proc%d %.2f %.2f\n", t0, i, dt, dl);	
	}
	return 0;
}
