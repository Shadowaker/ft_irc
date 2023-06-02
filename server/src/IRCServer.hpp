#ifndef IRCSERVER_HPP
# define IRCSERVER_HPP

# include <string>
# include <cstring>
# include <cerrno>
# include <sstream>
# include <iostream>
# include <vector>
# include <map>

# include <sys/socket.h>
# include <fcntl.h>
# include <stdio.h>
# include <stdlib.h>
# include <unistd.h>
# include <arpa/inet.h>
# include <sys/poll.h>
# include <limits.h>

# include "Utils.hpp"
# include "IRCprotocol.h"
# include "Channel.hpp"
# include "User.hpp"

# define TIMEOUT 180000
# define SERVER_NAME "ircserv"

class IRCServer
{
	private:
		int					port;
		int					socket_fd;


		struct pollfd		poll_fds[200];
		int					n_poll_fds;
		std::string			request;
		int					client;
		std::string			password;

		std::vector<User *>					users;
		std::map<std::string, std::string>	requests;
		std::map<std::string, Channel *>	channels;

		void	socketInit();
		void	pollInit();
		void	loop();
		void	newConnection();
		void	compressArray();
		bool	handler(int i, char *buffer);

		void	addUser(unsigned int user_id, int fd);
		void	rmUser(unsigned int);

		void	channelDelete(std::string name);
		void	channelsClear();

	public:
		IRCServer(int port, std::string password);
		~IRCServer();

		std::string 						getPassword() const;
		User* 								getUserByNickname(std::string nickname);
		std::vector<User *> 				getUsers();
		std::map<std::string, Channel *>	getChannels();

		void	open();
		void	channelCreate(std::string name);
		void	sendMessage(std::string nickname, std::string message);
		void	broadcastMessage(std::string message, User *except);

	class IRCServerException : public std::exception {
		protected:
			std::string _reason;

		public:
			IRCServerException(std::string reason) : _reason(reason) {};
			virtual ~IRCServerException() throw() {};
			virtual const char *what() const throw() {
				return (_reason.c_str());
			};
	};

	class SocketError : public IRCServerException {

		public:
			SocketError() : IRCServerException("An error occured while openening the socket.") {};
			virtual ~SocketError() throw() {};
			virtual const char *what() const throw() {
				return (_reason.c_str());
			};
	};

	class SocketOptError : public IRCServerException {

		public:
			SocketOptError() : IRCServerException("An error occurred while setting the socket options.") {};
			virtual ~SocketOptError() throw() {};
			virtual const char *what() const throw() {
				return (_reason.c_str());
			};
	};

	class SocketOptErrorAsync : public IRCServerException {

		public:
			SocketOptErrorAsync() : IRCServerException("An error occurred while setting the socket async options.") {};
			virtual ~SocketOptErrorAsync() throw() {};
			virtual const char *what() const throw() {
				return (_reason.c_str());
			};
	};

	class SocketBindError : public IRCServerException {

		public:
			SocketBindError() : IRCServerException("An error occurred while binding a port to the socket.") {};
			virtual ~SocketBindError() throw() {};
			virtual const char *what() const throw() {
				return (_reason.c_str());
			};
	};

	class ListenError : public IRCServerException {

		public:
			ListenError() : IRCServerException("An error occurred while opening the port.") {};
			virtual ~ListenError() throw() {};
			virtual const char *what() const throw() {
				return (_reason.c_str());
			};
	};

	class PollError : public IRCServerException {

		public:
			PollError() : IRCServerException("An error occurred while calling poll.") {};
			virtual ~PollError() throw() {};
			virtual const char *what() const throw() {
				return (_reason.c_str());
			};
	};

	class PollTimeout : public IRCServerException {

		public:
			PollTimeout() : IRCServerException(" Poll timeout.") {};
			virtual ~PollTimeout() throw() {};
			virtual const char *what() const throw() {
				return (_reason.c_str());
			};
	};

	class ChannelExist : public IRCServerException {

		public:
			ChannelExist() : IRCServerException("Channel already exist.") {};
			virtual ~ChannelExist() throw() {};
			virtual const char *what() const throw() {
				return (_reason.c_str());
			};
	};

	class ClientWouldBlock : public IRCServerException {

		public:
			ClientWouldBlock() : IRCServerException("Client would block.") {};
			virtual ~ClientWouldBlock() throw() {};
			virtual const char *what() const throw() {
				return (_reason.c_str());
			};
	};

	class AcceptError : public IRCServerException {

		public:
			AcceptError() : IRCServerException("New client error.") {};
			virtual ~AcceptError() throw() {};
			virtual const char *what() const throw() {
				return (_reason.c_str());
			};
	};

	class UserNotFound : public IRCServerException {

		public:
			UserNotFound() : IRCServerException("User not found.") {};
			virtual ~UserNotFound() throw() {};
			virtual const char *what() const throw() {
				return (_reason.c_str());
			};
	};

	class KillServer : public IRCServerException {
		public:
			KillServer() : IRCServerException("Server is shutting down.") {};
			virtual ~KillServer() throw() {};
			virtual const char *what() const throw() {
				return (_reason.c_str());
			};
	};
};

# include "CommandRouter.hpp"

#endif
