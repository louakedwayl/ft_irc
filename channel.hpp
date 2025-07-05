#pragma once

#include "data.hpp"
#include "client.hpp"

class Client;

class Channel
{
    private :
            std::string _name;
            std::string _topic;
            std::vector<Client*> _clients; // clients présents dans le channel
            std::vector<Client*> _invited;
            std::vector<Client*> _operators;

            bool                _is_invite_only;
            bool                _is_topic_restricted_to_operators;
            std::string         _channel_key;
            int        _users_limit;

    public:
            Channel(const std::string& name);

            const std::string& getName() const ;
            const std::vector<Client*>& getClients() const ;

            void addInvited(Client* client);
            const std::vector<Client*>& getInvited() const ;

            const std::string& getTopic() const ;
            void setTopic(const std::string& topic);

            bool getIsInviteOnly();
            bool getIsTopicRestricted();
            std::string getChannelKey();
            int getUsersLimit();

            void setIsInviteOnly(bool x);
            void setIsTopicRestricted(bool x);
            void setChannelKey(std::string s);
            void setUsersLimit(int x);


            void addClient(Client* client);
            void removeClient(Client* client);
            bool hasClient(Client* client) const;

            // Exemple : broadcast message à tous les clients du channel sauf l'expéditeur
            void broadcastMessage(Client* sender, const std::string& message) const;

            void addOperator(Client* client);
            void removeOperator(Client* client);
            bool isOperator(Client* client) const;

            int getSize() const;
};
