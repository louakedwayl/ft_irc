#pragma once

#include "data.hpp"
#include "channel.hpp"

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
        std::string _nickName;
        std::string _UserName;
        std::vector<Channel*> _channels;
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
        void sendMessage(const std::string& message) const;

        void addChannel(Channel* channel);
        void removeChannel(Channel* channel);
        const std::vector<Channel*>& getChannels() const;

};
