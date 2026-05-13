#include <mpi.h>

#include <iostream>
#include <unistd.h>

#include <cstring>
#include <string>
#include <algorithm>

#include <chrono>

#include <iomanip>
#include <cmath>

#include "getIP.cpp"

using namespace std;

#include <vector>


bool esPrimo(long long int x) {
    if (x < 2) return false;
    for (long long int i = 2; i * i <= x; i++) {
        if (x % i == 0) return false;
    }
    return true;
}

vector<int> primosAnteriores( int start , int end ) {
    vector<int> resultado;
    for (int i = start; i < end; i++) {
        if (esPrimo(i)) {
            resultado.push_back(i);
        }
    }
    return resultado;
}


int main(int argc, char **argv)
{

    // ENTRADA DE DATOS ------------------------------------------------------------------------
    int NumeroLimite = stoi(argv[1]);  // tamaño de las matrices, ingresado por consola
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

    // ------------------------------------------------
    // divide las filas de la matriz resultado entre los procesos
    int tamanioSeccion = NumeroLimite/size;
    
    int limit_inferior = rank * tamanioSeccion; 
    int limit_superior = (rank == size - 1) ? NumeroLimite : limit_inferior+tamanioSeccion;
    // ------------------------------------------------
    
    vector<int> vectorResultado = primosAnteriores( limit_inferior , limit_superior );
    
    vector<int> vectorResultadoFinal;

    // =======

    if (rank == 0) {
        vectorResultadoFinal = vectorResultado;

        // recibir del resto de los rank menos 0 
        for (int p = 1; p < size; p++) {
            int cantidad;
            MPI_Recv( &cantidad, 1, MPI_INT, p, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE );

            vector<int> buffer(cantidad);

            MPI_Recv( buffer.data(), cantidad, MPI_INT, p, 1, MPI_COMM_WORLD, MPI_STATUS_IGNORE
            );

            vectorResultadoFinal.insert(
                vectorResultadoFinal.end(),
                buffer.begin(),
                buffer.end()
            );
        }
    }
    else {
        int cantidad = vectorResultado.size();
        MPI_Send( &cantidad, 1, MPI_INT, 0, 0, MPI_COMM_WORLD );
        MPI_Send( vectorResultado.data(), cantidad, MPI_INT, 0, 1, MPI_COMM_WORLD );
    }

    // =======
    
    if (rank==0){ sort( vectorResultadoFinal.begin(), vectorResultadoFinal.end() ); }

    auto end_time = chrono::high_resolution_clock::now();
    auto duration = chrono::duration_cast<chrono::milliseconds>(end_time - start_time);

    if ( rank == 0 ){ //Primeras Filas

        cout << "10 Ultimos Primos: " << std::endl;
        int contador = 0;
        for (int x = vectorResultadoFinal.size()-1 ; x>=0 ; x-- ) {
            contador ++;
            cout << vectorResultadoFinal[x] << " , ";
            if (contador == 10) { break; }
        }
        cout << "\nCantidad de Primos: "<< vectorResultadoFinal.size() << std::endl;
        cout << "Tiempo de ejecución: " << duration.count() << " ms\n" << endl;
    }

    if (MPI_Finalize() != MPI_SUCCESS)
    {
        cout << "Error finalizando MPI" << endl;
        return 1;
    }
    
    return 0;
}