#include "../data.hpp"

void INVITE(Client* client, Command command)
{
  Data& data = Data::getInstance();

    if (client->getState() != REGISTERED)
    {
        client->appendToSendBuffer("ERROR :<INVITE> client not registered\r\n");
        data.enablePollOutIfNeeded(client);
        return;
    }

    if (command.args.size() < 2)
    {
        client->appendToSendBuffer("461 " + client->getNickName() + " INVITE :Not enough parameters\r\n");
        return;
    }

    std::string targetNick = command.args[0];
    std::string channelName = command.args[1];

    Channel* channel = data.getThisChannel(channelName);
    if (!channel)
    {
        client->appendToSendBuffer("403 " + channelName + " :No such channel\r\n");
        return;
    }

    if (!channel->hasClient(client))
    {
        client->appendToSendBuffer("442 " + channelName + " :You're not on that channel\r\n");
        return;
    }

    if (channel->getIsInviteOnly() && !channel->isOperator(client))
    {
        client->appendToSendBuffer("482 " + channelName + " :You're not channel operator\r\n");
        return;
    }

    Client* target = data.getClientByNickname(targetNick);
    if (!target)
    {
        client->appendToSendBuffer("401 " + targetNick + " :No such nick\r\n");
        return;
    }

    if (channel->hasClient(target))
    {
        client->appendToSendBuffer("443 " + targetNick + " " + channelName + " :is already on channel\r\n");
        return;
    }

    // Ajouter à la liste des invités du canal
    channel->addClient(target);

    // Message à l’utilisateur invité
    target->appendToSendBuffer(":" + client->getPrefix() + " INVITE " + targetNick + " :" + channelName + "\r\n");

    // Message de confirmation à l’émetteur
    client->appendToSendBuffer("341 " + targetNick + " " + channelName + "\r\n");
}