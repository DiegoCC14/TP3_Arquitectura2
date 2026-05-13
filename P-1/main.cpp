#include <mpi.h>

#include <iostream>
#include <unistd.h>

#include <cstring>
#include <string>

#include <chrono>

#include <iomanip>
#include <cmath>

#include "getIP.cpp"
#include "ln_sin_hilos.cpp"
#include "ln_con_hilos.cpp"

using namespace std;


long double cal_series(long double x, int start, int end) {
    long double _2n_mas_1;
    long double result = 0.0;
    long double term = ((x - 1) / (x + 1));

    for (int i = start; i <= end; ++i) {
        _2n_mas_1 = 2 * (long double) i + 1;
        result += pow(term, _2n_mas_1) / _2n_mas_1;
    }
    return 2 * result;
}

int main(int argc, char **argv)
{

    // ENTRADA DE DATOS ------------------------------------------------------------------------
    long double x = stold(argv[1]); // toma valor de argumento ingresado por consola
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

    int num_terms = 10000000;
    int terms_per_process = num_terms / size; //Dividimos los terminos por el tamanio de procesos
    int start_term = rank * terms_per_process; //
    int end_term = (rank == size - 1) ? 9999999 : (rank + 1) * terms_per_process - 1;

    long double result = 0.0;

    //------------------------------------------------------------------------------------------
    auto start_time = chrono::high_resolution_clock::now();  // empieza el clock

    cout << "\nCalculando en proceso " << rank << " - IP= " << ipLocal << endl;
    
    result = cal_series(x, start_term, end_term);

    long double global_result = 0.0;
    MPI_Reduce(&result, &global_result, 1, MPI_LONG_DOUBLE, MPI_SUM, 0, MPI_COMM_WORLD);
    // MPI_Reduce(void* send_data, void* recv_data, int count, MPI_Datatype datatype, MPI_Op op, int root, MPI_Comm communicator)
    // MPI_Reduce toma el valor de result de cada proceso y lo suma en global_result del proceso 0
    
    auto end_time = chrono::high_resolution_clock::now();
    auto duration = chrono::duration_cast<chrono::milliseconds>(end_time - start_time);

    if (rank == 0) {
        cout << "Resultado: " << setprecision(15) << global_result
        << " -> Proceso " << rank << " de " << size
        << " corriendo en la maquina " << hostname
        << " IP= " << ipLocal << endl;

        cout << "Tiempo de ejecución: " << duration.count() << " ms\n" << endl;
        cout << "Cantidad de procesos: " << size << endl;
    }
    
    // ==========================================================================>>>>>>>>>>>>>>>>
    
    if (MPI_Finalize() != MPI_SUCCESS)
    {
        cout << "Error finalizando MPI" << endl;
        return 1;
    }

    return 0;
}