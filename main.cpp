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

void CAP (Client* client)
{
    Data &data = Data::getInstance();
    client->appendToSendBuffer("CAP * LS :\r\n");

    for (size_t i = 0; i < data.getPollFds().size(); ++i)
    {
        if (data.getPollFds()[i].fd == client->getFd())
        {
            data.getPollFds()[i].events |= POLLOUT;
            break;
        }
    }
}

void send001(Client* client)
{
    std::string nick;
    if (client->getNickName().empty())
        nick = "unknown";
    else
        nick = client->getNickName();

    std::string user;
    if (client->getUserName().empty())
        user = "unknown";
    else
        user = client->getUserName();

    std::string host;
    if (client->getHostName().empty())
        host = "localhost";
    else
        host = client->getHostName();

    std::string msg = ":irc.slytherin.com 001 " + nick +
        " :Welcome to the Internet Relay Network " +
        nick + "!" + user + "@" + host + "\r\n";

    client->appendToSendBuffer(msg);
}



void send002(Client* client)
{
    std::string nick;
    if (client->getNickName().empty())
        nick = "unknown";
    else
        nick = client->getNickName();

    std::string msg = ":irc.slytherin.com 002 " + nick +
        " :Your host is irc.slytherin.com, running version 1.0\r\n";

    client->appendToSendBuffer(msg);
}

void send003(Client* client)
{
    std::string nick;
    if (client->getNickName().empty())
        nick = "unknown";
    else
        nick = client->getNickName();

    std::string msg = ":irc.slytherin.com 003 " + nick +
        " :This server was created 25/06/2025\r\n";

    client->appendToSendBuffer(msg);
}


void send004(Client* client)
{
    std::string nick;
    if (client->getNickName().empty())
        nick = "unknown";
    else
        nick = client->getNickName();

    // Format : :server 004 <nick> <servername> <version> <user modes> <channel modes>
    std::string msg = ":irc.slytherin.com 004 " + nick +
        " irc.slytherin.com 1.0 ao mtov\r\n";

    client->appendToSendBuffer(msg);
}

void PASS (Client *client, Command command)
{
    Data &data = Data::getInstance();

    if (!command.args.empty() && data.checkPassword(command.args[0]))
    {
        client->appendToSendBuffer(":irc.slytherin.com NOTICE AUTH :Password accepted\r\n");
        client->setState(SENT_PASS);
    }
    else
    {
        client->appendToSendBuffer("ERROR :Password incorrect\r\n");
        client->markForDisconnect();
    }
    for (size_t i = 0; i < data.getPollFds().size(); ++i)
    {
        if (data.getPollFds()[i].fd == client->getFd())
        {
            data.getPollFds()[i].events |= POLLOUT;
            break;
        }
    }
}

void QUIT_SERV (Client *client, Command command)
{
    Data &data = Data::getInstance();
    (void)client;
    (void)command;
    data.shutdown();
    exit (0);
}

static bool	does_nickname_have_channel_prefix(std::string const & nickname)
{
	if (nickname[0] == '#' || nickname[0] == '&' || nickname[0] == '~' ||
		nickname[0] == '@' || nickname[0] == '%' || nickname[0] == ':')
			return (true);
	if (nickname[0] == '+' && nickname[1])
	{
		if ( // norme IRC
			nickname[1] == 'q' || nickname[1] == 'a' || nickname[1] == 'o' ||
			nickname[1] == 'h' || nickname[1] == 'v')
			return (true);
	}
	return (false);
}

