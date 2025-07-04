#include "data.hpp"

int main(int argc, char **argv)
{
    handle_signal();

    parse(argc, argv);
    Data& data = Data::getInstance();
    std::cout << "[Server] Starting IRC server..." << std::endl;
    if (make_server())
        return (EXIT_FAILURE);
    data.addPollFd(data.getServerSocket());
    std::cout << "[Server] Set up poll fd array\n" << std::endl;
    ft_irc();
}
