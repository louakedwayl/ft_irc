#include "data.hpp"

void parseCommands(Client* client, const char* buffer)
{
    std::istringstream stream(buffer);
    std::string line;

    while (std::getline(stream, line))
    {
        // retirer \r en fin si présent
        if (!line.empty() && line[line.size() - 1] == '\r')
            line.erase(line.size() - 1, 1);

        Command command(line);
        // Ici, appelle ta fonction qui traite chaque commande
        handleCommand(client, line);
    }
}

void handleCommand(Client* client, Command command)
{
    if (command.name == "CAP")
    {
        CAP (client );
    }
    else if (command.name == "PASS")
    {
        PASS (client, command);
    }
    else if (command.name == "NICK")
    {
        NICK (client, command);
    }
    else if (command.name == "LIST")
    {
        LIST (client, command);
    }
    else if (command.name == "USER")
    {
        USER (client, command);
    }
    else if (command.name == "PING")
    {
        PING (client, command);
    }
    else if (command.name == "JOIN")
    {
        JOIN  (client, command);
    }
    else if (command.name == "INVITE")
    {
        INVITE (client, command);
    }
    else if (command.name == "PRIVMSG")
    {
        PRIVMSG(client, command);
    }
    else if (command.name == "KICK")
    {
        KICK(client, command);
    }
    else if (command.name == "PART")
    {
        PART (client, command);
    }
    else if (command.name == "KICK")
    {
        INVITE (client, command);
    }
    else if (command.name == "TOPIC")
    {
        TOPIC (client, command);
    }
    else if (command.name == "MODE")
    {
        MODE (client, command);
    }
    else if (command.name == "WHOIS")
    {
        WHOIS (client, command);
    }
    else if (command.name == "/QUIT_SERV")
    {
        QUIT_SERV(client, command);
    }
    else if (command.name == "SEND")
    {
        SEND(client, command);
    }
    else
    {
        UNKNOWN(client, command);
    }
}