#include <stdio.h>
#define EPSILON 1.e-10

double   ftSqrt(double num) {
    if (num < 0.0)
        num *= -1.0;
    //x is whatever value near square root
    double x = num;
    double old = 0.0;
    double  eqZ = num;

    if (num < 0.0 || num == 0.0)
        return (0.0);
    while (x != old) {
        old = x;
        x = (x + num / x) * 0.5;
        eqZ = (x * x) - num;
        if (eqZ < 0.0)
            eqZ *= -1.0;
		if (eqZ < EPSILON)
			break ;
    }
    return (x);
}

int	main(void) {
	double arr[] = {0.041, 0.178, 0.167};
	double sum = 0.000000;
	double squareSum = 0.000000;
	double stdDev;
	double average;

	for (int i = 0; i < 3; ++i) {
		sum += arr[i];
		squareSum += arr[i] * arr[i];
	}
	average = sum / 3;
	stdDev = ftSqrt((squareSum / 3) - (average * average));
	printf("%.3f %.3f %.3f\n", sum, squareSum, stdDev);
	return (0);
}
