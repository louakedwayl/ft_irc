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
#include <sstream>
#include <signal.h>
#include "CMD.hpp"

class Data
{
    private :
        int _server_socket;
        int _port;
        unsigned long _password;
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

        std::vector<Channel*>& getChannel() ;
        Channel*getThisChannel(const std::string& name) const;

        unsigned long hash_string(const std::string& str) const;
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
        void enablePollOutIfNeeded(Client* client);
        
        Client* getClientByNickname(const std::string& nickname);
  
        void deleteChannel(const std::string& channelName);
        void clearChannel();
};

//data.cpp
int addPollFd(int fd);

//parse.cpp
int parse(int argc, char **argv);

//server.cpp
int make_server();
int create_server_socket(void);
int server_listen();

//NICK.cpp
void NICK(Client *client, Command command);
bool	does_nickname_have_channel_prefix(std::string const & nickname);

//answer.cpp
void send001(Client* client);
void send002(Client* client);
void send003(Client* client);
void send004(Client* client);

//LIST.cpp
void LIST(Client* client, Command command);

//PASS.cpp
void PASS (Client *client, Command command);

//JOIN.cpp
void JOIN(Client* client, Command command);

//CAP.cpp
void CAP (Client* client);

//USER.cpp
void USER(Client* client, Command command);

//PING.cpp
void PING(Client* client, Command command);

//PRIVMSG.cpp
void PRIVMSG(Client* client, Command command);

//UNKNOWN.cpp
void UNKNOWN(Client *client, Command command);

//QUIT_SERV.cpp
void QUIT_SERV (Client *client, Command command);

//INVITE.cpp
void INVITE(Client* client, Command command);

//KICK.cpp
void KICK(Client* client, Command command);

//PART.cpp
void PART(Client* client, Command command);
std::vector<std::string> split(const std::string& s, char delimiter);

//WHOIS.cpp
void WHOIS(Client* client, Command command);

//MODE.cpp
void MODE(Client* client, Command command);

//TOPIC.cpp
void TOPIC(Client* client, Command command);

//handleCommand.cpp
void handleCommand(Client* client, Command command);
void parseCommands(Client* client, const char* buffer);

//handlePollin.cpp
void accept_new_connection(Data &data);
void read_data_from_socket(int i, Data& data);
void handlePollin(int fd, int i);

//ft_irc.cpp
void ft_irc ();

//send.cpp
void SEND(Client* client, Command command);

//signal.cpp
void handle_signal();