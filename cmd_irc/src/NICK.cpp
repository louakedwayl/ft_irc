static bool	does_nickname_have_channel_prefix(std::string const & nickname)
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

static bool	does_nickname_already_exist(std::string const & nickname)
{
	// iterator on start of clients list
	// while iterator is different to the end of list
		// if nicknameis name, return true
		// incrementation
	// return false
}

static bool	is_nickname_valid(std::string const & nickname)
{
	
	if (nickname.empty())
		// write ERR_NONICKNAMEGIVEN to client;
		return (false);
	}

	std::string	invalid = " ,*?!@.$:";
	if (does_nickname_have_channel_prefix(nickname) == true || nickname.find_first_of(invalid) != std::string::npos)
	{
		// write ERR_ERRONEUSNICKNAME to client;
		return (false);
	}

	if (does_nickname_already_exist(nickname, user_manager) == true)
	{
		// write ERR_NICKNAMEINUSE to client;
		return (false);
	}

	return (true);
}

void	NICK(std::string const & arg)
{
	User & user = ; // GET USER

	if (arg.empty() || is_nickname_valid(arg) == false)
		return ;

	std::string	old_nickname = user.get_nickname();

	user.set_nickname(arg);

	if (user.get_is_registered() == false && user.get_username().empty() == false)
	{
		if (user.get_password() == SERVER.password)
		{
			// write welcome message to client
			user.set_is_registered(true);
		}
		else
		{
			// write ERR_PASSWDMISMATCH to Client
			// disconnect Client
		}
	}
	else // sinon, on écrit que son nickname a changé
		// write NICK_CHANGED to Client
}
