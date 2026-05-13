#include <iostream>
#include <unistd.h>

#include <arpa/inet.h>
#include <sys/socket.h>

#include <cstring>
#include <string>

using namespace std;

string obtenerIPLocal()
{
    int sock = socket(AF_INET, SOCK_DGRAM, 0);
    
    if (sock < 0)
    {
        return "ERROR_IP";
    }
    
    sockaddr_in servidor;

    memset(&servidor, 0, sizeof(servidor));

    servidor.sin_family = AF_INET;

    servidor.sin_port = htons(80);

    // Servidor externo solo para detectar interfaz
    inet_pton(AF_INET, "8.8.8.8", &servidor.sin_addr);

    connect( sock, (sockaddr *)&servidor , sizeof(servidor) );

    sockaddr_in local;

    socklen_t len = sizeof(local);

    getsockname( sock , (sockaddr *)&local , &len );

    char ip[INET_ADDRSTRLEN];

    inet_ntop( AF_INET , &local.sin_addr , ip , sizeof(ip));

    close(sock);
    
    return string(ip);
}