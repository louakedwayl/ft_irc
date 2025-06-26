#include "../data.hpp"

void PASS (Client *client, Command command)
{
    Data &data = Data::getInstance();

    if (!command.args.empty() && data.checkPassword(command.args[0]))
    {
        client->appendToSendBuffer(":irc.slytherin.com NOTICE AUTH :Password accepted\r\n");
        client->setState(SENT_PASS);
    }
    else
    {
        client->appendToSendBuffer("ERROR :Password incorrect\r\n");
        client->markForDisconnect();
    }
    for (size_t i = 0; i < data.getPollFds().size(); ++i)
    {
        if (data.getPollFds()[i].fd == client->getFd())
        {
            data.getPollFds()[i].events |= POLLOUT;
            break;
        }
    }
}
