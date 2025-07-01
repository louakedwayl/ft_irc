#include "data.hpp"

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



void read_data_from_socket(int i, Data& data)
{
    char buffer[BUFSIZ];
    int bytes_read;
    int sender_fd = data.getPollFds()[i].fd;

    std::memset(buffer, 0, sizeof(buffer));
    bytes_read = recv(sender_fd, buffer, sizeof(buffer) - 1, 0);

    Client *client = data.getClientByFd(sender_fd);
    if (!client)
    {
        return ;
    }
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
    client->appendToRecvBuffer(buffer);
    std::string& recvBuffer = client->getRecvBuffer();
    size_t pos;
    // Boucle tant qu'on a des lignes complètes (terminées par \r\n)
    while ((pos = recvBuffer.find("\r\n")) != std::string::npos)
    {
        std::string line = recvBuffer.substr(0, pos);
        parseCommands(client, line.c_str()); // Traite une ligne complète
        recvBuffer.erase(0, pos + 2); // Enlève la ligne traitée du buffer
    }
}

void handlePollin(int fd, int i)
{
    Data& data = Data::getInstance();

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