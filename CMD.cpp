#include "CMD.hpp"

Command::Command(const std::string& line)
{
    std::istringstream iss(line);
    std::string token;

    // Lire la commande (ex: PRIVMSG, JOIN, etc.)
    iss >> token;
    name = token;

    bool hasTrailing = false;
    std::string trailingArg;

    while (iss >> token)
    {
        if (!hasTrailing && token[0] == ':')
        {
            hasTrailing = true;
            trailingArg = token.substr(1); // Enlève le ':'

            std::string word;
            while (iss >> word)
                trailingArg += " " + word;

            args.push_back(trailingArg);
            break;
        }
        else
        {
            args.push_back(token);
        }
    }
}
