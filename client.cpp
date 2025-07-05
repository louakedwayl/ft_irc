#include "data.hpp"
#include "channel.hpp"


Client::Client(int fd) : _fd(fd) , _state(CONNECTING){
    _UserName = "";
    _nickName = "";
    _registered = 0;
}

int Client::getFd() const { return _fd; }

void Client::setNickName(const std::string& name) { _nickName = name; }

std::string Client::getNickName() const { return _nickName; }

void Client::setUserName(const std::string& name) { _UserName = name; }

std::string Client::getUserName() const { return _UserName; }

void Client::setState(ClientState state) { _state = state; }

ClientState Client::getState() const { return _state; }

void Client::appendToSendBuffer(const std::string& msg) {
    _send_buffer += msg;
}

std::string& Client::getSendBuffer() {
    return _send_buffer;
}


void Client::appendToRecvBuffer(const std::string& msg) 
{
    _recv_buffer += msg;
}

std::string& Client::getRecvBuffer() 
{
    return _recv_buffer;
}


void Client::eraseFromSendBuffer(size_t n) 
{
    if (n >= _send_buffer.size()) {
        _send_buffer.clear();
    } else {
        _send_buffer.erase(0, n);
    }
}

void Client::sendMessage(const std::string& message) const
{
    const char* msg_cstr = message.c_str();
    size_t total_sent = 0;
    size_t msg_len = message.length();

    while (total_sent < msg_len)
    {
        int sent = send(_fd, msg_cstr + total_sent, msg_len - total_sent, 0);
        if (sent == -1)
        {
            // Gestion basique des erreurs
            perror("send");
            break;
        }
        total_sent += sent;
    }
}

void Client::addChannel(Channel* channel)
{
    if (std::find(_channels.begin(), _channels.end(), channel) == _channels.end()) 
    {
        _channels.push_back(channel);
    }
}

void Client::removeChannel(Channel* channel) 
{
    std::vector<Channel*>::iterator it = std::find(_channels.begin(), _channels.end(), channel);
    if (it != _channels.end()) {
        _channels.erase(it);
    }
}


const std::vector<Channel*>& Client::getChannels() const 
{
    return _channels;
}

void Client::markForDisconnect()
{
    _state = TO_DISCONNECT;
}


void Client::setHostName(const std::string &name) { _hostName = name; }

const std::string& Client::getHostName() const { return _hostName;} 


void Client::setServerName(const std::string &name) { _hostName = name; }

const std::string& Client::getServerName() const { return _hostName;} 

// Dans Client.cpp
void Client::setRealName(const std::string& realName)
{
    _realName = realName;
}

const std::string& Client::getRealName() const
{
    return _realName;
}

bool Client::isFullyRegistered() const
{
    return !getNickName().empty() && !getUserName().empty();
}

std::string Client::getPrefix() const
{
    return this->_nickName + "!" + this->_UserName + "@" + this->_hostName;
}

std::string Client::getChannelsList() const
{
    std::string list;
    for (std::vector<Channel*>::const_iterator it = _channels.begin(); it != _channels.end(); ++it) {
        if (!list.empty())
            list += " ";
        list += (*it)->getName();
    }
    return list;
}

int Client::get_registered() const
{
    return _registered;
}
void Client::set_registered()
{
    _registered = 1;
}