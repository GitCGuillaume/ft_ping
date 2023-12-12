#include <stdio.h>

double   ftSqrt(double num) {
    //x is whatever value near square root
    double x = num;
    double old = 0.0;

    if (num < 0 || num == 0)
        return (0.0);
    while (x != old){
        old = x;
        x = (x + num / x) / 2.0;
    }
    return (x);
}


int	main(void) {
	double arr[] = {29.196, 10.998, 10.524};
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
	printf("d:%.90f\n", 2.0);
	printf("%.3f %.3f %.3f\n", sum, squareSum, stdDev);
	return (0);
}
