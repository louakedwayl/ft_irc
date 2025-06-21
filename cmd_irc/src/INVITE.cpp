static bool	is_valid_invite()
{
	// if Channel doesn't exist -> error

	// if user doesn't exist -> error

	// if client is not on the channel -> error, can't invite

	// if client is not operator -> error, can't invite

	// if invited user is already on the Channel -> error

	// if Channel is full -> error

	// if here, ok
}

void	INVITE()
{
	std::stringstream 	ss(args);
	std::string			nickname;
	std::string 		channel_name;

	if (!(ss >> nickname >> channel_name) || channel_name.empty() || nickname.empty())
		// error, need more parameters;
	else if (is_valid_invite() == true)
	{
		// envoyer le message d'invitation

		// si l'invité veut rejoindre, l'ajouter et le dire aux autres membres de la Channel 
	}
}
