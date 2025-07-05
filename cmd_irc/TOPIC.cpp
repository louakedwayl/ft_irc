#include "../data.hpp"

void TOPIC(Client* client, Command command)
{
    Data& data = data.getInstance();

    if (client->getState() != REGISTERED)
    {
        client->appendToSendBuffer("ERROR :<TOPIC> client not registered\r\n");
        data.enablePollOutIfNeeded(client);
        return;
    }

    if (command.args.empty())
    {
        client->appendToSendBuffer("461 " + client->getNickName() + " TOPIC :Not enough parameters\r\n");
        data.enablePollOutIfNeeded(client);
        return;
    }

    std::string channelName = command.args[0];
    Channel* channel = data.getThisChannel(channelName);


    if (!channel)
    {
        client->appendToSendBuffer("403 " + channelName + " :No such channel\r\n");
        data.enablePollOutIfNeeded(client);
        return;
    }

    if (!channel->hasClient(client))
    {
        client->appendToSendBuffer("442 " + channelName + " :You're not on that channel\r\n");
        data.enablePollOutIfNeeded(client);
        return;
    }

    // 🧠 Juste lecture du topic
    if (command.args.size() == 1)
    {
        if (channel->getTopic().empty())
            client->appendToSendBuffer("331 " + client->getNickName() + " " + channelName + " :No topic is set\r\n");
        else
            client->appendToSendBuffer("332 " + client->getNickName() + " " + channelName + " :" + channel->getTopic() + "\r\n");
        data.enablePollOutIfNeeded(client);
        return;
    }

    // 🛡️ Restriction seulement pour la modification du topic
    if (channel->getIsTopicRestricted() && !channel->isOperator(client))
    {
        client->appendToSendBuffer("482 " + client->getNickName() + " " + channelName + " :You're not channel operator\r\n");
        data.enablePollOutIfNeeded(client);
        return;
    }

    // 📝 Reconstruction du nouveau topic
    std::string newTopic = command.args[1];
    for (size_t i = 2; i < command.args.size(); ++i)
        newTopic += " " + command.args[i];

    if (!newTopic.empty() && newTopic[0] == ':')
        newTopic = newTopic.substr(1);

    channel->setTopic(newTopic);

    // 📣 Broadcast à tous les membres du canal
    channel->broadcastMessage(NULL, ":" + client->getPrefix() + " TOPIC " + channelName + " :" + newTopic + "\r\n");
}