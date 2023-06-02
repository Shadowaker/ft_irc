#ifndef COMMAND_HPP
# define COMMAND_HPP

# include <string>
# include "User.hpp"
# include "IRCprotocol.h"
# include "IRCServer.hpp"
//# include "Utils.hpp"

class Command
{
	private:
		std::string _name;

	protected:
		Command(std::string name) : _name(name) {};

	public:
		virtual void execute(User *user, IRCServer *server, std::string request) = 0;
		std::string getName() {
			return this->_name;
		};
		virtual ~Command() {};
};

/* Auth client to join server.*/
class PassCommand : public Command
{
	public:
		PassCommand() : Command(CMD_PASS) {};
		virtual void execute(User *user, IRCServer *server, std::string request) {
			if (user->getStatus() != PASSWORD) {
				user->sendMessage(":" SERVER_NAME " 462 * :You may not reregister");
				return ;
			}

			// KVIrc does weird things so we adapt
			if (request[0] == ':') {
				request = request.substr(1, request.length());
			}

			if (server->getPassword() == request) {
				user->setStatus(NICKNAME);
			} else {
				user->sendMessage(":" SERVER_NAME " 464 * :Password incorrect");
			}
		};
};

/* Can be used to update nickname, returns 'NICK <newnickname>' to the client as success.*/
class NickCommand : public Command
{
	public:
		NickCommand() : Command(CMD_NICK) {};
		virtual void execute(User *user, IRCServer *server, std::string request)
		{
			if (user->getStatus() != LOGGED && user->getStatus() != NICKNAME)
			{
				user->sendMessage(std::string(":" SERVER_NAME " 451 * :You must authenticate first"));
				return ;
			}

			if (request.find(" ") != std::string::npos || request.find("#") != std::string::npos || request.find("&") != std::string::npos || request.length() > 9)
			{
				user->sendMessage(std::string(":" SERVER_NAME " 432 *").append(request).append(" :Erroneus nickname"));
				return ;
			}

			std::vector<User *> users = server->getUsers();
			size_t				size = users.size();
			for (size_t i = 0; i < size; i++) {
				if (users[i]->getNickname() == request) {
					user->sendMessage(std::string(":" SERVER_NAME " 433 *").append(request).append(" :Nickname is already in use"));
					return ;
				}
			}

			if (user->getStatus() != LOGGED) {
				user->setStatus(USERNAME);
				user->sendMessage(std::string(":").append(user->getNickname()).append("!").append(user->getUsername()).append("@127.0.0.1 NICK ").append(request));
			} else {
				server->broadcastMessage(std::string(":").append(user->getNickname()).append("!").append(user->getUsername()).append("@127.0.0.1 NICK ").append(request), NULL);
			}
			user->setNickname(request);
		};
};

/* Set username and real name of client.*/
class UserCommand : public Command
{
	public:
		UserCommand() : Command(CMD_USER) {};
		virtual void execute(User *user, IRCServer *server, std::string request) {
			if (user->getStatus() != USERNAME) {
				if (user->getStatus() != LOGGED)
					user->sendMessage(std::string(":" SERVER_NAME " 451 * :You must authenticate first"));
				else
					user->sendMessage(":" SERVER_NAME " 462 * :You may not reregister");
				return ;
			}

			size_t usernameEnd = request.find(" ");
			if (usernameEnd == std::string::npos) {
				usernameEnd = request.size();
			}

			std::string username = request.substr(0, usernameEnd);
			if (username.find(" ") != std::string::npos || username.find("#") != std::string::npos || username.find("&") != std::string::npos || username.length() > 9) {
				user->sendMessage(":" SERVER_NAME " 461 * :Not enough parameters");
				return ;
			}

			user->setUsername(username);
			user->setStatus(LOGGED);
			user->sendMessage(std::string(":127.0.0.1:4242 001 ").append(user->getNickname()).append(" :Welcome to the IRC network ").append(user->getNickname()).append("!").append(user->getUsername()).append("@").append("127.0.0.1"));
		(void) server;
		};
};

