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

    if (command.args.size() < 2) {
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

    // Ouvre le fichier
    int file_fd = open(filePath.c_str(), O_RDONLY);
    if (file_fd < 0) 
    {
        client->appendToSendBuffer("550 :Failed to open file\r\n");
        data.enablePollOutIfNeeded(client);
        return;
    }

    // deplace le curseur a la fin du fichier pour savoir sa taille
    off_t filesize = lseek(file_fd, 0, SEEK_END);
    lseek(file_fd, 0, SEEK_SET);

    // Socket d'écoute
    int port = 5000 + (rand() % 1000); // simple port dynamique
    int server_fd = socket(AF_INET, SOCK_STREAM, 0);
    sockaddr_in addr;
    std::memset(&addr, 0, sizeof(addr));    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = INADDR_ANY;
    addr.sin_port = htons(port);

    if (bind(server_fd, (sockaddr*)&addr, sizeof(addr)) < 0 || listen(server_fd, 1) < 0)
     {
        client->appendToSendBuffer("551 :Failed to bind or listen\r\n");
        close(file_fd);
        data.enablePollOutIfNeeded(client);
        return;
    }

    // Convertir IP en entier
    std::string ip = "127.0.0.1"; 
    in_addr inaddr;
    inet_aton(ip.c_str(), &inaddr);
    unsigned int ipInt = ntohl(inaddr.s_addr);

    // Construire le PRIVMSG avec CTCP DCC SEND
    std::stringstream ss;
    ss << "\x01" << "DCC SEND \"" << filePath << "\" " << ipInt << " " << port << " " << filesize << "\x01\r\n";
    std::string dccMessage = ":" + client->getNickName() + " PRIVMSG " + targetNick + " :" + ss.str();

    // L’envoie à target
    target->appendToSendBuffer(dccMessage);
    data.enablePollOutIfNeeded(target);

    std::cout << "[DCC] Attente connexion de " << targetNick << " pour envoyer " << filePath << "...\n";

    // ⚠️ ici tu devrais idéalement créer un thread ou déléguer l’envoi à un gestionnaire de transfert
    int receiver_fd = accept(server_fd, NULL, NULL);
    if (receiver_fd < 0) 
    {
        std::cerr << "Erreur : échec accept()\n";
        close(file_fd);
        close(server_fd);
        return;
    }

    // Envoi brut du fichier
    char buffer[1024];
    ssize_t n;
    while ((n = read(file_fd, buffer, sizeof(buffer))) > 0) 
    {
        send(receiver_fd, buffer, n, 0);
    }

    close(receiver_fd);
    close(server_fd);
    close(file_fd);
    std::cout << "[DCC] Transfert terminé vers " << targetNick << "\n";
}
