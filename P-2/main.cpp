#include <mpi.h>

#include <iostream>
#include <unistd.h>

#include <cstring>
#include <string>
#include <algorithm>

#include <chrono>

#include <iomanip>
#include <cmath>
#include <fstream>

#include "getIP.cpp"

using namespace std;

#include <vector>


int count_pattern_occurrences( string pattern, string text ) {
    int count = 0; //conteo de ocurrencias
    for (int i = 0; i <= text.size() - pattern.size(); i++) {
        if (text.substr(i, pattern.size()) == pattern) {
            count++;
        }
    }
    return count;
}

int main(int argc, char **argv)
{

    // ENTRADA DE DATOS ------------------------------------------------------------------------
    //int NumeroLimite = stoi(argv[1]);  // tamaño de las matrices, ingresado por consola
    //------------------------------------------------------------------------------------------

    if (MPI_Init(&argc, &argv) != MPI_SUCCESS)
    {
        cout << "Error iniciando MPI" << endl;
        return 1;
    }

    int rank; //Numero Proceso Asignado
    int size; //Numero Total de Procesos

    MPI_Comm_rank(MPI_COMM_WORLD, &rank); //MPI Comunicacion, se asigna valor a rank

    MPI_Comm_size(MPI_COMM_WORLD, &size); //MPI Comunicacion, se asigna valor a size

    char hostname[256];

    gethostname(hostname, sizeof(hostname)); //Nombre donde hosteamos el proceso

    string ipLocal = obtenerIPLocal();

    // ==========================================================================>>>>>>>>>>>>>>>>

    auto start_time = chrono::high_resolution_clock::now();  // empieza el clock
    cout << "\nCalculando en proceso " << rank << " - IP= " << ipLocal << endl;

    // CARGA DE DATOS -------------------------------------------------------------------------
    string pattern;
    vector<string> patterns;
    string text;

    ifstream patterns_file("patrones.txt");
    while (getline(patterns_file, pattern)) {
        patterns.push_back(pattern);
    }
    patterns_file.close();
    
    ifstream text_file("texto.txt");
    getline(text_file, text);
    text_file.close();
    
    int num_patterns = patterns.size();
    //------------------------------------------------------------------------------------------

    // ------------------------------------------------
    // divide las filas de la matriz resultado entre los procesos
    int tamanioSeccion = num_patterns/size;
    
    int limit_inferior = rank * tamanioSeccion; 
    int limit_superior = (rank == size - 1) ? num_patterns : limit_inferior+tamanioSeccion;

    vector<string> patternsRecortado( patterns.begin()+limit_inferior, patterns.begin()+limit_superior );
    // ------------------------------------------------
    
    vector<int> vectorPatronCoincidencias;
    string patronActual;
    for (int x=0;x<patternsRecortado.size();x++ ){
        patronActual = patternsRecortado[x]; 
        vectorPatronCoincidencias.push_back( count_pattern_occurrences( patronActual, text ) );
    }

    vector<int> vectorResultadoFinal;
    // =======

    if (rank == 0) {

        vectorResultadoFinal = vectorPatronCoincidencias;

        // recibir del resto de los rank menos 0 
        for (int p = 1; p < size; p++) {
            int cantidad;
            MPI_Recv( &cantidad, 1, MPI_INT, p, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE );

            vector<int> buffer(cantidad);

            MPI_Recv( buffer.data(), cantidad, MPI_INT, p, 1, MPI_COMM_WORLD, MPI_STATUS_IGNORE );

            vectorResultadoFinal.insert( vectorResultadoFinal.end(), buffer.begin(), buffer.end() );
        }
    }
    else {
        int cantidad = vectorPatronCoincidencias.size();
        MPI_Send( &cantidad, 1, MPI_INT, 0, 0, MPI_COMM_WORLD );
        MPI_Send( vectorPatronCoincidencias.data(), cantidad, MPI_INT, 0, 1, MPI_COMM_WORLD );
    }

    // =======

    auto end_time = chrono::high_resolution_clock::now();
    auto duration = chrono::duration_cast<chrono::milliseconds>(end_time - start_time);

    if ( rank == 0 ){ //Primeras Filas
        
        for (int i = 0; i < vectorResultadoFinal.size(); i++) {
            cout << "   El patron " << i << " " << patterns[i] << " aparece " << vectorResultadoFinal[i] << " veces." << endl;
        }
        
        cout << "Tiempo de ejecución: " << duration.count() << " ms\n" << endl;
    }
    
    if (MPI_Finalize() != MPI_SUCCESS)
    {
        cout << "Error finalizando MPI" << endl;
        return 1;
    }
    
    return 0;
}