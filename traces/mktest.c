#include <stdio.h>
#include <stdlib.h>
#include <time.h>

int main (void) {
	int i;
	float t0, dt, dl;
	FILE *out;
	out = fopen ("lilIn", "w");

	for (i = 0; i < 100; i++) {
		t0 = rand() % 20;
		dt = rand() % 50;
		dl = t0 + dt + (rand () % 50);
		fprintf (out, "%.2f proc%d %.2f %.2f\n", t0, i, dt, dl);	
	}
	fclose (out);
	return 0;
}
