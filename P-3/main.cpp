#include <mpi.h>

#include <iostream>
#include <unistd.h>

#include <cstring>
#include <string>

#include <chrono>

#include <iomanip>
#include <cmath>

#include "getIP.cpp"

using namespace std;

#include <vector>


vector<vector<float>> multiplicacion_matrices( vector<vector<float>> matrix1, vector<vector<float>> matrix2, int startRow , int endRow ) {
    int N = matrix1.size();
    int filas = endRow - startRow;
    vector<vector<float>> result(filas, vector<float>(N, 0));

    for (int i = startRow; i < endRow; i++) {
        for (int j = 0; j < N; j++) {
            for (int k = 0; k < N; k++) {
                result[i - startRow][j] += matrix1[i][k] * matrix2[k][j];
            }
        }
    }

    return result;
}


int main(int argc, char **argv)
{

    // ENTRADA DE DATOS ------------------------------------------------------------------------
    int N = stoi(argv[1]);  // tamaño de las matrices, ingresado por consola
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
    int rows_per_process = N / size;
    
    // Dividimos en filas la matriz para asignar a Hilo
    int start_row = rank * rows_per_process;
    int end_row = (rank == size - 1) ? N : start_row + rows_per_process;
    // ------------------------------------------------

    vector<vector<float>> matrix1(N, vector<float>(N, 0.1));  // crea la matriz 1 y la llena con 0.1
    vector<vector<float>> matrix2(N, vector<float>(N, 0.2));  // crea la matriz 2 y la llena con 0.2

    vector<vector<float>> matrixResult = multiplicacion_matrices( matrix1 , matrix2 , start_row , end_row );

    float sumaElementosMatriz = 0.0;
    for (int i = 0; i < matrixResult.size(); ++i) {
        for (int j = 0; j<N; ++j) {
            sumaElementosMatriz += matrixResult[i][j];
        }
    }
    
    float sumaElementosMatrizFinal = 0.0;    
    MPI_Reduce( &sumaElementosMatriz , &sumaElementosMatrizFinal , 1, MPI_FLOAT, MPI_SUM, 0, MPI_COMM_WORLD);
    
    auto end_time = chrono::high_resolution_clock::now();
    auto duration = chrono::duration_cast<chrono::milliseconds>(end_time - start_time);

    if ( rank == 0 ){ //Primeras Filas
        cout << "\nResultado Matriz:" << endl;
        cout << " - Esquina superior izquierda: " << matrixResult[0][0] << endl;
        cout << " - Esquina superior derecha: " << matrixResult[0][N - 1] << endl;
    }
    MPI_Barrier(MPI_COMM_WORLD);
    if ( rank == (size-1) ){ //Ultimas Filas
        cout << " - Esquina inferior izquierda: " << matrixResult[ matrixResult.size()-1 ][0] << endl;
        cout << " - Esquina inferior derecha: " << matrixResult[matrixResult.size() - 1][N - 1] << endl;
    }
    MPI_Barrier(MPI_COMM_WORLD);
    if ( rank == 0 ){
        cout << "Sumatoria: " << setprecision(15) << sumaElementosMatrizFinal << endl;
        cout << "Tiempo de ejecución: " << duration.count() << " ms\n" << endl;
    }

    if (MPI_Finalize() != MPI_SUCCESS)
    {
        cout << "Error finalizando MPI" << endl;
        return 1;
    }
    
    return 0;
}