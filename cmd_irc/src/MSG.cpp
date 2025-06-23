static std::string parse_target(std::string arg)
{
	std::stringstream	ss(arg);
	std::string			target;

	std::getline(ss, target, ' ');

	return (target);
}

static std::string parse_msg(std::string arg)
{
	std::stringstream	ss(arg);
	std::string			message;

	std::getline(ss, message, ':');
	std::getline(ss, message);

	return (message);
}

void	MSG()
{
	// si PRIVMSG (premier char des args est PAS un #)

		// si nickname existe, on envoie le message

		// sinon, erreur

	// sinon, message pour toute la channel

		// si channel existe pas, erreur

		// sinon, si Client est pas dans la Channel, erreur

		// sinon, on envoie le message à toute la Channel
}
