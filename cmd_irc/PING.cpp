#include "../data.hpp"

void PING(Client* client, Command command) 
{
    Data &data = Data::getInstance();

    if (command.args.empty()) 
    {
        // Pas d’argument, ignore ou envoie une erreur.
        return;
    }
    std::string response = "PONG :" + command.args[0] + "\r\n";
    client->appendToSendBuffer(response);

    for (size_t i = 0; i < data.getPollFds().size(); ++i)
    {
        if (data.getPollFds()[i].fd == client->getFd())
        {
            data.getPollFds()[i].events |= POLLOUT;
            break;
        }
    }

}