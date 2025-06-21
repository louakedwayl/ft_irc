#ifndef DATA_hpp
#define DATA_hpp

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

#include "client.hpp"
#include "channel.hpp"

#define PORT 4243

class Data
{
    public:
        int _server_socket;
        int _port;
        std::string _password;
        std::vector<Client> _clients;
        std::vector<Channel> _channels;
};

//parse.cpp
int parse(int argc, char **argv, Data& data);


#endif