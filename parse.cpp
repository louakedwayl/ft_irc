#include "data.hpp"

int parse(int argc, char **argv)
{
    int port;
    
    if (argc != 3)
    {
        std::cerr << "usage ./ircserv <port> <password>" << std::endl ;
        exit (1);
    }
    Data& data = Data::getInstance();
    std::istringstream stream(argv[1]);
    if (!(stream >> port))
    {
        std::cerr << "Invalid port: must be a number." << std::endl;
        exit(1);
    }
    if (port <= 0 || port > 65535)
    {
        std::cerr << "[Server] Port invalide : " << port << std::endl;
        exit (1);
    }
    data.setPort(port);
    data.setPassword(argv[2]);
    return (0);
}
