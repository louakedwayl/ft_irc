# Irssi : 
 
### pour se connecter au server
irssi  -c localhost -p 4444 -w mdp walle

### for return request IRC
https://github.com/AK7iwi/ft_irc/blob/main/srcs/main.cpp

### doc irc for return of the request IRC
https://modern.ircdocs.horse/#rplwelcome-001

### server irc random for test
irssi -c irc.oftc.net -p 6697 -n testwayl 

##test
/rawlog open test_yassine.log

## First TCP request

```irc
CAP LS
NICK wlouaked
```

CAP LS
PASS mdp
NICK wlouaked
USER wlouaked wlouaked localhost :Wayl Louaked
