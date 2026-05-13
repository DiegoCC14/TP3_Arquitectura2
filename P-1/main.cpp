/*
Escribir un programa que calcule el logaritmo natural de números mayores a
1000000000 (1*109) en punto flotante de doble precisión largo (tipo long double en
C++) mediante serie de Taylor, empleando ###100000000 (cien millones)### de términos
de dicha serie. El resultado debe imprimirse con 15 dígitos (ver anexo). Resuelva el
problema de dos formas:
● Sin emplear multihilos.
● Empleando múltiples hilos que trabajen concurrentemente, resolviendo cada
hilo una parte de los ###100 millones### de términos de la serie de Taylor
(sugerencia, utilizar como número de hilos divisores de 10000000).
Tanto el operando del logaritmo natural como la cantidad de hilos a emplear deben
ingresarse por teclado.
a) Incluya código que permita obtener el tiempo de ejecución en cada programa, y
calcule el speedup (ver Anexo 1.4).
b) Observe el porcentaje de uso de cada núcleo en cada implementación (ver Anexo
1.6). Obtenga el valor de IPC (instrucciones por ciclo) con el comando perf (ver
Anexo 4).
c) Observe la eficiencia en el uso de la memoria caché. Observe los fallos en la
lectura de la caché de mayor nivel, usualmente L3 (parámetro conocido como Last
level load misses o LLC-load-misses) y los fallos en la lectura de la caché L1 de
datos (conocido como L1-dcache-load-misses) (ver Anexo 4).
d) ¿Cómo clasificaría este problema, CPU-bound o memory-bound?
Serie de Taylor del logaritmo natural:
*/

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