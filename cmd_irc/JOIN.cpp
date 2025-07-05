#include "../data.hpp"

void JOIN(Client* client, Command command)
{
    Data& data = Data::getInstance();

    // si le client n est pas pleinement enregistrer
    if (client->getState() != REGISTERED)
    {
        client->appendToSendBuffer("ERROR :<JOIN> client not registered\r\n");
        data.enablePollOutIfNeeded(client);
        return;
    }

    //si join sans arg
    if (command.args.empty())
    {
        client->appendToSendBuffer("ERROR :<JOIN> no argument\r\n");
        data.enablePollOutIfNeeded(client);
        return;
    }

    // si plus de 1 cmd et 2 arg
    if (command.args.size() > 2)
    {
        client->appendToSendBuffer("ERROR :<JOIN> usage: <channel> (<key>)\r\n");
        data.enablePollOutIfNeeded(client);
        return;
    }

    //si le premier arg ne commence pas par #
    std::string channelName = command.args[0];
    if (channelName.empty() || channelName[0] != '#')
    {
        client->appendToSendBuffer("ERROR :<JOIN> channel name must start with '#'\r\n");
        data.enablePollOutIfNeeded(client);
        return;
    }

    Channel* channel = data.getThisChannel(channelName);

    if (channel)
    {
        // Channel invite-only : vérifier si client est invité
        if (channel->getIsInviteOnly())
        {
            const std::vector<Client*>& invited = channel->getInvited();
            bool isInvited = std::find(invited.begin(), invited.end(), client) != invited.end();
            if (!isInvited)
            {
                client->appendToSendBuffer("ERROR :<JOIN> channel is invite-only\r\n");
                data.enablePollOutIfNeeded(client);
                return;
            }
        }

        // Channel avec clé : vérifier la clé
        std::string key = channel->getChannelKey();
        if (!key.empty())
        {
            if (command.args.size() < 2 || command.args[1] != key)
            {
                client->appendToSendBuffer("ERROR :<JOIN> invalid channel key\r\n");
                data.enablePollOutIfNeeded(client);
                return;
            }
        }

        // Limite d’utilisateurs
        if (channel->getUsersLimit() > 0 && channel->getClients().size() >= static_cast<size_t>(channel->getUsersLimit()))
        {
            client->appendToSendBuffer("ERROR :<JOIN> channel user limit reached\r\n");
            data.enablePollOutIfNeeded(client);
            return;
        }

        // Ajout client
        channel->addClient(client);
        client->addChannel(channel);
        std::cout << "[server] : client " << client->getFd() << " joined channel " << channelName << std::endl;
    }
    else
    {
        // Création d’un nouveau channel
        channel = new Channel(channelName);
        data.getChannel().push_back(channel);
        channel->addClient(client);
        client->addChannel(channel);
        channel->addOperator(client); // Créateur = opérateur
        std::cout << "[server] : client " << client->getFd() << " created new channel " << channelName << std::endl;
    }

    // Construction du message JOIN
    std::string joinMsg = ":" + client->getPrefix() + " JOIN " + channelName + "\r\n";
    channel->broadcastMessage(NULL, joinMsg);           // Envoyer à tous les membres

    // Envoie du topic au client
    if (channel->getTopic().empty())
    {
        client->appendToSendBuffer("331 " + client->getNickName() + " " + channelName + " :No topic is set\r\n");
    }
    else
    {
        client->appendToSendBuffer("332 " + client->getNickName() + " " + channelName + " :" + channel->getTopic() + "\r\n");
    }
    data.enablePollOutIfNeeded(client);
}
