#include "../data.hpp"

void send001(Client* client)
{
    std::string nick;
    if (client->getNickName().empty())
        nick = "unknown";
    else
        nick = client->getNickName();

    std::string user;
    if (client->getUserName().empty())
        user = "unknown";
    else
        user = client->getUserName();

    std::string host;
    if (client->getHostName().empty())
        host = "localhost";
    else
        host = client->getHostName();

    std::string msg = ":irc.slytherin.com 001 " + nick +
        " :Welcome to the Internet Relay Network " +
        nick + "!" + user + "@" + host + "\r\n";

    client->appendToSendBuffer(msg);
}



void send002(Client* client)
{
    std::string nick;
    if (client->getNickName().empty())
        nick = "unknown";
    else
        nick = client->getNickName();

    std::string msg = ":irc.slytherin.com 002 " + nick +
        " :Your host is irc.slytherin.com, running version 1.0\r\n";

    client->appendToSendBuffer(msg);
}

void send003(Client* client)
{
    std::string nick;
    if (client->getNickName().empty())
        nick = "unknown";
    else
        nick = client->getNickName();

    std::string msg = ":irc.slytherin.com 003 " + nick +
        " :This server was created 25/06/2025\r\n";

    client->appendToSendBuffer(msg);
}


void send004(Client* client)
{
    std::string nick;
    if (client->getNickName().empty())
        nick = "unknown";
    else
        nick = client->getNickName();

    // Format : :server 004 <nick> <servername> <version> <user modes> <channel modes>
    std::string msg = ":irc.slytherin.com 004 " + nick +
        " irc.slytherin.com 1.0 ao mtov\r\n";

    client->appendToSendBuffer(msg);
}