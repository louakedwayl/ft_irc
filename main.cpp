#include "data.hpp"
#include <sstream>

  // le port de notre serveur
void accept_new_connection(Data &data);
void read_data_from_socket(int i, Data &data);

int main(int argc, char **argv)
{
    Data& data = Data::getInstance();
    parse(argc, argv);
    std::cout << "[Server] 🚀 Lancement du serveur IRC..." << std::endl;
    
    // Création du serveur
    if (make_server())
        return (EXIT_FAILURE);

    data.addPollFd(data.getServerSocket());
    std::cout << "[Server] Set up poll fd array\n" << std::endl;

    while (1) 
    {
        int status = poll(data.getPollFds().data(), data.getPollFds().size(), 2000);
        if (status == -1) 
        {
            std::cerr << "[Server] Poll error: " << strerror(errno) << std::endl;
            data.shutdown();
            exit(1);
        }
        else if (status == 0) 
        {
            std::cout << "[Server] Poll timeout reached: no sockets ready. Looping again..." << std::endl;
            continue;
        }

        for (size_t i = 0; i < data.getPollFds().size(); i++) 
        {
            short revents = data.getPollFds()[i].revents;
            int fd = data.getPollFds()[i].fd;

            if (revents == 0)
                continue; // rien à faire

            std::cout << "[Server] socket FD " << fd << " is ready for ";

/*
POLLERR : il y a une erreur de socket détectée (exemple : problème réseau, 
socket cassée, erreur inattendue). Le socket n’est pas forcément fermé mais quelque chose ne va pas.
*/

/*
POLLHUP : le peer a fermé la connexion (client ou serveur a fermé proprement le socket).
 C’est une "fin de communication" détectée.
*/

            if (revents & POLLIN) std::cout << "reading ";
            if (revents & POLLOUT) std::cout << "writing ";
            if (revents & POLLERR) std::cout << "error ";
            if (revents & POLLHUP) std::cout << "hangup ";
            std::cout << std::endl;

            // Gestion des erreurs & hangup avant toute lecture ou écriture
            if (revents & (POLLERR | POLLHUP))
            {
                std::cerr << "[Server] Closing connection on fd " << fd << " due to error/hangup." << std::endl;
                // Tu dois fermer la socket, retirer du poll et supprimer le client
                close(fd);
                data.removePollFdAtIndex(i);
                // Suppression du client associé (à implémenter)
                data.removeClientByFd(fd);
                // Ajuste i car vecteur modifié
                --i;
                continue;
            }

            if (revents & POLLIN)
            {
                if (fd == data.getServerSocket())
                {
                    // Nouvelle connexion entrante
                    accept_new_connection(data);
                }
                else
                {
                    // Données reçues d’un client
                    read_data_from_socket(i, data);
                }
            }

            if (revents & POLLOUT)
            {
                // Socket prête à écrire, on tente d’envoyer les données du buffer
                Client* client = data.getClientByFd(fd);
                if (client && !client->getSendBuffer().empty())
                {
                    int sent = send(fd, client->getSendBuffer().c_str(), client->getSendBuffer().size(), 0);
                    if (sent == -1)
                    {
                        std::cerr << "[Server] Send error on fd " << fd << ": " << strerror(errno) << std::endl;
                        close(fd);
                        data.removePollFdAtIndex(i);
                        data.removeClientByFd(fd);
                        --i;
                        continue;
                    }
                    else
                    {
                        client->eraseFromSendBuffer(sent);
                        if (client->getSendBuffer().empty())
                        {
                            // Plus rien à envoyer, on désactive POLLOUT pour ce fd
                            data.getPollFds()[i].events &= ~POLLOUT;
                        }
                    }
                }
                if (client->getState() == TO_DISCONNECT)
                {
                    std::cout << "[Server] Disconnecting client on fd " << fd << " after flush.\n";
                    close(fd);
                    data.removePollFdAtIndex(i);
                    data.removeClientByFd(fd);
                    --i;
                    continue;
                }
            }
        }
    }
}



