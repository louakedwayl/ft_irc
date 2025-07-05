#include "../data.hpp"

bool	does_nickname_have_channel_prefix(std::string const & nickname)
{
	if (nickname[0] == '#' || nickname[0] == '&' || nickname[0] == '~' ||
		nickname[0] == '@' || nickname[0] == '%' || nickname[0] == ':')
			return (true);
	if (nickname[0] == '+' && nickname[1])
	{
		if ( // norme IRC
			nickname[1] == 'q' || nickname[1] == 'a' || nickname[1] == 'o' ||
			nickname[1] == 'h' || nickname[1] == 'v')
			return (true);
	}
	return (false);
}

void NICK(Client *client, Command command)
{
    Data &data = Data::getInstance();

    if (client->getState() == CONNECTING || client->getState() == TO_DISCONNECT)
    {
        std::cerr << "[Server] NICK: client " << client->getFd() << ": Password not sent. Can't register nickname." << std::endl;
        client->appendToSendBuffer(":irc.slytherin.com 464 * :You need to send a password before registering\r\n");
        client->markForDisconnect();

        for (size_t i = 0; i < data.getPollFds().size(); ++i)
        {
            if (data.getPollFds()[i].fd == client->getFd())
            {
                data.getPollFds()[i].events |= POLLOUT;
                break;
            }
        }
        client->setState(TO_DISCONNECT);
        return;
    }

    if (command.args.empty())
    {
        std::cerr << "[Server] NICK: client " << client->getFd() << " no argument." << std::endl;
        client->appendToSendBuffer(":irc.slytherin.com 431 * :No nickname given\r\n");
    }
    else if (does_nickname_have_channel_prefix(command.args[0]))
    {
        std::cerr << "[Server] NICK: client " << client->getFd() << " nickname cannot start by a channel prefix." << std::endl;
        client->appendToSendBuffer(":irc.slytherin.com 432 * " + command.args[0] + " :Erroneous nickname\r\n");
    }
    else if (data.nickNameIsAvailable(command.args[0]))
    {
        client->setNickName(command.args[0]);
        client->setState(SENT_NICK);
        std::cout << "[Server] Nickname assigned: " << command.args[0] << " (fd: " << client->getFd() << ")" << std::endl;
    }
    else
    {
        std::cerr << "[Server] NICK: client " << client->getFd() << " Nickname already in use: " << command.args[0] << std::endl;
        client->appendToSendBuffer(":irc.slytherin.com 433 * " + command.args[0] + " :Nickname is already in use\r\n");
    }

    for (size_t i = 0; i < data.getPollFds().size(); ++i)
    {
        if (data.getPollFds()[i].fd == client->getFd())
        {
            data.getPollFds()[i].events |= POLLOUT;
            break;
        }
    }

    if (client->isFullyRegistered() && client->get_registered() == 0)
    {
        client->set_registered();
        client->setState(REGISTERED);
        send001(client);
        send002(client);
        send003(client);
        send004(client);
    }
}
