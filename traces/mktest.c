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
	int i, num, var;
	float t0, dt, dl;
	num = 100;
	var = 5;

	/* numero de iterações */
	if(argc > 1) {
		num = atoi(argv[1]);
	}

	/* variador de intervalo */
	if(argc > 2) {
		var = atoi(argv[2]);
	}

	srand(time(NULL));
	for (i = 0; i < num; i++) {
		t0 = rand() % (20 + var);
		dt = rand() % (50 + var);
		dl = t0 + dt + (rand () % (50 + var));
		printf ("%.2f proc%d %.2f %.2f\n", t0, i, dt, dl);	
	}
	return 0;
}