void accept_new_connection(Data &data)
{
    int client_fd = accept(data.getServerSocket(), NULL, NULL);
    if (client_fd == -1)
     {
        std::cerr <<"[Server] Accept error: " << strerror(errno) << std::endl;
        return;
    }

    Client* new_client = new Client(client_fd);
    data.addClient(new_client);
    data.addPollFd(client_fd);

    std::cout << "[Server] Accepted new connection on client socket " << client_fd << std::endl;
}

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
        // Envoie le message à tous les membres sauf l'émetteur
        channel->broadcastMessage(client, ":" + client->getPrefix() + " PRIVMSG " + recipient + " :" + message + "\r\n");
        std::cout << "test" <<std::endl;
        
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

            std::cout << "[DEBUG] Envoi message à " << target->getNickName() << " : " << message << std::endl;
        std::string fmessage = ":" + client->getPrefix() + " PRIVMSG " + recipient + " :" + message + "\r\n" ;
        target->appendToSendBuffer(fmessage);
        data.enablePollOutIfNeeded(target);
    }
}

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
        return;
    }

    std::string channelName = command.args[0];
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

    // 🧠 Juste lecture du topic
    if (command.args.size() == 1)
    {
        if (channel->getTopic().empty())
            client->appendToSendBuffer("331 " + client->getNickName() + " " + channelName + " :No topic is set\r\n");
        else
            client->appendToSendBuffer("332 " + client->getNickName() + " " + channelName + " :" + channel->getTopic() + "\r\n");
        return;
    }

    // 🛡️ Restriction seulement pour la modification du topic
    if (channel->getIsTopicRestricted() && !channel->isOperator(client))
    {
        client->appendToSendBuffer("482 " + client->getNickName() + " " + channelName + " :You're not channel operator\r\n");
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
    channel->broadcastMessage(client, ":" + client->getPrefix() + " TOPIC " + channelName + " :" + newTopic + "\r\n");
}


void MODE(Client* client, Command command)
{
    Data& data = Data::getInstance();

    if (command.args.empty()) {
        client->appendToSendBuffer("461 MODE :Not enough parameters\r\n");
        data.enablePollOutIfNeeded(client);
        return;
    }

    std::string target = command.args[0];

    // ---------- MODE <#channel> ----------
    if (!target.empty() && target[0] == '#') 
    {
        Channel* channel = data.getThisChannel(target);
        if (!channel) 
        {
            client->appendToSendBuffer("403 " + target + " :No such channel\r\n");
            data.enablePollOutIfNeeded(client);
            return;
        }

        // Si aucun mode donné → afficher les modes actuels
        if (command.args.size() == 1) 
        {
            std::string modeString = "+";
            if (channel->getIsInviteOnly())       modeString += "i";
            if (channel->getIsTopicRestricted())  modeString += "t";
            if (channel->getUsersLimit() != -1)       modeString += "l";
            if (!channel->getChannelKey().empty())             modeString += "k";

            client->appendToSendBuffer("324 " + client->getNickName() + " " + channel->getName() + " " + modeString + "\r\n");
            data.enablePollOutIfNeeded(client);
            return;
        }

        // Vérifie si le client est opérateur
        if (!channel->isOperator(client)) 
        {
            client->appendToSendBuffer("482 " + target + " :You're not channel operator\r\n");
            data.enablePollOutIfNeeded(client);
            return;
        }

        std::string modeFlags = command.args[1];
        bool addMode = true;
        unsigned int argIndex = 2;

        for (unsigned int i = 0; i < modeFlags.size(); ++i) 
        {
            char mode = modeFlags[i];
            if (mode == '+') 
            {
                addMode = true;
            }
            else if (mode == '-') 
            {
                addMode = false;
            }
            else {
                if (mode == 'i') 
                {
                    std::cout << "test" <<std::endl;
                    channel->setIsInviteOnly(addMode);
                }
                else if (mode == 't') {
                    channel->setIsTopicRestricted(addMode);
                }
                else if (mode == 'l') 
                {
                    if (addMode) {
                        if (argIndex >= command.args.size())
                            break;
                        int limit = std::atoi(command.args[argIndex].c_str());
                        ++argIndex;
                        channel->setUsersLimit(limit);
                    }
                        else {
                                // Il faut gérer la désactivation de la limite d’utilisateurs ici
                                channel->setUsersLimit(-1); // ou ta valeur spéciale pour "pas de limite"
                            }
                }
                else if (mode == 'k') 
                {
                    if (addMode) {
                        if (argIndex >= command.args.size())
                            break;
                        std::string key = command.args[argIndex];
                        ++argIndex;
                        channel->setChannelKey(key);
                    }
                    else {
                            channel->setChannelKey(""); // supprimer la clé quand on désactive
                        }
                }
                else if (mode == 'o') 
                {
                    if (argIndex >= command.args.size())
                        break;
                    std::string targetNick = command.args[argIndex];
                    ++argIndex;
                    Client* targetClient = data.getClientByNickname(targetNick);
                    if (targetClient != NULL) 
                    {
                        if (addMode)
                            channel->addOperator(targetClient);
                        else
                            channel->removeOperator(targetClient);
                    }
                }
                else 
                {
                    client->appendToSendBuffer("472 " + std::string(1, mode) + " :is unknown mode char\r\n");
                }
            }
        }

        // Construction du message MODE à envoyer à tous les clients du channel
        std::string modeLine = ":" + client->getPrefix() + " MODE " + channel->getName() + " " + modeFlags;
        for (std::vector<std::string>::size_type i = 2; i < command.args.size(); ++i) 
        {
            modeLine += " " + command.args[i];
        }
        modeLine += "\r\n";

        channel->broadcastMessage(NULL ,modeLine);
        return;
    }
}



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


