# ft_irc

L’objectif de ce projet est de reproduire le fonctionnement d’un serveur IRC. Vous utiliserez un vrai client IRC afin de vous connecter à votre serveur et ainsi de le tester.

# Introduction

Internet Relay Chat est un protocole de communication textuel sur Internet. Il sert
à la communication instantanée principalement sous la forme de discussions en groupe
par l’intermédiaire de canaux de discussion, mais peut aussi être utilisé pour de la com-
munication directe entre deux personnes.
Les clients IRC se connectent à des serveurs IRC afin d’accéder à des canaux. Les
serveurs IRC sont connectés entre eux afin de créer des réseaux.

# Consignes générales

• Votre programme ne doit en aucun cas crash (même si vous êtes à court de mé-
moire) ni s’arrêter de manière inattendue sauf dans le cas d’un comportement
indéfini.
Si cela arrive, votre projet sera considéré non fonctionnel et vous aurez 0.

• Vous devez rendre un Makefile qui compilera vos fichiers sources. Il ne doit pas
relink.

• Votre Makefile doit contenir au minimum les règles suivantes :
$(NAME), all, clean, fclean et re.

• Compilez votre code avec c++ et les flags -Wall -Wextra -Werror

• Vous devez vous conformer à la norme C++ 98. Par conséquent, votre code doit
compiler si vous ajoutez le flag -std=c++98

• Dans votre travail, essayez d’utiliser en priorité des fonctionnalités C++ (par exemple,
préférez <cstring> à <string.h>). Vous pouvez utiliser des fonctions C, mais
faites votre possible pour choisir la version C++ quand vous le pouvez.

• Tout usage de bibliothèque externe ou de l’ensemble Boost est interdit.

# Partie obligatoire

Nom du programme : ircserv

Fichiers de rendu Makefile, *.{h, hpp}, *.cpp, *.tpp, *.ipp,

un fichier de configuration optionnel

Makefile NAME, all, clean, fclean, re

Arguments port : Le port d’écoute

password : Le mot de passe de connexion

Fonctions externes autorisées

Tout ce qui respecte la norme C++ 98.

socket, close, setsockopt, getsockname,
getprotobyname, gethostbyname, getaddrinfo,
freeaddrinfo, bind, connect, listen, accept, htons,
htonl, ntohs, ntohl, inet_addr, inet_ntoa, send,
recv, signal, sigaction, lseek, fstat, fcntl, poll
(ou équivalent)

Votre binaire devra être appelé comme ceci :

./ircserv <port> <password>
• port : Le numéro du port sur lequel votre serveur acceptera les connexions en-
trantes.
• password : Le mot de passe permettant de s’identifier auprès de votre serveur
IRC, et qui devra être fourni par tout client IRC souhaitant s’y connecter.
