#include "../data.hpp"

void UNKNOWN(Client *client, Command command)
{
    (void)client;
    (void)command;
    Data &data = Data::getInstance();

    client->appendToSendBuffer("ERROR : Unknown command.\r\n");
    std::cout << "[SERVER] ERROR : Unknown command." <<std::endl;

    for (size_t i = 0; i < data.getPollFds().size(); ++i)
    {
        if (data.getPollFds()[i].fd == client->getFd())
        {
            data.getPollFds()[i].events |= POLLOUT;
            break;
        }
    }
}
