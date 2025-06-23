static bool	valid_topic_request(std::string channel)
{
	if (/* Channel doesn't exist */)
	{
		// write error to Client
		return (false);
	}

	if (/* User is not in Channel */)
	{
		// write error to Client
		return (false);
	}

	if (/* Channel is restricted to operators AND User is not an operator */)
	{
		// write error to Client
		return (false);
	}

	return (true);
}

void	TOPIC(std::string arg)
{
	std::stringstream 	ss(arg);
	std::string 		channel;
	std::string			topic;

	if (!(ss >> channel >> topic) || channel.empty())
		// write error to client

	if (valid_topic_request(channel) == false)
		return ;

	Channel channel = ; // GET CHANNEL
	User &	user = ; // GET USER

	if (topic.empty() == true) // pas d'args -> user veut seulement voir le Topic
	{
		if (channel.topic.empty() == true) // si il n'y a pas de Topic
			// write to client that there is not Topic
		else
			// write the Topic to the Client
	}
	else // sinon, on change le Topic de la Channel et on l'écrit aux membres de la Channel
	{
		channel.set_channel_topic(topic);

		std::set<int>::iterator it = channel.clients_fd.begin();
		while (it != channel.clients_fd.end())
		{
			Client current_client = ; // GET CLIENT
			// write the new Topic to the Client
			it++;
		}
	}
}
