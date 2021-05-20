#include <stdio.h>
#include <assert.h>
#include <math.h>
#include <stdbool.h>
#include <float.h>

#define K 6

float nth_root(float A, int n)
{
	float x[K] = { 1 };

	for (int k = 0; k < K - 1; k++)
		x[k + 1] = (1.0 / n) * ((n - 1) * x[k] + A / pow(x[k], n - 1));
	return x[K - 1];
}

float slow_powf(float base, float ex)
{
	if (base == 0)
		return 0;
	// power of 0
	if (ex == 0) {
		return 1;
		// negative exponenet
	} else if (ex < 0) {
		return 1 / pow(base, -ex);
		// fractional exponent
	} else if (ex > 0 && ex < 1) {
		return nth_root(base, 1 / ex);
	} else if ((int)ex % 2 == 0) {
		float half_pow = pow(base, ex / 2);
		return half_pow * half_pow;
		//integer exponenet
	} else {
		return base * pow(base, ex - 1);
	}
}

/* int main() { */

/*     for (float i = 0; i < 5.5; i+=0.1) { */

/*         float c = slow_powf(2.0, i); */
/*         float d = powf(2.0, i); */
/*         printf("%f <> %f\n", c, d); */
/*     } */

/* 	return 0; */
/* } */
