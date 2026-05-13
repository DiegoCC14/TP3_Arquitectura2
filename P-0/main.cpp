//#include <mpi.h>

#include <iostream>
#include <unistd.h>

#include <arpa/inet.h>
#include <sys/socket.h>

#include <cstring>

#include "getIP.cpp"

using namespace std;

int main(int argc, char **argv)
{

    if (MPI_Init(&argc, &argv) != MPI_SUCCESS)
    {
        cout << "Error iniciando MPI" << endl;
        return 1;
    }

    int rank;
    int size;

    MPI_Comm_rank(MPI_COMM_WORLD, &rank); //MPI Comunicacion

    MPI_Comm_size(MPI_COMM_WORLD, &size); //MPI Comunicacion

    char hostname[256];

    gethostname(hostname, sizeof(hostname));

    string ip = obtenerIPLocal();

    cout
        << "Hola Mundo! soy el proceso "
        << rank
        << " de "
        << size
        << " corriendo en la maquina "
        << hostname
        << " IP= "
        << ip
        << endl;
    
    if (MPI_Finalize() != MPI_SUCCESS)
    {
        cout << "Error finalizando MPI" << endl;
        return 1;
    }

    return 0;
}