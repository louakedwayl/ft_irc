#include "../data.hpp"

void QUIT_SERV (Client *client, Command command)
{
    Data &data = Data::getInstance();
    (void)client;
    (void)command;
    data.shutdown();
    exit (0);
}