void NICK(Client *client, Command command)
{
    Data &data = Data::getInstance();

    if (client->getState() == CONNECTING || client->getState() == TO_DISCONNECT)
    {
        std::cerr << "[Server] NICK: client " << client->getFd() << ": Password not sent. Can't register nickname." << std::endl;
        client->appendToSendBuffer(":irc.slytherin.com 464 * :You need to send a password before registering\r\n");
        client->markForDisconnect();

        for (size_t i = 0; i < data.getPollFds().size(); ++i)
        {
            if (data.getPollFds()[i].fd == client->getFd())
            {
                data.getPollFds()[i].events |= POLLOUT;
                break;
            }
        }
        client->setState(TO_DISCONNECT);
        return;
    }

    if (command.args.empty())
    {
        std::cerr << "[Server] NICK: client " << client->getFd() << " no argument." << std::endl;
        client->appendToSendBuffer(":irc.slytherin.com 431 * :No nickname given\r\n");
    }
    else if (does_nickname_have_channel_prefix(command.args[0]))
    {
        std::cerr << "[Server] NICK: client " << client->getFd() << " nickname cannot start by a channel prefix." << std::endl;
        client->appendToSendBuffer(":irc.slytherin.com 432 * " + command.args[0] + " :Erroneous nickname\r\n");
    }
    else if (data.nickNameIsAvailable(command.args[0]))
    {
        client->setNickName(command.args[0]);
        client->setState(SENT_NICK);
        std::cout << "[Server] Nickname assigned: " << command.args[0] << " (fd: " << client->getFd() << ")" << std::endl;
    }
    else
    {
        std::cerr << "[Server] NICK: client " << client->getFd() << " Nickname already in use: " << command.args[0] << std::endl;
        client->appendToSendBuffer(":irc.slytherin.com 433 * " + command.args[0] + " :Nickname is already in use\r\n");
    }

    for (size_t i = 0; i < data.getPollFds().size(); ++i)
    {
        if (data.getPollFds()[i].fd == client->getFd())
        {
            data.getPollFds()[i].events |= POLLOUT;
            break;
        }
    }

    if (client->isFullyRegistered())
    {
        client->setState(REGISTERED);
        send001(client);
        send002(client);
        send003(client);
        send004(client);
    }
}


void LIST (Client *client, Command command)
{
    (void)client;
    (void)command;
    Data &data = Data::getInstance();

    std::vector<Channel *>ChannelHub = data.getChannel();
    if (ChannelHub.empty())
    {
        client->appendToSendBuffer(":irc.slytherin.com 321 * Channel :Users  Name\r\n");
        client->appendToSendBuffer(":irc.slytherin.com 461 * None channel\n :LIST: you can create a channel whith JOIN\r\n");
    }
    else
    {
        client->appendToSendBuffer(":irc.slytherin.com 321 * Channel :Users  Name\r\n");
        for (size_t i = 0; i < ChannelHub.size() ; i++)
        {
            Channel *chan = ChannelHub[i];

            // Récupérer nom, nb users, topic (assure-toi que ces méthodes existent)
            std::string name = chan->getName();
            size_t userCount = chan->getClients().size();
            std::string topic = chan->getTopic();

            std::stringstream ss;
            ss << userCount;
            std::string userCountStr = ss.str();

            std::string line = ":irc.slytherin.com 322 * " + name + " " + userCountStr + " :" + topic + "\r\n";


            client->appendToSendBuffer(line);
        }
    }
    for (size_t i = 0; i < data.getPollFds().size(); ++i)
    {
        if (data.getPollFds()[i].fd == client->getFd())
        {
            data.getPollFds()[i].events |= POLLOUT;
            break;
        }
    }

}

void USER(Client* client, Command command)
{
    Data &data = Data::getInstance();

    // Vérifie si le client a passé le mot de passe si nécessaire
    if (client->getState() == CONNECTING || client->getState() == TO_DISCONNECT)
    {
        std::cerr << "[Server] USER: client " << client->getFd() << ": Password not sent. Can't register nickname." << std::endl;
        client->appendToSendBuffer(":irc.slytherin.com 464 * :Password required\r\n");
        client->setState(TO_DISCONNECT);

        for (size_t i = 0; i < data.getPollFds().size(); ++i)
        {
            if (data.getPollFds()[i].fd == client->getFd())
            {
                data.getPollFds()[i].events |= POLLOUT;
                break;
            }
        }
        return;
    }

    // USER attend au moins 4 arguments (username, hostname, servername, realname)
    if (command.args.size() < 4)
    {
        client->appendToSendBuffer(":irc.slytherin.com 461 * USER :Not enough parameters\r\n");
        return;
    }

    // Assignation des arguments
    client->setUserName(command.args[0]);    // username
    client->setHostName(command.args[1]);    // hostname
    client->setServerName(command.args[2]);  // servername
    client->setRealName(command.args[3]);    // trailing = realname

    std::cout << "[Server] USER assigned: username=" << client->getUserName()
              << ", hostname=" << client->getHostName()
              << ", servername=" << client->getServerName()
              << ", realname=" << client->getRealName()
              << " (client fd: " << client->getFd() << ")" << std::endl;

    client->setState(SENT_USER);

if (client->isFullyRegistered())
{
    client->setState(REGISTERED);
    send001(client);
    send002(client);
    send003(client);
    send004(client);
}
    // Puis activer POLLOUT pour dire à poll qu'on a des données à envoyer
    for (size_t i = 0; i < data.getPollFds().size(); ++i)
    {
        if (data.getPollFds()[i].fd == client->getFd())
        {
            data.getPollFds()[i].events |= POLLOUT;
            break;
        }
    }

}


