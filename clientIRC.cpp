#include <iostream>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>    // Pour INADDR_ANY
#include <string.h>


#define LISTENING_PORT 4243
#define BUFFER_SIZE 1024
#define CONNECTION_HOST "127.0.0.1"

int main()
{
    int socketFD;
    int bindReturn;

    struct sockaddr_in socketAddress;
    socketAddress.sin_family = AF_INET;
    socketAddress.sin_port = htons(LISTENING_PORT);
    socketAddress.sin_addr.s_addr = INADDR_ANY;

    int inetReturnCode = inet_pton (AF_INET, CONNECTION_HOST, &socketAddress.sin_addr);
    if ( inetReturnCode == -1)
    {
        std::cerr << "(CLIENT)Erreur de conversion de l'ip en binaire." << std::endl;
        return 1;
    }

    socketFD = socket(AF_INET, SOCK_STREAM, 0);
    if (socketFD == -1)
    {
        std::cerr << "(CLIENT) Echec d'initialisation du socket." << std::endl;
        return 1;
    }
    int connectionStatus = connect (socketFD, (struct sockaddr*) &socketAddress, sizeof(socketAddress));
    
    std::string message = "Message";
    int sentBytes = send (socketFD, message.c_str(), message.length(), 0);

    char buffer [BUFFER_SIZE] = {0};
    int receivedBytes = recv(socketFD, buffer, BUFFER_SIZE, 0);
    if (receivedBytes == -1)
    {
        std::cerr << "(CLIENT) Echec d'initialisation du socket." << std::endl;
        return 1;
    }

    std::cout << buffer << std::endl ;
}
