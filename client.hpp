#pragma once

#include "data.hpp"
#include "channel.hpp"

enum ClientState
{
    CONNECTING,     // État initial du client : juste connecté, pas encore identifié
    SENT_PASS,      // Le client a envoyé PASS, mot de passe correct
    TO_DISCONNECT,  // Le client est marqué pour être déconnecté proprement (ex. : mauvais mot de passe)
    SENT_NICK,      // Le client a envoyé NICK
    SENT_USER,      // Le client a envoyé USER
    REGISTERED     // Le client est totalement connecté (PASS + NICK + USER valides)
};

class Client
{
    private :
        int _fd;
        std::string _nickName;
        std::string _UserName;
        std::string _hostName;
        std::string _serverName;
        std::string _realName;
        std::vector<Channel*> _channels;
        std::string _recv_buffer;
        std::string _send_buffer;
        ClientState _state;
        int         _registered;
        
    public :
        Client(int fd);

        int getFd() const ;
        void setNickName(const std::string& name);  
        std::string getNickName() const ;

        void setUserName(const std::string& name);
        std::string getUserName() const ;

        void setRealName(const std::string& realName);
        const std::string& getRealName() const;

        void setHostName(const std::string& name);
        const std::string& getHostName() const ;

        void setServerName(const std::string& name);
        const std::string& getServerName() const ;

        void setState(ClientState state);
        ClientState getState() const ;
        void appendToSendBuffer(const std::string& msg);  // pour ajouter du texte
        std::string& getSendBuffer();
        void eraseFromSendBuffer(size_t n);
        void sendMessage(const std::string& message) const;

        void addChannel(Channel* channel);
        void removeChannel(Channel* channel);
        const std::vector<Channel*>& getChannels() const;

        void markForDisconnect();

        bool isFullyRegistered() const;

        std::string getPrefix() const;

        std::string getChannelsList() const;

        void appendToRecvBuffer(const std::string& msg) ;
        std::string& getRecvBuffer() ;

        int get_registered() const;
        void set_registered();
};
