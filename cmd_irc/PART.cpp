#include "../data.hpp"

std::vector<std::string> split(const std::string& s, char delimiter)
{
    std::vector<std::string> tokens;
    std::stringstream ss(s);
    std::string item;

    while (std::getline(ss, item, delimiter)) {
        if (!item.empty())
            tokens.push_back(item);
    }
    return tokens;
}

void PART(Client* client, Command command)
{
    Data& data = Data::getInstance();

    if (command.args.size() < 1 || command.args.size() > 2 )
    {
        client->appendToSendBuffer("Usage : PART <channel> [message]");
        data.enablePollOutIfNeeded(client);
        return ;
    }

    // Récupération des noms de channels (séparés par des virgules)
    std::vector<std::string> channels = split(command.args[0], ',');

    std::string partMessage;
    if (command.args.size() == 2)
        partMessage = command.args[1];
    else
        partMessage = "";

    
    for (size_t i = 0; i < channels.size(); ++i)
    {
        std::string channelName = channels[i];
        Channel* channel = data.getThisChannel(channelName);

        // Si le channel n'existe pas
        if (!channel)
        {
            client->appendToSendBuffer("403 " + channelName + " :No such channel\r\n");
            continue;
        }

        // Si le client n'est pas dans ce channel
        if (!channel->hasClient(client))
        {
            client->appendToSendBuffer("442 " + channelName + " :You're not on that channel\r\n");
            continue;
        }

        // Construire le message PART
        std::string msg = ":" + client->getPrefix() + " PART " + channelName;
        if (!partMessage.empty())
            msg += " :" + partMessage;
        msg += "\r\n";

        // Envoyer à tous les membres
        channel->broadcastMessage(client, msg);         // aux autres
        client->appendToSendBuffer(msg);         // à soi-même

        // Retirer le client
        channel->removeClient(client);

        // Supprimer le channel s'il est vide
        if (channel->getSize() == 0)
            data.deleteChannel(channelName);
    }

    data.enablePollOutIfNeeded(client);
}
