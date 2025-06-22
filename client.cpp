#include "data.hpp"


Client::Client(int fd) : _fd(fd) , _state(CONNECTING){}

int Client::getFd() const { return _fd; }

void Client::setName(const std::string& name) { _name = name; }

std::string Client::getName() const { return _name; }

void Client::setState(ClientState state) { _state = state; }

ClientState Client::getState() const { return _state; }