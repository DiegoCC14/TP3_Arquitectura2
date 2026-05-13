#include <string>
#include <iostream>
#include <chrono>
#include "ln_sin_hilos.cpp"
#include "ln_con_hilos.cpp"

using namespace std;

int main() {

    // ENTRADA DE DATOS ------------------------------------------------------------------------
    string inputX;
    cout << "Ingrese Valor de Logaritmo Natural a Calcular: ";
    getline(cin, inputX);
    long double x = stold(inputX);

    string input_num_threads;
    cout << "Ingrese Numero Hilos a Ejecutar: ";
    getline(cin, input_num_threads);
    int num_threads = stoi(input_num_threads);
    //------------------------------------------------------------------------------------------
    cout << "\nEjecucion Sin Hilos...\n";

    auto start_time1 = chrono::high_resolution_clock::now(); //empieza el clock
    
    long double result1 = cal_series(x);

    auto end_time1 = chrono::high_resolution_clock::now();
    auto duration1 = chrono::duration_cast<chrono::milliseconds>(end_time1 - start_time1);

    cout << "Sin Hilos:";
    cout << "\n - Resultado: " << setprecision(15) << result1;
    cout << "\n - Tiempo de ejecucion: " << duration1.count() << " ms\n\n";
    
    //------------------------------------------------------------------------------------------
    cout << "Ejecucion Multiples Hilos:";

    auto start_time2 = chrono::high_resolution_clock::now(); //empieza el clock
    
    long double result2 = cal_series_threads(num_threads, x);

    auto end_time2 = chrono::high_resolution_clock::now();
    auto duration2 = chrono::duration_cast<chrono::milliseconds>(end_time2 - start_time2);

    cout << "\n - Resultado: " << setprecision(15) << result2;
    cout << "\n - Tiempo de ejecucion: " << duration2.count() << " ms\n\n";
    
    //------------------------------------------------------------------------------------------
    
    double speed_up = ((double) duration1.count())/duration2.count(); //solo en 1 porque automáticamente castea el de abajo
    cout << "Speed up: " << speed_up << endl;
    
    return 0;
    
}