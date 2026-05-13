// Cree un programa que busque la cantidad de veces que cada patrón
// aparece en el archivo “texto.txt”. El programa deberá generar una salida similar a la
// siguiente:

// el patron 0 aparece 14 veces
// el patron 1 aparece 3 veces
// el patron 2 aparece 0 veces
// el patron 3 aparece 0 veces
// el patron 4 aparece 0 veces
// el patron 5 aparece 0 veces
// .................
// .................

// El archivo “texto.txt” no posee saltos de línea (es decir, posee solo una línea).

// Resuelva el problema de dos formas:
// ● Sin emplear multihilos.
// ● Mediante 32 hilos que trabajen concurrentemente, de modo que cada hilo busque un patrón.

// a) Incluya código que permita obtener el tiempo de ejecución en cada programa, y
// calcule el speedup (ver Anexo 1.4).
// b) Observe el porcentaje de uso de cada núcleo en cada implementación (ver Anexo 1.6).

// Ayuda: El patrón 0 aparece 14 veces, 
// el patrón 1 aparece 3 veces, 
// el patrón 9 aparece 3622 veces, 
// el patrón 11 aparece 2 veces, 
// el patrón 13 aparece 6 veces, 
// el patrón 16 aparece 2 veces, 
// el patrón 18 aparece 6 veces, 
// el patrón 21 aparece 2 veces, 
// el patrón 27 aparece 6 veces, 
// todos los demás patrones aparecen 0 veces.

#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <chrono>
#include <thread>

using namespace std;

vector<string> patterns;
string text;

int count_pattern_occurrences( string pattern, string text ) {
    int count = 0; //conteo de ocurrencias
    for (int i = 0; i <= text.size() - pattern.size(); i++) {
        if (text.substr(i, pattern.size()) == pattern) {
            count++;
        }
    }
    return count;
}


// busca un patrón en el texto utilizando hilos
void search_pattern_in_thread( string text , string pattern , int pattern_index , vector<string> resultado_hilos ) { //pattern_index indica el índice del patrón que debe buscar
    int count = count_pattern_occurrences( pattern, text);
    resultado_hilos[pattern_index] = "El patron " + to_string(pattern_index) + " " + pattern + " aparece " + to_string(count) + " veces.";
}

int main() {
    
    // CARGA DE DATOS -------------------------------------------------------------------------
    ifstream patterns_file("patrones.txt");
    string pattern;
    while (getline(patterns_file, pattern)) {
        patterns.push_back(pattern);
    }
    patterns_file.close();
    
    ifstream text_file("texto.txt");
    getline(text_file, text);
    text_file.close();
    
    int num_patterns = patterns.size();
    //------------------------------------------------------------------------------------------

    //------------------------------------------------------------------------------------------ 
    cout << "Ejecucion Sin Hilos:\n";
    
    auto start = chrono::high_resolution_clock::now(); //high_resolution_clock::now() para obtener una marca de tiempo de alta resolución
    
    for (int i = 0; i < num_patterns; i++) {
        int count = count_pattern_occurrences( patterns[i] , text );
        cout << "   El patron " << i << " " << patterns[i] << " aparece " << count << " veces." << endl;
    }
    
    auto end = chrono::high_resolution_clock::now();
    chrono::duration<double> duration = end - start;
    cout << "\n   Tiempo sin hilos: " << duration.count() << " segundos." << endl;
    //------------------------------------------------------------------------------------------
    
    //------------------------------------------------------------------------------------------
    
    cout << "\nEjecucion Con Hilos:";
    
    auto start_threads = chrono::high_resolution_clock::now();
    
    vector<thread> threads; //almacena objetos de tipo thread
    vector<string> resultado_hilos;
    
    //CONTAMOS CON 32 PATRONES, ENTONCES CREAMOS 1 HILO POR PATRON
    for (int i = 0; i < num_patterns; i++) { 
        resultado_hilos.push_back("");
        threads.push_back( thread( search_pattern_in_thread, text , patterns[i] , i , resultado_hilos ) ); //crea un nuevo objeto std::thread 
    }
    
    for (int i = 0; i < num_patterns; i++) { // espera a que todos los hilos terminen su ejecución mediante join()
        threads[i].join();
    }
    
    for (int i = 0; i < num_patterns; i++){
        cout << resultado_hilos[i];    
    }
    
    auto end_threads = chrono::high_resolution_clock::now();
    chrono::duration<double> duration_threads = end_threads - start_threads;
    cout << "\n   Tiempo con hilos: " << duration_threads.count() << " segundos." << endl;
    
    //------------------------------------------------------------------------------------------
    double speed_up = ((double) duration.count())/duration_threads.count(); //solo en 1 porque automáticamente castea el de abajo
    cout << "\nSpeed up: " << speed_up << endl;
    
    return 0;
}