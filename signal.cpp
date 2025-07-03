#include "data.hpp"

void handleSigterm(int sig) 
{
    Data &data = Data::getInstance();

    data.shutdown();
    (void)sig;
    exit(0);
}

void handle_signal()
{
    signal(SIGINT, SIG_IGN); 
    signal(SIGQUIT, SIG_IGN); 
    signal(SIGPIPE, SIG_IGN);   // Évite crash si send() sur socket fermée
    signal(SIGTSTP, SIG_IGN);   // Empêche Ctrl+Z de suspendre le serveur
    signal(SIGTERM, handleSigterm);
}