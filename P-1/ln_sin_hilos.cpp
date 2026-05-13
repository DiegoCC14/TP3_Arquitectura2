#include <iostream>
#include <string>
#include <iomanip>
#include <cmath>
#include <chrono>

using namespace std;

// func aux para calcular la serie
long double cal_series(long double x) {
    long double _2n_mas_1; 
    long double result = 0.0;
    long double term = ( (x - 1)/(x + 1) );

    for (long double n = 0; n <= 100000000; ++n) {
        _2n_mas_1 = 2* n + 1;
        result += (1/_2n_mas_1) * pow(term, _2n_mas_1);
    }
    long double final_result = 2*result;
    return final_result;
}