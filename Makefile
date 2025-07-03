NAME = ircserv

CXX = c++
CXXFLAGS = -std=c++98 -Wall -Werror -Wextra -Wshadow $(DEBUG) $(HEADER) 
DEBUG = -g3
HEADER = -I./include

red		= /bin/echo -e "\x1b[31m\#\# $1\x1b[0m"
green		= /bin/echo -e "\x1b[32m\#\# $1\x1b[0m"
yellow		= /bin/echo -e "\x1b[33m\#\# $1\x1b[0m"
blue		= /bin/echo -e "\x1b[34m\#\# $1\x1b[0m"
purple		= /bin/echo -e "\x1b[35m\#\# $1\x1b[0m"

CMD_SRC = $(addprefix cmd_irc/, CAP.cpp INVITE.cpp JOIN.cpp KICK.cpp MODE.cpp WHOIS.cpp PART.cpp \
                      PRIVMSG.cpp NICK.cpp PASS.cpp TOPIC.cpp USER.cpp LIST.cpp \
					  PING.cpp UNKNOWN.cpp QUIT_SERV.cpp answer.cpp SEND.cpp )

SRC = main.cpp data.cpp parse.cpp server.cpp client.cpp channel.cpp CMD.cpp handleCommand.cpp \
	handlePollin.cpp ft_irc.cpp signal.cpp $(CMD_SRC)

OBJ = $(SRC:.cpp=.o)

DEP = $(SRC:.cpp=.d)  # Fichiers de dépendances

# ---------------------------------- RULES ----------------------------------- #

all: $(NAME)

$(NAME) : $(OBJ)
	@$(CXX) $(CXXFLAGS) -o $(NAME) $(OBJ)
	@$(call blue,"✅ $@ build successful!")

%.o: %.cpp
	@mkdir -p $(dir $@)
	@$(CXX) $(CXXFLAGS) -MMD -c $< -o $@
	@$(call blue,"✅ $< compiled!")

clean   :
	@rm -rf $(OBJ) $(DEP)
	@$(call blue,"🗑️ $(NAME) cleaned")

fclean  : clean
	@rm -rf $(OBJ) $(NAME) 
	@$(call blue,"🗑️ $(NAME) fcleaned")

re      : fclean all

.PHONY : all bonus clean fclean re run val fun

-include $(DEP) 

# ---------------------------------------------------------------------------- #

