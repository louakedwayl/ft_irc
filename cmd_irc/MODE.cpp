#include "../data.hpp"

void MODE(Client* client, Command command)
{
    Data& data = Data::getInstance();

    if (command.args.empty()) {
        client->appendToSendBuffer("461 MODE :Not enough parameters\r\n");
        data.enablePollOutIfNeeded(client);
        return;
    }

    std::string target = command.args[0];

    // ---------- MODE <#channel> ----------
    if (!target.empty() && target[0] == '#') 
    {
        Channel* channel = data.getThisChannel(target);
        if (!channel) 
        {
            client->appendToSendBuffer("403 " + target + " :No such channel\r\n");
            data.enablePollOutIfNeeded(client);
            return;
        }

        // Si aucun mode donné → afficher les modes actuels
        if (command.args.size() == 1) 
        {
            std::string modeString = "+";
            if (channel->getIsInviteOnly())       modeString += "i";
            if (channel->getIsTopicRestricted())  modeString += "t";
            if (channel->getUsersLimit() != -1)       modeString += "l";
            if (!channel->getChannelKey().empty())             modeString += "k";

            client->appendToSendBuffer("324 " + client->getNickName() + " " + channel->getName() + " " + modeString + "\r\n");
            data.enablePollOutIfNeeded(client);
            return;
        }

        // Vérifie si le client est opérateur
        if (!channel->isOperator(client)) 
        {
            client->appendToSendBuffer("482 " + target + " :You're not channel operator\r\n");
            data.enablePollOutIfNeeded(client);
            return;
        }

        std::string modeFlags = command.args[1];
        bool addMode = true;
        unsigned int argIndex = 2;

        for (unsigned int i = 0; i < modeFlags.size(); ++i) 
        {
            char mode = modeFlags[i];
            if (mode == '+') 
            {
                addMode = true;
            }
            else if (mode == '-') 
            {
                addMode = false;
            }
            else {
                if (mode == 'i') 
                {
                    channel->setIsInviteOnly(addMode);
                }
                else if (mode == 't')
                {
                    channel->setIsTopicRestricted(addMode);
                }
                else if (mode == 'l') 
                {
                    if (addMode) {
                        if (argIndex >= command.args.size())
                            break;
                        int limit = std::atoi(command.args[argIndex].c_str());
                        ++argIndex;
                        channel->setUsersLimit(limit);
                    }
                        else {
                                // Il faut gérer la désactivation de la limite d’utilisateurs ici
                                channel->setUsersLimit(-1); // ou ta valeur spéciale pour "pas de limite"
                            }
                }
                else if (mode == 'k') 
                {
                    if (addMode) {
                        if (argIndex >= command.args.size())
                            break;
                        std::string key = command.args[argIndex];
                        ++argIndex;
                        channel->setChannelKey(key);
                    }
                    else {
                            channel->setChannelKey(""); // supprimer la clé quand on désactive
                        }
                }
                else if (mode == 'o') 
                {
                    if (argIndex >= command.args.size())
                        break;
                    std::string targetNick = command.args[argIndex];
                    ++argIndex;
                    Client* targetClient = data.getClientByNickname(targetNick);
                    if (targetClient != NULL) 
                    {
                        if (addMode)
                            channel->addOperator(targetClient);
                        else
                            channel->removeOperator(targetClient);
                    }
                }
                else 
                {
                    client->appendToSendBuffer("472 " + std::string(1, mode) + " :is unknown mode char\r\n");
                }
            }
        }

        // Construction du message MODE à envoyer à tous les clients du channel
        std::string modeLine = ":" + client->getPrefix() + " MODE " + channel->getName() + " " + modeFlags;
        for (std::vector<std::string>::size_type i = 2; i < command.args.size(); ++i) 
        {
            modeLine += " " + command.args[i];
        }
        modeLine += "\r\n";
        channel->broadcastMessage(NULL ,modeLine);
        return;
    }
}