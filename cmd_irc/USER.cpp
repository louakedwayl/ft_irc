#include "../data.hpp"

void USER(Client* client, Command command)
{
    Data &data = Data::getInstance();

    // Vérifie si le client a passé le mot de passe si nécessaire
    if (client->getState() == CONNECTING || client->getState() == TO_DISCONNECT)
    {
        std::cerr << "[Server] USER: client " << client->getFd() << ": Password not sent. Can't register nickname." << std::endl;
        client->appendToSendBuffer(":irc.slytherin.com 464 * :Password required\r\n");
        client->setState(TO_DISCONNECT);

        for (size_t i = 0; i < data.getPollFds().size(); ++i)
        {
            if (data.getPollFds()[i].fd == client->getFd())
            {
                data.getPollFds()[i].events |= POLLOUT;
                break;
            }
        }
        return;
    }

    // USER attend au moins 4 arguments (username, hostname, servername, realname)
    if (command.args.size() < 4)
    {
        client->appendToSendBuffer(":irc.slytherin.com 461 * USER :Not enough parameters\r\n");
        data.enablePollOutIfNeeded(client);
        return;
    }

    // Assignation des arguments
    client->setUserName(command.args[0]);    // username
    client->setHostName(command.args[1]);    // hostname
    client->setServerName(command.args[2]);  // servername
    client->setRealName(command.args[3]);    // trailing = realname

    std::cout << "[Server] USER assigned: username=" << client->getUserName()
              << ", hostname=" << client->getHostName()
              << ", servername=" << client->getServerName()
              << ", realname=" << client->getRealName()
              << " (client fd: " << client->getFd() << ")" << std::endl;

    client->setState(SENT_USER);

if (client->isFullyRegistered())
{
    client->set_registered();
    client->setState(REGISTERED);
    send001(client);
    send002(client);
    send003(client);
    send004(client);
}
    // Puis activer POLLOUT pour dire à poll qu'on a des données à envoyer
    for (size_t i = 0; i < data.getPollFds().size(); ++i)
    {
        if (data.getPollFds()[i].fd == client->getFd())
        {
            data.getPollFds()[i].events |= POLLOUT;
            break;
        }
    }

}