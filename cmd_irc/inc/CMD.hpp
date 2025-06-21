#pragma once

typedef void (*t_ft_cmd)(
	// parameters
);

struct cmd_enum
{
	const char*	cmd_name;
	t_ft_cmd 	cmd_func;
};

void	INVITE(
	// parameters
);

void 	JOIN(
	// parameters
);

void	KICK(
	// parameters
);

void	MODE(
	// parameters
);

void 	MSG(
	// parameters
);

void 	NICK(
	// parameters
);

void	PASS(
	// parameters
);

void	TOPIC(
	// parameters
);

void	USER(
	// parameters
);

const cmd_enum g_cmd_enum[] = {
	{"INVITE", INVITE},
	{"JOIN", JOIN},
	{"KICK", KICK},
	{"MODE", MODE},
	{"MSG", MSG},
	{"NICK", NICK},
	{"PASS", PASS},
	{"TOPIC", TOPIC},
	{"USER", USER},
	{NULL, NULL}};

class CMD
{
	private:
		// one attribute per parameter in the functions

		void	execute();

	public:
		CMD(const State & state);
		~CMD();
};