/* Join a new channel, if not exist create it and the user is operator of that channel.*/
class JoinCommand : public Command
{
	public:
		JoinCommand() : Command(CMD_JOIN) {};
		virtual void execute(User *user, IRCServer *server, std::string request) {
			if (!user->isLogged()) {
				user->sendMessage(std::string(":" SERVER_NAME " 451 ").append(user->getNickname()).append(" " CMD_JOIN " :Not registered"));
				return ;
			}

			if (request[0] != '#') {
				user->sendMessage(std::string(":" SERVER_NAME " 443 ").append(user->getNickname()).append(" " CMD_JOIN " :Channel name should start with # or &"));
				return ;
			}

			if (request.empty() || request.length() < 2) {
				user->sendMessage(std::string(":" SERVER_NAME " 461 ").append(user->getNickname()).append(" " CMD_JOIN " :Not enough parameters"));
				return ;
			}
			std::string	name = request.substr(1, request.length());
			std::map<std::string, Channel *> channels = server->getChannels();
			if (channels.find(name) == channels.end())
			{
				server->channelCreate(name);
				channels = server->getChannels();
			}

			try {
				channels[name]->addUser(user);
			}
			catch (Channel::ChannelException &e) {
				user->sendMessage(std::string(":" SERVER_NAME " 443 ").append(user->getNickname()).append(" " CMD_JOIN " :User is already on channel"));
			}
		};
};

/* Client disconnect from server.*/
class QuitCommand : public Command
{
	public:
		QuitCommand() : Command(CMD_QUIT) {};
		virtual void execute(User *user, IRCServer *server, std::string request) {
			std::string reason = request;
			if (reason.empty()) {
				reason = "Disconnected";
			}
			
			if (user->isLogged()) {
				std::map<std::string, Channel *>	channels = server->getChannels();
				for (std::map<std::string, Channel *>::iterator i = channels.begin(); i != channels.end(); i++) {
					if ((*i).second->isInChannel(user)) {
						(*i).second->partUser(user, "is leaving the party.");
					}
				}
				server->broadcastMessage(std::string(":").append(user->getNickname()).append("!").append(user->getUsername()).append("@127.0.0.1 QUIT :Quit: ").append(reason), user);
				user->sendMessage(std::string(":" SERVER_NAME " ERROR :Closing Link:").append(request));
			} else {
				user->sendMessage(std::string(":").append(user->getNickname()).append("!").append(user->getUsername()).append("@127.0.0.1 QUIT :Quit: ").append(reason));
			}
			user->setStatus(TO_CLOSE);
		};
};

/* Try to authenticate as Server Operator.*/
class OperCommand : public Command
{
	public:
		OperCommand() : Command(CMD_OPER) {};
		virtual void execute(User *user, IRCServer *server, std::string request) {
			if (!user->isLogged()) {
				user->sendMessage(std::string(":" SERVER_NAME " 451 * " CMD_OPER " :You have not registered."));
				return ;
			}

			size_t space_pos = request.find(" ");
			if (space_pos == std::string::npos) {
				user->sendMessage(std::string(":" SERVER_NAME " 461 ").append(user->getNickname()).append(" OPER :Not enough parameters"));
				return ;
			}

			std::string name = request.substr(0, space_pos);
			space_pos = request.find(" ", space_pos);
			if (space_pos == std::string::npos) {
				user->sendMessage(std::string(":" SERVER_NAME " 461 ").append(user->getNickname()).append(" OPER :Not enough parameters"));
				return ;
			}

			std::string pass = request.substr(space_pos + 1, request.length());
			if (name != OPER_USERNAME || pass != OPER_PASSWORD) {
				user->sendMessage(":ircserver 464 * :Password incorrect");
			}
			else {
				user->sendMessage(std::string(":" SERVER_NAME " 381 ").append(user->getNickname()).append(" :You are now an IRC operator"));
				user->sendMessage(std::string(":" SERVER_NAME " MODE ").append(user->getNickname()).append(" :+o"));
				user->setOperator(true);
			}
			(void) server;
		}
};

/* PING to respond with PONG <PING arguments>*/
class PingCommand : public Command {
	public:
		PingCommand() : Command(CMD_PING) {};
		virtual void execute(User *user, IRCServer *server, std::string request) {
			if (!user->isLogged()) {
				user->sendMessage(std::string(":" SERVER_NAME " 451 * " CMD_PING " :You have not registered."));
				return ;
			}
			user->sendMessage(std::string(CMD_PONG " ").append(request));
			(void) server;
		};
};

