# ft_irc

L’objectif de ce projet est de reproduire le fonctionnement d’un serveur IRC. Vous utiliserez un vrai client IRC afin de vous connecter à votre serveur et ainsi de le tester.

# Introduction

Internet Relay Chat est un protocole de communication textuel sur Internet. Il sert
à la communication instantanée principalement sous la forme de discussions en groupe
par l’intermédiaire de canaux de discussion, mais peut aussi être utilisé pour de la com-
munication directe entre deux personnes.
Les clients IRC se connectent à des serveurs IRC afin d’accéder à des canaux. Les
serveurs IRC sont connectés entre eux afin de créer des réseaux.

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

  ./ircserv port password

• port : Le numéro du port sur lequel votre serveur acceptera les connexions en-
trantes.

• password : Le mot de passe permettant de s’identifier auprès de votre serveur
IRC, et qui devra être fourni par tout client IRC souhaitant s’y connecter.
