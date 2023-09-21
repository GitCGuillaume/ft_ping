#include <stdio.h>

double   ftSqrt(double num) {
    double x = num;
    double old = 0;

    if (num < 0 || num == 0)
        return (0.0d);
    while (x != old){
        old = x;
        x = (x + num / x) / 2;
    }
    return (x);
}


int	main(void) {
	double arr[] = {0.580, 1.012, 0.339};
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
