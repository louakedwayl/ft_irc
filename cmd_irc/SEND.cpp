#include "../data.hpp"
#include <iostream>         // std::cout, std::cerr
#include <sstream>          // std::stringstream
#include <string>           // std::string
#include <fcntl.h>          // open
#include <unistd.h>         // read, write, close
#include <sys/socket.h>     // socket, bind, listen, accept, send
#include <netinet/in.h>     // sockaddr_in, htons
#include <arpa/inet.h>      // inet_aton, ntohl
#include <cstring>          // memset

void SEND(Client* client, Command command)
{
    Data& data = Data::getInstance();

    if (command.args.size() < 2) 
    {
        client->appendToSendBuffer("461 SEND :Not enough parameters\r\n");
        data.enablePollOutIfNeeded(client);
        return;
    }

    std::string targetNick = command.args[0];
    std::string filePath = command.args[1];

    // Vérifie que le destinataire existe
    Client* target = data.getClientByNickname(targetNick);
    if (!target) 
    {
        client->appendToSendBuffer("401 " + targetNick + " :No such nick\r\n");
        data.enablePollOutIfNeeded(client);
        return;
    }

    int file_fd = open(filePath.c_str(), O_RDONLY);
    if (file_fd < 0) 
    {
        client->appendToSendBuffer("550 :Failed to open file\r\n");
        data.enablePollOutIfNeeded(client);
        return;
    }

    // Envoi message de début de transfert
    {
        std::stringstream startMsg;
        startMsg << ":" << client->getNickName() << " PRIVMSG " << targetNick << " :FILESTART " << filePath << "\r\n";
        target->appendToSendBuffer(startMsg.str());
        data.enablePollOutIfNeeded(target);
    }

    const size_t maxIrcMsgLen = 400; // taille max du chunk, sécurité sous 512 total
    char buffer[maxIrcMsgLen];
    ssize_t bytesRead;

    while ((bytesRead = read(file_fd, buffer, maxIrcMsgLen)) > 0) 
    {
        std::string chunk(buffer, bytesRead);

        // Nettoyer le chunk : remplacer \r et \n par espace pour éviter découpage IRC
        for (size_t i = 0; i < chunk.size(); ++i) 
        {
            if (chunk[i] == '\r' || chunk[i] == '\n') 
                chunk[i] = ' ';
        }

        // Construire le message PRIVMSG FILEPART
        std::stringstream msg;
        msg << ":" << client->getNickName() << " PRIVMSG " << targetNick << " :FILEPART " << chunk << "\r\n";

        target->appendToSendBuffer(msg.str());
        data.enablePollOutIfNeeded(target);
    }

    // Message fin de transfert
    {
        std::stringstream endMsg;
        endMsg << ":" << client->getNickName() << " PRIVMSG " << targetNick << " :FILEEND\r\n";
        target->appendToSendBuffer(endMsg.str());
        data.enablePollOutIfNeeded(target);
    }

    close(file_fd);
}