void handleCommand(Client* client, Command command)
{
    if (command.name == "CAP")
    {
        CAP (client );
    }
    else if (command.name == "PASS")
    {
        PASS (client, command);
    }
    else if (command.name == "NICK")
    {
        NICK (client, command);
    }
    else if (command.name == "LIST")
    {
        LIST (client, command);
    }
    else if (command.name == "USER")
    {
        USER (client, command);
    }
    else if (command.name == "PING")
    {
        PING (client, command);
    }
    else if (command.name == "JOIN")
    {
        JOIN  (client, command);
    }
    else if (command.name == "INVITE")
    {
        INVITE (client, command);
    }
    else if (command.name == "PRIVMSG")
    {
        PRIVMSG(client, command);
    }
    else if (command.name == "KICK")
    {
        KICK(client, command);
    }
    else if (command.name == "KICK")
    {
        INVITE (client, command);
    }
    else if (command.name == "TOPIC")
    {
        TOPIC (client, command);
    }
    else if (command.name == "MODE")
    {
        MODE (client, command);
    }
    else if (command.name == "WHOIS")
    {
        WHOIS (client, command);
    }
    else if (command.name == "/QUIT_SERV")
    {
        QUIT_SERV(client, command);
    }
    else
    {
        UNKNOWN(client, command);
    }
}

void parseCommands(Client* client, const char* buffer)
{
    std::istringstream stream(buffer);
    std::string line;

    while (std::getline(stream, line))
    {
        // retirer \r en fin si présent
        if (!line.empty() && line[line.size() - 1] == '\r')
            line.erase(line.size() - 1, 1);

        Command command(line);

std::cout << "line: " << line << std::endl;


// Accès à la commande
std::cout << "Commande: " << command.name << std::endl;

// Affiche tous les arguments
for (size_t i = 0; i < command.args.size(); ++i) {
    std::cout << "Arg[" << i << "] = " << command.args[i] << std::endl;
}

        // Ici, appelle ta fonction qui traite chaque commande
        handleCommand(client, line);
    }
}

void read_data_from_socket(int i, Data& data)
{
    char buffer[BUFSIZ];
    int bytes_read;
    int sender_fd = data.getPollFds()[i].fd;

    std::memset(buffer, 0, sizeof(buffer));
    bytes_read = recv(sender_fd, buffer, sizeof(buffer) - 1, 0);

    if (bytes_read <= 0)
    {
        if (bytes_read == 0)
        {
            // client qui se deco tt seul

            close (sender_fd);
            std::cout << "[" << sender_fd << "] Client disconnected." << std::endl;
        }
        else
        {
            std::cerr << "[Server] recv() error on fd " << sender_fd << ": " << std::strerror(errno) << std::endl;
        }
        close(sender_fd);
        data.removePollFdAtIndex(i);
        return;
    }
    parseCommands(data.getClientByFd(sender_fd), buffer);
}
