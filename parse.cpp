#include "data.hpp"

int parse(int argc, char **argv, Data& data)
{
    Data::getInstance();
    if (argc != 3)
    {
        std::cerr << "usage ./ircserv <port> <password>" << std::endl ;
        exit (1);
    }
    std::istringstream stream(argv[1]); // conversion chaîne -> int (facultatif selon ton type)
    int port;
    if (!(stream >> port)) // vérifie si argv[1] est bien un entier
    {
        std::cerr << "Invalid port: must be a number." << std::endl;
        exit(1);
    }
    data._port = port;
    data._password = argv[2];
    return (0);
}