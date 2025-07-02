#include "../data.hpp"

void SEND(Client* client, Command command)
{
    Data& data = Data::getInstance();

    if (command.args.empty()) {
        client->appendToSendBuffer("461 WHOIS :Not enough parameters\r\n");
        data.enablePollOutIfNeeded(client);
        return;
    }

    std::cout << "test" <<std::endl;
}