/* Kick a client from the server */
class KillCommand : public Command {
	public:
		KillCommand() : Command(CMD_KILL) {};
		virtual void execute(User *user, IRCServer *server, std::string request) {
			if (!user->isOperator()) {
				user->sendMessage(std::string(":" SERVER_NAME " 481 ").append(user->getNickname()).append(" :Permission Denied- You're not an IRC operator"));
				return ;
			}

			size_t first_space = request.find(" ");
			if (first_space == std::string::npos) {
				user->sendMessage(std::string(":" SERVER_NAME " 461 ").append(user->getNickname()).append(" KILL :Not enough parameters"));
				return ;
			}

			std::string toKick = request.substr(0, first_space);
			first_space = request.find(" ", first_space);
			if (first_space == std::string::npos) {
				user->sendMessage(std::string(":" SERVER_NAME " 461 ").append(user->getNickname()).append(" KILL :Not enough parameters"));
				return ;
			}

			std::string reason = request.substr(first_space, request.length());
			User *toKickUser = server->getUserByNickname(toKick);
			if (toKickUser == NULL) {
				user->sendMessage(std::string(":" SERVER_NAME " 401 ").append(user->getNickname()).append(" ").append(toKick).append(" :No such nick/channel"));
				return ;
			}
			std::map<std::string, Channel *>	channels = server->getChannels();
			for (std::map<std::string, Channel *>::iterator i = channels.begin(); i != channels.end(); i++) {
				if ((*i).second->isInChannel(toKickUser)) {
					(*i).second->partUser(toKickUser, "is leaving the party.");
				}
			}
			server->broadcastMessage(std::string(":").append(user->getNickname()).append(" KILL ").append(toKickUser->getUsername()).append(" :has been disconnected from the server. Reason: ").append(reason), toKickUser);

			toKickUser->sendMessage(std::string(":" SERVER_NAME " KILL ").append(toKickUser->getUsername()).append(" :You have been disconnected from the server. Reason: ").append(reason));
			toKickUser->setStatus(TO_CLOSE);
		}
};

/* Send a message to a client (private) or to a channel (public)*/
class PrivmsgCommand : public Command
{
	public:
		PrivmsgCommand() : Command(CMD_PRIVMSG) {};
		virtual void execute(User *user, IRCServer *server, std::string request) {
			if (!user->isLogged()) {
				user->sendMessage(std::string(":" SERVER_NAME " 451 ").append(user->getNickname()).append(" PRIVMSG :Not registered"));
				return ;
			}

			std::vector<std::string>	tokens;
			std::istringstream 			iss(request);
			std::string 				token;

			while (std::getline(iss, token, ' ')) {
				tokens.push_back(token);
			}

			size_t	size = tokens.size();
			if (size < 2) {
				user->sendMessage(std::string(":" SERVER_NAME " 461 ").append(user->getNickname()).append(" PRIVMSG :Not enough parameters"));
				return ;
			}

			std::string	receiver = tokens[0];
			std::string message = tokens[1].substr(0, tokens[1].length());
			for (size_t i = 2; i < size; i++) {
				message += ' ' + tokens[i];
			}
			std::string text = std::string(":").append(user->getNickname().append(" PRIVMSG ").append(receiver).append(" :").append(message));

			if (receiver[0] != '#') {
				try {
					server->sendMessage(receiver, text);
				}
				catch(const IRCServer::IRCServerException& e) {
					user->sendMessage(std::string(":" SERVER_NAME " 401 ").append(user->getNickname()).append(" ").append(receiver).append(" :No such nick"));
				}
			}
			else {
				try {
					server->getChannels()[receiver.substr(1, receiver.length())]->sendAllMessage(text, user);
				} catch (const std::exception &e) {
					user->sendMessage(std::string(":" SERVER_NAME " 401 ").append(user->getNickname()).append(" ").append(receiver).append(" :No such channel"));
				}
			}
		}
};

/* Exactly like PRIVMSG but different scope*/
class NoticeCommand : public Command
{
	public:
		NoticeCommand() : Command(CMD_NOTICE) {};
		virtual void execute(User *user, IRCServer *server, std::string request) {
			if (!user->isLogged()) {
				user->sendMessage(std::string(":" SERVER_NAME " 451 ").append(user->getNickname()).append(" NOTICE :Not registered"));
				return ;
			}

			std::vector<std::string>	tokens;
			std::istringstream 			iss(request);
			std::string 				token;

			while (std::getline(iss, token, ' ')) {
				tokens.push_back(token);
			}

			size_t	size = tokens.size();
			if (size < 2) {
				user->sendMessage(std::string(":" SERVER_NAME " 461 ").append(user->getNickname()).append(" NOTICE :Not enough parameters"));
				return ;
			}

			std::string	receiver = tokens[0];
			std::string message = tokens[1].substr(1, tokens[1].length());
			for (size_t i = 2; i < size; i++) {
				message += ' ' + tokens[i];
			}
			std::string text = std::string(":").append(user->getNickname().append(" NOTICE ").append(receiver).append(" :").append(message));

			if (receiver[0] != '#') {
				try {
					server->sendMessage(receiver, text);
				} catch(const IRCServer::IRCServerException& e) {
					user->sendMessage(std::string(":" SERVER_NAME " 401 ").append(user->getNickname()).append(" ").append(receiver).append(" :No such nick"));
				}
			} else {
				try {
					server->getChannels()[receiver.substr(1, receiver.length())]->sendAllMessage(text, NULL);
				} catch (const std::exception &e) {
					user->sendMessage(std::string(":" SERVER_NAME " 401 ").append(user->getNickname()).append(" ").append(receiver).append(" :No such channel"));
				}
			}
		}
};

