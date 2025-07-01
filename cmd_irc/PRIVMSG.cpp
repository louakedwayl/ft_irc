#include "../data.hpp"
#include <ctime>

void PRIVMSG(Client* client, Command command)
{
    Data& data = Data::getInstance();
    
    
    if (command.args.size() < 2)
    {
        client->appendToSendBuffer("ERROR :No recipient or message given\r\n");
        data.enablePollOutIfNeeded(client);
        return;
    }
    
    std::string  recipient = command.args[0];
    std::string message;
    
    for (size_t i = 1; i < command.args.size(); ++i)
    {
        if (i > 1)
            message+= " ";
        message += command.args[i]; 
    }

    // Supprime le ':' initial s'il est là
    if (!message.empty() && message[0] == ':')
        message = message.substr(1);


    if (recipient[0] == '#')  // C'est un canal
    {
            Channel* channel = data.getThisChannel(recipient);
        if (!channel)
        {
            client->appendToSendBuffer("ERROR :No such channel\r\n");
            data.enablePollOutIfNeeded(client);
            return;
        }

        // Vérifier que le client est dans le channel
        if (!channel->hasClient(client))
        {
            client->appendToSendBuffer("404 " + recipient + " :Cannot send to channel\r\n");
            data.enablePollOutIfNeeded(client);
            return;
        }

         if (recipient == "#bot" && message == "/time")
        {
            // Générer l’heure actuelle
            std::time_t now = std::time(NULL);
            char timebuf[64];
            std::strftime(timebuf, sizeof(timebuf), "%Y-%m-%d %H:%M:%S", std::localtime(&now));

            // Construire la réponse du serveur dans le channel
            std::string response = ":server!server@irc.server PRIVMSG " + recipient + " :Current server time is ";
            response += timebuf;
            response += "\r\n";

            // Envoyer à tous les membres du channel #bot
            channel->broadcastMessage(NULL, response);
            data.enablePollOutIfNeeded(client);
            return;
        }

        // Envoie le message à tous les membres sauf l'émetteur
        channel->broadcastMessage(client, ":" + client->getPrefix() + " PRIVMSG " + recipient + " :" + message + "\r\n"); 
    }
    else  // Message privé à un utilisateur
    {
        Client* target = data.getClientByNickname(recipient);
        if (!target)
        {
            client->appendToSendBuffer("401 " + recipient + " :No such nick\r\n"); // Code IRC correct
            data.enablePollOutIfNeeded(client);
            return;
        }

        std::string fmessage = ":" + client->getPrefix() + " PRIVMSG " + recipient + " :" + message + "\r\n" ;
        target->appendToSendBuffer(fmessage);
        data.enablePollOutIfNeeded(target);
    }
}
