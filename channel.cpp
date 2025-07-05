#include "data.hpp"

Channel:: Channel(const std::string& name): _name(name)
{
    this->_is_invite_only = false;
    this->_is_topic_restricted_to_operators = false;
    this->_channel_key = "";
    this->_users_limit = -1;
}

const std::string& Channel::getName() const { return _name; }

const std::vector<Client*>& Channel::getClients() const { return _clients; }

const std::vector<Client*>& Channel::getInvited() const { return _invited; }


void Channel::addInvited(Client* client)
{
    if (std::find(_invited.begin(), _invited.end(), client) == _invited.end())
        _invited.push_back(client);
}

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
    Data& data = Data::getInstance();
    for (size_t i = 0; i < _clients.size(); ++i)
    {
        if (_clients[i] != sender)
        {
            _clients[i]->sendMessage(message); // méthode à définir dans Client
            data.enablePollOutIfNeeded(_clients[i]);
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

bool Channel::getIsInviteOnly()
{
    return _is_invite_only;
}
bool Channel::getIsTopicRestricted()
{
    return _is_topic_restricted_to_operators;
}
std::string Channel::getChannelKey()
{
    return _channel_key;
}
int Channel::getUsersLimit()
{
    return _users_limit;
}

void Channel::setIsInviteOnly(bool x)
{
    _is_invite_only = x;
}
void Channel::setIsTopicRestricted(bool x)
{
    _is_topic_restricted_to_operators = x;
}
void Channel::setChannelKey(std::string s)
{
    _channel_key = s;
}
void Channel::setUsersLimit(int x)
{
    _users_limit = x;
}

const std::string& Channel::getTopic() const { return _topic;}

void  Channel::setTopic(const std::string& topic) { _topic = topic;}

int Channel::getSize() const
{
    return _clients.size();
}
