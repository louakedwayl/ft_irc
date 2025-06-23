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

void Channel::addOperator(Client* client) 
{
    if (std::find(_operators.begin(), _operators.end(), client) == _operators.end())
    {
        _operators.push_back(client);
    }
}

void Channel::removeOperator(Client* client) 
{
    std::vector<Client*>::iterator it = std::find(_operators.begin(), _operators.end(), client);
    if (it != _operators.end()) 
    {
        _operators.erase(it);
    }
}

bool Channel::isOperator(Client* client) const 
{
    return std::find(_operators.begin(), _operators.end(), client) != _operators.end();
}