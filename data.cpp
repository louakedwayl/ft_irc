#include "data.hpp"

Data& Data::getInstance() 
{
        static Data instance; // instance unique et locale statique
        return instance;
}