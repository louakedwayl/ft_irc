#ifndef COMMAND_HPP
#define COMMAND_HPP

#include <string>
#include <vector>

struct Command 
{
    std::string name;
    std::vector<std::string> args;

    Command();                         // Constructeur par défaut
    Command(const std::string& line);  // Constructeur qui parse une ligne IRC
};

#endif