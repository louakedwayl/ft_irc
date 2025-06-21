static bool	is_int(std::string const & str)
{
	for(std::string::const_iterator it = str.begin(); it != str.end(); ++it)
	{
		if(!std::isdigit(*it))
			return (false);
	}
	return (true);
}

static bool	is_syntax_valid(std::string const & str_mode, std::string const & str_arg)
{
	if (str_mode.length() != 2) // toujours un + ou un - en préfixe
		return (false);

	if (str_mode[0] != '+' && str_mode[0] != '-')
		return (false);
	
	if (str_mode[1] != 'i' && str_mode[1] != 't' && str_mode[1] != 'k' &&
		str_mode[1] != 'o' && str_mode[1] != 'l')
		return (false);

    // Le mode 'o' (statut d'opérateur) requiert TOUJOURS un argument, que ce soit pour
    // l'ajouter (+o) ou le retirer (-o). L'argument est le pseudonyme de l'utilisateur concerné.
    // Si l'argument est vide, la commande est invalide.
    if (str_mode[1] == 'o' && str_arg.empty())
        return (false);
    
    // --- Traitement des cas où un mode est AJOUTÉ (+) ---
    if (str_mode[0] == '+')
    {
        // Pour DÉFINIR une clé/mot de passe sur un canal (+k), un argument (la clé) est obligatoire.
        if (str_mode[1] == 'k' && str_arg.empty())
            return (false);

        // Pour DÉFINIR une limite d'utilisateurs (+l), un argument est obligatoire ET il doit être un nombre entier.
        if (str_mode[1] == 'l' && (str_arg.empty() || is_int(str_arg)) == false)
            return (false);
    }
    // --- Traitement des cas où un mode est RETIRÉ (-) ---
    else if (str_mode[0] == '-')
    {
        // Pour RETIRER la clé d'un canal (-k), aucun argument ne doit être fourni.
        // Le simple fait d'envoyer "MODE #canal -k" suffit.
        if (str_mode[1] == 'k' && str_arg.empty() == false)
            return (false);
            
        // Pour RETIRER la limite d'utilisateurs (-l), aucun argument ne doit être fourni.
        if (str_mode[1] == 'l' && str_arg.empty() == false)
            return (false);
    }

	return (true);
}

static bool is_valid_mode(std::string const & str_mode, std::string const & str_arg/*, ...*/)
{
	// si la Channel n'existe pas, erreur
	
	// si la syntaxe n'est pas valide -> erreur

	// si l'user n'est pas dans la Channel -> erreur

	// si l'user n'est pas un operator -> erreur

	// sinon, on est bueno
}

// est-ce que la Channel est invite-only ou non
static void	update_channel_invite_only()
{
	if (sign == '+')
	{
		// passer channel en invite only, et l'afficher aux membres
	}
	else if (sign == '-')
	{
		// passer channel en not invite only, et l'afficher aux membres
	}
}

// est-ce que TOPIC est restreint aux operators ou non
static void	update_topic_restricted_to_operators()
{
	if (sign == '+')
	{
		// passer topic de channel en restricted to operators, et l'afficher aux membres
	}
	else if (sign == '-')
	{
		// passer topic de channel en not restricted to operators, et l'afficher aux membres
	}
}

// est-ce qu'il faut une clé pour rejoindre la Channel ou non
static void	update_channel_key(std::string const & str_arg)
{
	if (sign == '+')
	{
		// passer Channel en password protected, mettre à jour le password et l'afficher aux membres
	}
	else if (sign == '-')
	{
		// passer Channel en not password protected, mettre à jour le password (str nulle) et l'afficher aux membres
	}
}

// ajouter ou retirer le privilège d'operator
static void	update_channel_operator()
{
	// si user existe pas, erreur

	// si user est pas dans la Channel, erreur
	
	if (sign == '+')
	{
		// on ajoute le user comme operator et on l'affiche au membres
	}
	else if (sign == '-')
	{
		// si pas un operator, erreur

		// sinon, on supprime le user des operator et on l'affiche au membres
	}
}

// pour ajouter ou retirer une user limit à la Channel
static void	update_user_limit( std::string const & str_arg)
{
	if (sign == '+')
	{
		// on update la user limit et on l'affiche aux membres
	}
	else if (sign == '-')
	{
		// on supprime la user limit et on l'affiche aux membres
	}
}

static void	update_mode()
{
	if (mode == 'i')
		update_channel_invite_only();
	else if (mode == 't')
		update_topic_restricted_to_operators();
	else if (mode == 'k')
		update_channel_key();
	else if (mode == 'o')
		update_channel_operator();
	else if (mode == 'l')
		update_user_limit();
}

void	MODE()
{	
	// si utilisateur est pas connecté, erreur

	// si extraction vers stringstream échoue, ou que Channel ou mode est vide, erreur

	if (is_valid_mode() == true)
		update_mode();
}
