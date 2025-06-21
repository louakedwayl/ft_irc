void	JOIN(ChannelManager & channel_manager, UserManager & user_manager, ClientManager & client_manager, const ServerSettings & server_settings, Client & client, std::string const & args)
{
	// on crée une stringstream sur notre string d'arg 

	// on extrait le premier argument (la Channel) de la stringstream

	// si l'extraction de du premier args de stringstream vers channel_name échoue ou que le résultat est vide -> erreur

	// si la Channel est invite-only, erreur
	
	// on extrait le deuxième arg de stringstream (le mot de passe)

	// si la Channel requiert un mot de passe et que l'extraction a échoué, ou que le mot de passe est nul ou faux, erreur

	// si la Channel n'existe pas, on la crée (vérifier syntaxe et mettre créateur comme operator dans fonction de création) et on return

	// on ajoute le Client à la Channel et on affiche ce fait aux autres membres
}
