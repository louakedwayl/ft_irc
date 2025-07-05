#include "data.hpp"

void ft_irc ()
{
    Data& data = Data::getInstance();

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
            if (revents & (POLLERR | POLLHUP))
            {
                std::cerr << "[Server] Closing connection on fd " << fd << " due to error/hangup." << std::endl;
                close(fd);
                data.removePollFdAtIndex(i);
                data.removeClientByFd(fd);
                --i;
                continue;
            }
            if (revents & POLLIN)
                handlePollin(fd, i);
            if (revents & POLLOUT)
            {
                // Socket prête à écrire, on tente d’envoyer les données du buffer
                Client* client = data.getClientByFd(fd);
                if (!client) 
                {
                    std::cerr << "[Server] POLLOUT but no client found for fd " << fd << std::endl;
                    continue;
                }
                if (client && !client->getSendBuffer().empty())
                {
                    int sent = send(fd, client->getSendBuffer().c_str(), client->getSendBuffer().size(), 0);
                    if (sent <= 0)  // gère sent == 0 et sent == -1
                    {
                        if (sent == 0)
                            std::cerr << "[Server] Send returned 0 on fd " << fd << ", closing connection." << std::endl;
                        else
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
                            data.getPollFds()[i].events &= ~POLLOUT;
                    }
                }
                if (client->getSendBuffer().empty() && client->getState() == TO_DISCONNECT)
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