/* Send the list of channels and users in the server */
class ListCommand : public Command {
	public:
		ListCommand() : Command(CMD_LIST) {};
		virtual void execute(User *user, IRCServer *server, std::string request) {
			user->sendMessage(std::string(":" SERVER_NAME " 321 ").append(user->getNickname()).append(" Channel"));
			std::map<std::string, Channel *> channels = server->getChannels();
			std::map<std::string, Channel *>::iterator end = channels.end();
			for (std::map<std::string, Channel *>::iterator x = channels.begin(); x != end; x++) {
				user->sendMessage(std::string(":" SERVER_NAME " 322 ").append(user->getNickname()).append(" #").append(x->second->getName()).append(" ").append(toString(x->second->getUsers().size())).append(" :").append(x->second->getTopic()));
			}
			user->sendMessage(std::string(":" SERVER_NAME " 323 ").append(user->getNickname()));
		(void) server;
		(void) request;
		}
};

/* Send info about a channel or a client */
class WhoCommand : public Command {
	public:
		WhoCommand() : Command(CMD_WHO) {};
		virtual void execute(User *user, IRCServer *server, std::string request) {
			(void) server;
			std::map<std::string, Channel *> channels = server->getChannels();
			if (channels.find(request) == channels.end()) {
				return ;
			}

			Channel *channel = channels[request];
			std::vector<User *> users = channel->getUsers();
			for (std::vector<User *>::iterator i = users.begin(); i != users.end(); i++) {
				user->sendMessage(std::string(":" SERVER_NAME " 352 ").append(user->getNickname()).append(" ").append(" #").append(channel->getName()).append(" ").append((*i)->getUsername()).append(" 127.0.0.1 " SERVER_NAME " ").append((*i)->getNickname()).append(" 0 0 ").append((*i)->getUsername()));
			}
			user->sendMessage(std::string(":" SERVER_NAME " 315 ").append(user->getNickname()).append(" #").append(channel->getName()).append(" :End of /WHO list"));
		}
};

/* Leave a channel */
class PartCommand : public Command {
	public:
		PartCommand() : Command(CMD_PART) {};
		virtual void execute(User *user, IRCServer *server, std::string request) {
			if (!user->isLogged()) {
				user->sendMessage(std::string(":" SERVER_NAME " 451 ").append(user->getNickname()).append(" " CMD_PART " :Not registered"));
				return ;
			}

			if (request[0] != '#') {
				user->sendMessage(std::string(":" SERVER_NAME " 443 ").append(user->getNickname()).append(" " CMD_PART " :Channel name should start with # or &"));
				return ;
			}

			size_t nameSplit = request.find(':');
			if (request.empty() || request.length() < 2 || nameSplit == std::string::npos) {
				user->sendMessage(std::string(":" SERVER_NAME " 461 ").append(user->getNickname()).append(" " CMD_PART " :Not enough parameters"));
				return ;
			}

			std::string	name = request.substr(1, nameSplit - 2);
			std::map<std::string, Channel *> channels = server->getChannels();
			if (channels.find(name) == channels.end()) {
				user->sendMessage(std::string(":" SERVER_NAME " 403 ").append(user->getNickname()).append(" " CMD_PART " :No such channel"));
				return ;
			}

			Channel *channel = channels[name];
			std::vector<User *> users = channel->getUsers();
			bool found = false;
			for (std::vector<User *>::iterator i = users.begin(); i != users.end(); i++) {
				if ((*i) == user) {
					found = true;
				}
			}

			if (!found) {
				user->sendMessage(std::string(":" SERVER_NAME " 442 ").append(user->getNickname()).append(" " CMD_PART " :You're not on that channel"));
				return ;
			}

			channel->partUser(user, "Leaving");
		}
};

#endif
