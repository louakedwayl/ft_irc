#ifndef CHANNEL_HPP
#define CHANNEL_HPP

#include "data.hpp"

class Channel
{
    private :
            std::string _name;
            std::vector<Client*> _clients; // clients présents dans le channel

    public:
            Channel(const std::string& name);

            const std::string& getName() const ;
            const std::vector<Client*>& getClients() const ;

            void addClient(Client* client);
            void removeClient(Client* client);
            bool hasClient(Client* client) const;


            // Exemple : broadcast message à tous les clients du channel sauf l'expéditeur
            void broadcastMessage(Client* sender, const std::string& message) const;
};

#endif

