#ifndef COMMANDROUTER_HPP
# define COMMANDROUTER_HPP

# include <map>
# include <string>
# include "Command.hpp"

class CommandRouter
{
	private:
		std::map<std::string, Command *> commands;
		void addCommand(Command *command);

	public:
		CommandRouter();
		~CommandRouter();

	bool route(User *user, IRCServer *server, std::string request);
};

#endif
