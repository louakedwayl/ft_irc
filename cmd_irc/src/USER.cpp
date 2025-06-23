void	USER(std::string const & arg)
{
	User & user = ;// GET USER
	std::stringstream ss(arg);
	std::string username;
	std::getline(ss, username, ' ');

	if (username.empty())
	{
		// write error to Client;
		return ;
	}

	user.set_username(username);

	if (user.get_is_registered() == false && user.get_nickname().empty() == false)
	{
		if (user.get_password() == SERVER.password)
		{
			// write welcome to Client
			user.set_is_registered(true);	
		}
		else
		{
			// write error to Client
			// disconnect the Client
		}
	}
}
