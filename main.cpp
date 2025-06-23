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
        exit(1);
    }
    else if (status == 0) 
    {
        std::cout << "[Server] ⏳ Poll timeout reached: no sockets ready. Looping again..." << std::endl;
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

    Client new_client(client_fd);
    // Préparer la reponse a la requete irc de connection dans le buffer d'envoi du client
    std::ostringstream welcome_msg;
    welcome_msg << "Welcome. You are client fd [" << client_fd << "]\n";
    new_client.appendToSendBuffer(welcome_msg.str());  // On remplit le buffer d'envoi
    
    data.addClient(new_client);
    data.addPollFd(client_fd);

    std::cout << "[Server] Accepted new connection on client socket " << client_fd << std::endl;
    

    int status = send(client_fd, new_client.getSendBuffer().c_str(), new_client.getSendBuffer().size(), 0);
    if (status == -1) 
    {
        std::cerr << "[Server] Send error to client fd" << client_fd <<": " << strerror(errno);
    }
}

void read_data_from_socket(int i, Data &data)
{
    char buffer[BUFSIZ];
    char msg_to_send[BUFSIZ];
    int bytes_read;
    int status;

    int sender_fd = data.getPollFds()[i].fd;

    memset(buffer, 0, sizeof buffer);
    bytes_read = recv(sender_fd, buffer, sizeof(buffer) - 1, 0);
    if (bytes_read <= 0) {
        if (bytes_read == 0) {
            printf("[%d] Client socket closed connection.\n", sender_fd);
        } else {
            fprintf(stderr, "[Server] Recv error: %s\n", strerror(errno));
        }
        close(sender_fd);
        data.removePollFdAtIndex(i);
    } else {
        printf("[%d] Got message: %s", sender_fd, buffer);

        snprintf(msg_to_send, sizeof(msg_to_send), "[%d] says: %s", sender_fd, buffer);

        for (size_t j = 0; j < data.getPollFds().size(); j++) {
            int dest_fd = data.getPollFds()[j].fd;
            if (dest_fd != data.getServerSocket() && dest_fd != sender_fd) {
                status = send(dest_fd, msg_to_send, strlen(msg_to_send), 0);
                if (status == -1) {
                    fprintf(stderr, "[Server] Send error to client fd %d: %s\n", dest_fd, strerror(errno));
                }
            }
        }
    }
}

