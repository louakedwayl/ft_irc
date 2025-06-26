#include "../data.hpp"

void LIST(Client* client, Command command)
{
    std::cout << "[DEBUG] LIST command called by client " << client->getFd() << std::endl;

    (void)command;
    Data& data = Data::getInstance();

    std::vector<Channel*> ChannelHub = data.getChannel();

    // En-tête obligatoire pour /LIST
    client->appendToSendBuffer(":irc.slytherin.com 321 * Channel :Users  Name\r\n");

    if (ChannelHub.empty())
    {
        client->appendToSendBuffer(":irc.slytherin.com 461 * None channel\r\n");
    }
    else
    {
        for (size_t i = 0; i < ChannelHub.size(); ++i)
        {
            Channel* chan = ChannelHub[i];
            std::string name = chan->getName();
            size_t userCount = chan->getClients().size();
            std::string topic = chan->getTopic();

            std::stringstream ss;
            ss << ":irc.slytherin.com 322 * " << name << " " << userCount << " :" << topic << "\r\n";
            client->appendToSendBuffer(ss.str());
        }
    }

    // Fin de la liste (obligatoire pour que Irssi affiche)
    client->appendToSendBuffer(":irc.slytherin.com 323 * :End of /LIST\r\n");

    // Active POLLOUT si besoin
    data.enablePollOutIfNeeded(client);
}
