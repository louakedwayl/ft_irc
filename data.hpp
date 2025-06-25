#pragma once

#include <cerrno>
#include <errno.h>
#include <netdb.h>
#include <poll.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/poll.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>
#include <iostream>
#include <vector>
#include <sstream>
#include <string>
#include <cstring>
#include <cstdio>
#include "client.hpp"
#include "channel.hpp"
#include <algorithm>
#include "CMD.hpp"


class Data
{
    private :
        int _server_socket;
        int _port;
        std::string _password;
        std::vector<Client*> _clients;
        std::vector<Channel*> _channels;
        std::vector<struct pollfd> _poll_fds;
        
        Data();
        ~Data();
        Data(const Data&);
        Data& operator=(const Data&);
        
    public :
        static Data& getInstance();
    
        int getServerSocket() const;
        void setServerSocket(int sock);

        int getPort() const;
        void setPort(int port);

        std::vector<Channel*>getChannel() const;
        Channel*getThisChannel(const std::string& name) const;


        void setPassword(const std::string& pass);
        bool checkPassword(const std::string& attempt) const;
        
        std::vector<struct pollfd>& getPollFds();
        void addPollFd(int fd);
        void removePollFdAtIndex(size_t i);

        
        void addClient(Client* client);
        Client* getClientByFd(int fd);
        void removeClientByFd(int fd);

        void shutdown();
        void clearClients();

        bool nickNameIsAvailable(const std::string& nick) const;

};

//data.cpp
int addPollFd(int fd);

//parse.cpp
int parse(int argc, char **argv);

//server.cpp
int make_server();
int create_server_socket(void);
int server_listen();
