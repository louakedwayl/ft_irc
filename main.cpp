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
    { // Boucle principale
        // Sonde les sockets prêtes (avec timeout de 2 secondes)
        int status = poll(data.getPollFds().data(), data.getPollFds().size(), 2000);
        if (status == -1) 
        {
            std::cerr << "[Server] Poll error: " << strerror(errno) << std::endl;
            exit(1);
        }
        else if (status == 0) 
        {
            // Aucun descipteur de fichier de socket n'est prêt //donc on retourne au denbut de la boucle
            std::cout << "[Server] ⏳ Poll timeout reached: no sockets ready. Looping again..." << std::endl;
            continue;
        }

        // Boucle sur notre tableau de sockets
        for (size_t i = 0; i < data.getPollFds().size(); i++) 
        {
            if ((data.getPollFds()[i].revents & POLLIN) == 0) 
            {
                // La socket n'est pas prête à être lue
                // on s'arrête là et on continue la boucle
                continue ;
            }
        std::cout << "[Server] socket FD " << data.getPollFds()[i].fd << " is ready for ";

            if (data.getPollFds()[i].revents & POLLIN)
                std::cout << "reading ";
            if (data.getPollFds()[i].revents & POLLOUT)
                std::cout << "writing ";
            if (data.getPollFds()[i].revents & POLLERR)
                std::cout << "error ";
            if (data.getPollFds()[i].revents & POLLHUP)
                std::cout << "hangup ";
            std::cout << std::endl; 
            if (data.getPollFds()[i].revents & POLLIN) 
            {
                if (data.getPollFds()[i].fd == data.getServerSocket()) 
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
        return (EXIT_SUCCESS);
    }
}
}


void accept_new_connection(Data &data)
{
    int client_fd = accept(data.getServerSocket(), NULL, NULL);
    if (client_fd == -1) {
        fprintf(stderr, "[Server] Accept error: %s\n", strerror(errno));
        return;
    }

    // Ajout dans le vecteur poll_fds via ta méthode
    data.addPollFd(client_fd);

    printf("[Server] Accepted new connection on client socket %d.\n", client_fd);

    char msg_to_send[BUFSIZ];
    snprintf(msg_to_send, sizeof(msg_to_send), "Welcome. You are client fd [%d]\n", client_fd);

    int status = send(client_fd, msg_to_send, strlen(msg_to_send), 0);
    if (status == -1) {
        fprintf(stderr, "[Server] Send error to client %d: %s\n", client_fd, strerror(errno));
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

