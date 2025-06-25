#include "data.hpp"
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

void PASS (Client *client, Command command)
{
    Data &data = Data::getInstance();

    if (!command.args.empty() && data.checkPassword(command.args[0]))
    {
        client->appendToSendBuffer(":irc.slytherin.com NOTICE AUTH :🐍Welcome to the Slytherin IRC server 🐍!\r\n");
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


void NICK (Client *client, Command command)
{
     Data &data = Data::getInstance();

     if (client->getState() == CONNECTING)
     {
        std::cerr << "[Server] NICK: client " << client->getFd() << ": Password not sent. Can't register nickname."<< std::endl;
        client->appendToSendBuffer(":irc.slytherin.com 433 * <nick> : You need send a password for register \r\n");
        for (size_t i = 0; i < data.getPollFds().size(); ++i)
        {
            if (data.getPollFds()[i].fd == client->getFd())
            {
                data.getPollFds()[i].events |= POLLOUT;
                break;
            }
        }
        return ;
     }

    // verifier si nickname deja pris 
    // si oui  envoies ce message d’erreur au client. :irc.slytherin.com 433 * <nick> :Nickname is already in use
    if (command.args.empty())
    {
        std::cerr << "[Server] NICK: client " << client->getFd() << "no argument." << std::endl;
        client->appendToSendBuffer(":irc.slytherin.com 433 * <nick> : no argument\r\n");
    } 
    else if (does_nickname_have_channel_prefix(command.args[0])) 
    {
        std::cerr << "[Server] NICK: client " << client->getFd() << "nickname cannot start by a channel prefix."<< std::endl;
        client->appendToSendBuffer(":irc.slytherin.com 433 * <nick> : nickname cannot start by a channel prefix\r\n");
    }
    else if (command.args[0] == "<nick>_")
    {
        return;
    }
    else if (data.nickNameIsAvailable(command.args[0]))
    {
            client->setNickName(command.args[0]);
            client->setState(SENT_NICK);
            std::cout << "[Server] Nickname assigned: " << command.args[0]
                  << " (fd: " << client->getFd() << ")" << std::endl;
            client->appendToSendBuffer(":irc.slytherin.com 433 * <NICK> : Register as " + client->getNickName() + "\r\n");
    }
    else
    {
        std::cerr << "[Server] NICK: client " << client->getFd() <<" Nickname already in use: " << command.args[0] << std::endl;
        client->appendToSendBuffer(":irc.slytherin.com 433 * <nick> :Nickname is already in use\r\n");
    }
    
    for (size_t i = 0; i < data.getPollFds().size(); ++i)
    {
        if (data.getPollFds()[i].fd == client->getFd())
        {
            data.getPollFds()[i].events |= POLLOUT;
            break;
        }
    }

    if (!client->getUserName().empty() && !client->getNickName().empty())
        client->setState(REGISTERED);
}

void LIST (Client *client, Command command)
{
    (void)client;
    (void)command;
    Data &data = Data::getInstance();

    std::vector<Channel *>ChannelHub = data.getChannel();
    if (ChannelHub.empty())
        std::cout << "None Channel .\n you can create a channel whith JOIN" << std::endl ;
}

void USER (Client *client, Command command)
{
    Data &data = Data::getInstance();

     if (client->getState() == CONNECTING)
     {
        std::cerr << "[Server] USER: client " << client->getFd() << ": Password not sent. Can't register nickname."<< std::endl;
        client->appendToSendBuffer(":irc.slytherin.com 433 * <user> : You need send a password for register \r\n");
        for (size_t i = 0; i < data.getPollFds().size(); ++i)
        {
            if (data.getPollFds()[i].fd == client->getFd())
            {
                data.getPollFds()[i].events |= POLLOUT;
                break;
            }
        }
        return ;
    }

    if (command.args.empty())
    {
        client->appendToSendBuffer("ERROR :<USER> no argument\r\n");

    }
    else if (data.checkPassword(command.args[0]))
    {
        std::cerr << "[Server] USER: client " << client->getFd() << ": Password not sent. Can't register nickname."<< std::endl;
        client->appendToSendBuffer(":irc.slytherin.com 433 * <USER> : You need send a password for register \r\n");
        client->setState(SENT_PASS);
    }
    else
    {
        client->setUserName(command.args[0]);
        client->setState(SENT_USER);
        std::cout << "[Server] Username assigned: " << command.args[0]
                  << " (fd: " << client->getFd() << ")" << std::endl;
        client->appendToSendBuffer(":irc.slytherin.com 433 * <USER> : Register as " + client->getUserName() + "\r\n");
    }
    for (size_t i = 0; i < data.getPollFds().size(); ++i)
    {
        if (data.getPollFds()[i].fd == client->getFd())
        {
            data.getPollFds()[i].events |= POLLOUT;
            break;
        }
    }
    if (!client->getUserName().empty() && !client->getNickName().empty())
        client->setState(REGISTERED);
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
    else if (command.name == "JOIN")
    {

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
        // commande inconnue renvoyer error
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