void JOIN (Client *client, Command command)
{
    Data &data = Data::getInstance();

    if (client->getState() != REGISTERED)
    {
        client->appendToSendBuffer("ERROR :<JOIN> client not registered \r\n");
        std::cout << "[server] : client " << client->getFd() << " ERROR :<JOIN> client " << client->getFd() << " not registered" << std::endl;
    }
    else if (command.args.empty())
    {
        client->appendToSendBuffer("ERROR :<JOIN> no argument\r\n");
        std::cout << "[server] : client " << client->getFd() << " ERROR :<JOIN> no argument." << std::endl;
    }
    else if (command.args.size() >= 3 && !command.args[0].empty() && !command.args[1].empty() && !command.args[2].empty())
    {
        client->appendToSendBuffer("ERROR :<JOIN> usage : <channel> <password> \r\n");
        std::cout << "[server] : client " << client->getFd() << " ERROR :<JOIN> usage : <channel> <password> ." << std::endl;
    }
    else if (!command.args[0].empty())
    {
        if (command.args[0][0] != '#')
        {
            client->appendToSendBuffer("ERROR :<JOIN> channel's name should begin by #.\r\n");
            std::cout << "[server] : client " << client->getFd() << "ERROR :<JOIN> channel's name should begin by #." << std::endl;
        }
    }
    else if (data.getThisChannel(command.args[0]) && data.getThisChannel(command.args[0])->getIsInviteOnly() == true)
    {
        client->appendToSendBuffer("ERROR :<JOIN> channel is Invite-only.\r\n");
        std::cout << "[server] : client " << client->getFd() << " ERROR :<JOIN> channel is Invite-only." << std::endl;
    }
    else if (!data.getThisChannel(command.args[0])->getChannelKey().empty() && (command.args[1].empty() || command.args[1] != data.getThisChannel(command.args[0])->getChannelKey()))
    {
        client->appendToSendBuffer("ERROR :<JOIN> Invalid channel key.\r\n");
    }
    else if (data.getThisChannel(command.args[0])->getUsersLimit() > 0 && data.getThisChannel(command.args[0])->getClients().size() >= static_cast<size_t>(data.getThisChannel(command.args[0])->getUsersLimit()))
    {
        client->appendToSendBuffer("ERROR :<JOIN> Channel user limit has been reached.\r\n");
        std::cout << "[server] : client " << client->getFd() << " ERROR :<JOIN> Channel user limit has been reached." << std::endl;
    }
    else if (data.getThisChannel(command.args[0]))
    {
        data.getThisChannel(command.args[0])->addClient(client);
        std::cout << "[server] : client " << client->getFd() << "added to channel " <<  data.getThisChannel(command.args[0])->getName() << std::endl;
    }
    else
    {
        Channel* channel = new Channel(command.args[0]);
        data.getChannel().push_back(channel);
        channel->addClient(client);
        std::cout << "[server] : client " << client->getFd() << "created new channel " << channel->getName() << std::endl;
    }

    for (size_t i = 0; i < data.getPollFds().size(); ++i)
    {
        if (data.getPollFds()[i].fd == client->getFd())
        {
            data.getPollFds()[i].events |= POLLOUT;
            break;
        }
    }

}

void UNKNOWN (Client *client, Command command)
{
    (void)client;
    (void)command;
    Data &data = Data::getInstance();

    client->appendToSendBuffer("ERROR : Unknown command.\r\n");
    std::cout << "[SERVER] ERROR : Unknown command." <<std::endl;

    for (size_t i = 0; i < data.getPollFds().size(); ++i)
    {
        if (data.getPollFds()[i].fd == client->getFd())
        {
            data.getPollFds()[i].events |= POLLOUT;
            break;
        }
    }
}

void PING(Client* client, Command command) 
{
    Data &data = Data::getInstance();

    if (command.args.empty()) 
    {
        // Pas d’argument, ignore ou envoie une erreur.
        return;
    }
    std::string response = "PONG :" + command.args[0] + "\r\n";
    client->appendToSendBuffer(response);

    for (size_t i = 0; i < data.getPollFds().size(); ++i)
    {
        if (data.getPollFds()[i].fd == client->getFd())
        {
            data.getPollFds()[i].events |= POLLOUT;
            break;
        }
    }

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
    else if (command.name == "PRIVMSG")
    {

    }
    else if (command.name == "KICK")
    {

    }
    else if (command.name == "TOPIC")
    {

    }
    else if (command.name == "MODE")
    {

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

