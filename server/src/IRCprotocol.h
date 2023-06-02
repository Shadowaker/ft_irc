#ifndef IRCPROTOCOL_H
# define IRCPROTOCOL_H


# define OPER_USERNAME "admin"
# define OPER_PASSWORD "admin"

// COMMANDS -------------------------------------------------

# define CMD_PASS "PASS"
# define CMD_USER "USER"
# define CMD_NICK "NICK"
# define CMD_QUIT "QUIT"	// https://www.rfc-editor.org/rfc/rfc1459#section-4.1.6
# define CMD_PING "PING"
# define CMD_PONG "PONG"
# define CMD_JOIN "JOIN"
# define CMD_OPER "OPER"	// https://www.rfc-editor.org/rfc/rfc1459#section-4.1.5
# define CMD_KILL "KILL"
# define CMD_PRIVMSG "PRIVMSG"
# define CMD_LIST "LIST"
# define CMD_WHO "WHO"
# define CMD_DCC "DCC"
# define CMD_PART "PART"
# define CMD_NOTICE "NOTICE"

// SOURCE: https://www.alien.net.au/irc/irc2numerics.html

# define REPL_WELCOME "001"
# define REPL_YOURHOST "002"
# define REPL_CREATED "003"
# define RPL_TRYAGAIN "263"
# define RPL_AWAY "301"
# define RPL_UNAWAY "305"
# define RPL_NOAWAY "306"
# define RPL_WHOISUSER "311"
# define RPL_WHOISSERVER "312"
# define RPL_WHOISOPERATOR "313"
# define RPL_WHOWASUSER "314"
# define RPL_ENDOFWHO "315"
# define RPL_TOPIC "332"
# define RPL_WHOREPLY "352"
# define RPL_USERSSTART "392"
# define RPL_USERS "393"
# define RPL_ENDOFUSERS "394"
# define RPL_NOUSERS "395"
# define ERR_UNKNOWNERROR "400"
# define ERR_NOSUCHNICK "401"
# define ERR_NOSUCHSERVER "402"
# define ERR_NOSUCHCHANNEL "403"
# define ERR_CANNOTSENDTOCHAN "404"
# define ERR_TOOMANYCHANNELS "405"
# define ERR_NOORIGIN "409"
# define ERR_NOTEXTTOSEND "412"
# define ERR_USERNOTINCHANNEL "441"
# define ERR_NOTONCHANNEL "442"
# define ERR_NOLOGIN "444"
# define ERR_PASSWDMISMATCH "464"
# define ERR_YOUREBANNEDCREEP "465"
# define ERR_CHANNELISFULL "471"
# define ERR_NOPRIVILEGES "481"

# define ERR_NEEDMOREPARAMS "461"
# define ERR_ALREADYREGISTRED "462"

# define ERR_NONICKNAMEGIVEN "431"
# define ERR_ERRONEUSNICKNAME "432"
# define ERR_NICKNAMEINUSE "433"
# define ERR_UNAVAILRESOURCE "437"
# define ERR_NICKCOLLISION "436"
# define ERR_RESTRICTED "484"

#endif
