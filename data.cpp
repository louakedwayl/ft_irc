#include "data.hpp"

Data& Data::getInstance() 
{
    static Data data; // instance unique et locale statique
    return data;
}

void Data::setServerSocket(int sock) 
{
        _server_socket = sock ;
}

int Data::getServerSocket() const 
{
        return _server_socket;
}

void Data::setPort(int port) 
{
        _port = port; 
}

int Data::getPort() const 
{
        return _port ;
}

unsigned long Data::hash_string(const std::string& str) const
{
    unsigned long hash = 5381; // Initialize hash to arbitrary seed

    for (std::string::size_type i = 0; i < str.length(); i++)
        hash = ((hash * 27) + hash) + str[i];
    
    return (hash);
}

void Data::setPassword(const std::string& pass) 
{
        _password = hash_string(pass) ;
}

bool Data::checkPassword(const std::string& attempt) const 
{
        if (this->hash_string(attempt) ==_password)
                return true;
        else 
                return false;
}

Data::Data()
{
    Channel *bot = new Channel("#bot");
    Channel *general = new Channel("#general");

    _channels.push_back(bot);
    _channels.push_back(general);
}

Data::~Data()
{
    clearChannel();
    clearClients();
}

std::vector<struct pollfd>& Data::getPollFds()
{
        return _poll_fds;
}

void Data::addPollFd(int fd) 
{
    struct pollfd pfd;
    pfd.fd = fd;
    pfd.events = POLLIN;  // on surveille la lecture
    pfd.revents = 0;
    _poll_fds.push_back(pfd);
}

void Data::removePollFdAtIndex(size_t i) 
{
    if (i < _poll_fds.size()) {
        _poll_fds.erase(_poll_fds.begin() + i);
    }
}

void Data::addClient(Client* client) 
{
    _clients.push_back(client);
}

Client* Data::getClientByFd(int fd) 
{
    for (size_t i = 0; i < _clients.size(); ++i) {
        if (_clients[i]->getFd() == fd) {
            return _clients[i];  // Retourne un pointeur vers le client trouvé
        }
    }
    return NULL;  // Aucun client avec ce fd
}

void Data::removeClientByFd(int fd)
{
    for (std::vector<Client*>::iterator it = _clients.begin(); it != _clients.end(); ++it) {
        if ((*it)->getFd() == fd)
        {
            delete *it;
            _clients.erase(it);
            return;  // Client supprimé, on sort
        }
    }
}

void Data::clearClients()
{
    for (size_t i = 0; i < _clients.size(); ++i)
    {
        delete _clients[i];  // libère la mémoire
    }
    _clients.clear();  // vide le vecteur (supprime tous les pointeurs)
}

void Data::clearChannel()
{
    for (size_t i = 0; i < _channels.size(); ++i)
    {
        delete _channels[i];  // libère la mémoire
    }
    _channels.clear();  // vide le vecteur (supprime tous les pointeurs)
}

void Data::shutdown()
{
    for (std::vector<Client*>::iterator it = _clients.begin(); it != _clients.end(); ++it)
    {
        close((*it)->getFd());
    }
    clearClients();
    clearChannel();
    close (_server_socket);
    std::cout << "Server is shutting down.";
}

bool Data::nickNameIsAvailable(const std::string& nick) const
{
    for (std::vector<Client*>::const_iterator it = _clients.begin(); it != _clients.end(); ++it)
    {
        if ((*it)->getNickName() == nick)
            return false;  // Nick déjà pris
    }
    return true;  // Nick libre
}

std::vector<Channel*>& Data::getChannel() 
{
    return _channels;
}

Channel* Data::getThisChannel(const std::string& name) const
{
    for (size_t i = 0 ; i < _channels.size() ; ++i)
    {
        if (_channels[i] && _channels[i]->getName() == name)
            return _channels[i];
    }
    return NULL;
}

void Data::enablePollOutIfNeeded(Client* client)
{
    if (!client)
        return;
    if (client->getSendBuffer().empty())
    {
        return;
    }
    // Activation POLLOUT
    for (size_t i = 0; i < getPollFds().size(); ++i)
    {
        if (getPollFds()[i].fd == client->getFd())
        {
            getPollFds()[i].events |= POLLOUT;
            return;
        }
    }
}

Client* Data::getClientByNickname(const std::string& nickname)
{
    for (size_t i = 0; i < _clients.size(); ++i)
    {
        if (_clients[i] && _clients[i]->getNickName() == nickname)
            return _clients[i];
    }
    return NULL; // Pas trouvé
}

void Data::deleteChannel(const std::string& channelName)
{
    for (std::vector<Channel*>::iterator it = _channels.begin(); it != _channels.end(); ++it)
    {
        if ((*it)->getName() == channelName)
        {
            delete *it;               // libère la mémoire du channel
            _channels.erase(it);       // supprime le pointeur du vector
            break;                   // on sort après suppression
        }
    }
}
