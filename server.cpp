#include "data.hpp"

int make_server()
{
    Data& data = Data::getInstance();
    create_server_socket();
    if (data.getServerSocket() == -1)
    {
        return (EXIT_FAILURE);
    }
    if (server_listen() == -1)
    {
        return (EXIT_FAILURE);
    }
    return (EXIT_SUCCESS);
}

int create_server_socket(void) 
{
    Data& data = Data::getInstance();

    struct sockaddr_in sa;
    int status;

    // Préparaton de l'adresse et du port pour la socket de notre serveur
    memset(&sa, 0, sizeof sa);
    sa.sin_family = AF_INET; // IPv4
    sa.sin_addr.s_addr = htonl(INADDR_LOOPBACK); // 127.0.0.1, localhost
    sa.sin_port = htons(data.getPort());

    // Création de la socket
    data.setServerSocket( socket(sa.sin_family, SOCK_STREAM, 0));
    if (data.getServerSocket() == -1) 
    {
        std::cerr << "[Server] Erreur socket : " << strerror(errno) <<std::endl ;
        return (-1);
    }
    std::cout << "[Server] Created server socket fd: " << data.getServerSocket() << std::endl ;

    // Liaison de la socket à l'adresse et au port
    status = bind(data.getServerSocket(), (struct sockaddr *)&sa, sizeof sa);
    if (status != 0) 
    {
        std::cerr << "[Server] Bind error: " << strerror(errno) << std::endl;
        close (data.getServerSocket());
        return (-1);
    }
    std::cout << "[Server] Bound socket to localhost port " << data.getPort() << std::endl;
    return (EXIT_SUCCESS);
}

int server_listen()
{
    Data& data = Data::getInstance();

    int status = listen(data.getServerSocket(), 10);
    if (status != 0)
    {
        std::cerr << "[Server] Listen error: " << strerror(errno) << std::endl;
        return (-1);
    }
    std::cout << "[Server] Listening on port " << data.getPort() << std::endl;
    return (EXIT_SUCCESS);
}