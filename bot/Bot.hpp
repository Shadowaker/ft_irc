#ifndef BOT_HPP
# define BOT_HPP

# include <string>
# include <iostream>
# include <netdb.h>
# include <stdio.h>
# include <stdlib.h>
# include <string.h>
# include <sys/socket.h>
# include <arpa/inet.h>
# include <unistd.h>


class Bot
{
	private:
		std::string		_host;
		std::string		_port;
		std::string		_pass;

		std::string		_username;
		std::string		_nickname;
		bool			_operator;
		int				_fd;

		bool			_isclosed;

	public:
		Bot(std::string username, std::string nickname, std::string host, std::string port, std::string pass);
		~Bot();

	void	run();
	void	handler(std::string request);

	std::string	listen();
};

#endif
