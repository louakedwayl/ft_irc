#include "data.hpp"


Client::Client(int fd) : _fd(fd) , _state(CONNECTING){}

int Client::getFd() const { return _fd; }

void Client::setName(const std::string& name) { _name = name; }

std::string Client::getName() const { return _name; }

void Client::setState(ClientState state) { _state = state; }

ClientState Client::getState() const { return _state; }

void Client::appendToSendBuffer(const std::string& msg) {
    _send_buffer += msg;
}

std::string& Client::getSendBuffer() {
    return _send_buffer;
}

void Client::eraseFromSendBuffer(size_t n) {
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