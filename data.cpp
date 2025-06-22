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

void Data::setPassword(const std::string& pass) 
{
        _password = pass ;
}

bool Data::checkPassword(const std::string& attempt) const 
{
        if (attempt ==_password)
                return true;
        else 
                return false;
}

Data::Data(){}

Data::~Data(){}
