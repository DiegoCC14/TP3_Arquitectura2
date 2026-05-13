#include <iostream>
#include <iomanip>
#include <cmath>
#include <chrono>
#include <thread>
#include <vector>
#include <string>

using namespace std;

volatile long double result = 0.0;

// función para calcular una parte de la serie de Taylor en paralelo
void cal_series_part(long double term, int start, int end) {
    long double _2n_mas_1;
    long double partial_result = 0.0;
     
    for (int i = start; i < end; ++i) {
        long double n = (long double) i;
        _2n_mas_1 = 2* n + 1;
        partial_result += (1/_2n_mas_1) * pow(term, _2n_mas_1);
    }
    result += partial_result;
}

long double cal_series_threads(int num_threads, long double x) {
    vector<thread> threads(num_threads);

    int n_terminos = 99999999; //Iniciamos en 0, son 100000000 terminos 

    int step = (n_terminos / num_threads);
    int start = 0;
    int end = step;

    long double term = ((x - 1)/(x + 1));
    
    //Creamos los Hilos
    for (int i = 0; i < num_threads; ++i) {
        if ( i+1 == num_threads ){
            end = n_terminos;
        }
        threads[i] = thread( cal_series_part, term, start, end); //Creamos Hilo y Ejecutamos
        start = end + 1;
        end += step;
    }

    for (int i = 0; i < num_threads; ++i) {
        if (threads[i].joinable()){ //True si el obj es Hilo y aun no ejecuto join().
            threads[i].join(); //Bloquea el Hilo, hasta que finalice, para todos los hilos.
        }
    }
    
    return 2 * result;  // El logaritmo natural es el doble serie
}