#include "data.hpp"

Channel:: Channel(const std::string& name): _name(name)
{

}

const std::string& Channel::getName() const { return _name; }

const std::vector<Client*>& Channel::getClients() const { return _clients; }

void Channel::addClient(Client* client)
{
    if (std::find(_clients.begin(), _clients.end(), client) == _clients.end())
        _clients.push_back(client);
}

void Channel::removeClient(Client* client)
{
    _clients.erase(std::remove(_clients.begin(), _clients.end(), client), _clients.end());
}

bool Channel::hasClient(Client* client) const
{
    return std::find(_clients.begin(), _clients.end(), client) != _clients.end();
}

void Channel::broadcastMessage(Client* sender, const std::string& message) const
{
    for (size_t i = 0; i < _clients.size(); ++i)
    {
        if (_clients[i] != sender)
        {
            _clients[i]->sendMessage(message); // méthode à définir dans Client
        }
    }
}