#include "data.hpp"

int main(int argc, char **argv)
{
    Data& data = Data::getInstance();
    parse(argc, argv);
    std::cout << "[Server] Starting IRC server..." << std::endl;

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
            std::cout << "[Server] Waiting for events..." << std::endl;
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


