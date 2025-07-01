#include "../data.hpp"

void WHOIS(Client* client, Command command)
{
    Data& data = Data::getInstance();

    if (command.args.empty()) {
        client->appendToSendBuffer("461 WHOIS :Not enough parameters\r\n");
        data.enablePollOutIfNeeded(client);
        return;
    }

    std::string targetNick = command.args[0];
    Client* targetClient = data.getClientByNickname(targetNick);

    if (!targetClient) 
    {
        client->appendToSendBuffer("401 " + targetNick + " :No such nick\r\n");
        data.enablePollOutIfNeeded(client);
        return;
    }

    // 311 RPL_WHOISUSER
    std::string reply311 = "311 " + client->getNickName() + " " + targetNick + " " +
                          targetClient->getUserName() + " " +
                          targetClient->getHostName() + " * :" +
                          targetClient->getRealName() + "\r\n";
    client->appendToSendBuffer(reply311);

    // 312 RPL_WHOISSERVER (serveur sur lequel est connecté l’utilisateur)
    std::string reply312 = "312 " + client->getNickName() + " " + targetNick + " " +
                          targetClient->getServerName() + " :IRC Server\r\n";
    client->appendToSendBuffer(reply312);

    // 319 RPL_WHOISCHANNELS (channels où est présent l’utilisateur)
    std::string channelsList = targetClient->getChannelsList(); // format "#chan1 #chan2 ..."
    if (!channelsList.empty()) 
    {
        std::string reply319 = "319 " + client->getNickName() + " " + targetNick + " :" + channelsList + "\r\n";
        client->appendToSendBuffer(reply319);
    }

    // 318 RPL_ENDOFWHOIS (fin du WHOIS)
    std::string reply318 = "318 " + client->getNickName() + " " + targetNick + " :End of WHOIS list\r\n";
    client->appendToSendBuffer(reply318);

    data.enablePollOutIfNeeded(client);
}