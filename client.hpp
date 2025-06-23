#ifndef CLIENT_HPP
#define CLIENT_HPP

#include "data.hpp"

enum ClientState 
{
    CONNECTING,
    SENT_PASS,
    SENT_NICK,
    SENT_USER,
    REGISTERED
};

class Client
{
    private :
        int _fd;
        std::string _name;
        std::string _recv_buffer;
        std::string _send_buffer;
        ClientState _state;
        
    public :
        Client(int fd);

        int getFd() const ;
        void setName(const std::string& name);
        std::string getName() const ;

        void setState(ClientState state);
        ClientState getState() const ;
        void appendToSendBuffer(const std::string& msg);  // pour ajouter du texte
        std::string& getSendBuffer();
        void eraseFromSendBuffer(size_t n);
};

#endif