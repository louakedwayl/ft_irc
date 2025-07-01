#include "../data.hpp"

void KICK(Client* client, Command command)
{
    Data &data = Data::getInstance();

    if (command.args.size() < 2)
    {
        client->appendToSendBuffer("ERROR :No channel or user given\r\n");
        data.enablePollOutIfNeeded(client);
        return ;
    }

    std::string channelName = command.args[0];
    std::string targetNick = command.args[1];
    std::string reason = (command.args.size() >= 3) ? command.args[2] : "Kicked";

    Channel* channel = data.getThisChannel(channelName);
    if (!channel)
    {
        client->appendToSendBuffer("ERROR :<KICK> No such channel\r\n");
        data.enablePollOutIfNeeded(client);
        return;
    }

    // 1. Vérifie que le client est dans le channel
    if (!channel->hasClient(client))
    {
        client->appendToSendBuffer("ERROR :<KICK> You're not on that channel\r\n");
        data.enablePollOutIfNeeded(client);
        return;
    }

    // 2. Vérifie que le client est opérateur
    if (!channel->isOperator(client))
    {
        client->appendToSendBuffer("ERROR :<KICK> You must be channel operator\r\n");
        data.enablePollOutIfNeeded(client);
        return;
    }

    // 3. Trouver le client à expulser
    Client* target = data.getClientByNickname(targetNick);
    if (!target || !channel->hasClient(target))
    {
        client->appendToSendBuffer("ERROR :<KICK> User not in channel\r\n");
        data.enablePollOutIfNeeded(client);
        return;
    }

    // 4. Envoi du message de KICK à tous les membres du channel
    std::string kickMsg = ":" + client->getPrefix() + " KICK " + channelName + " " + targetNick + " :" + reason + "\r\n";
    channel->broadcastMessage(NULL, kickMsg); // NULL = tous les membres

    // 5. Retirer le target du channel
    channel->removeClient(target);
    target->removeChannel(channel); // si tu as cette méthode
}
