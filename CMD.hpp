#pragma once

#include <string>
#include <vector>
#include <sstream>

struct Command 
{
    std::string name;
    std::vector<std::string> args;

    Command(const std::string& line);  // Constructeur qui parse une ligne IRC
};
