#define LOGGER_NAME "CommandRouter"

#include "CommandRouter.hpp"
#include "SimpleLogs.hpp"

CommandRouter::CommandRouter()
{
	addCommand(new PassCommand());
	addCommand(new NickCommand());
	addCommand(new UserCommand());
	addCommand(new QuitCommand());
	addCommand(new KillCommand());
	addCommand(new JoinCommand());
	addCommand(new OperCommand());
	addCommand(new ListCommand());
	addCommand(new PingCommand());
	addCommand(new PrivmsgCommand());
	addCommand(new WhoCommand());
	addCommand(new NoticeCommand());
	addCommand(new PartCommand());
}

CommandRouter::~CommandRouter()
{
	for (std::map<std::string, Command *>::iterator x = commands.begin(); x != commands.end(); x++)
		delete x->second;
}

void CommandRouter::addCommand(Command *command)
{
	this->commands[command->getName()] = command;
}

bool CommandRouter::route(User *user, IRCServer *server, std::string request)
{
	DEBUG("Routing command: $" << request << "$")
	size_t first_space = request.find(" ");
	std::string cmd = first_space == std::string::npos ? request : request.substr(0, first_space);
	size_t	size = cmd.length();

	for (size_t i = 0; i < size; i++) {
		cmd[i] = toupper(cmd[i]);
	}
	if (this->commands.find(cmd) == this->commands.end())
		return false;

	this->commands[cmd]->execute(user, server, first_space == std::string::npos ? "" : request.substr(first_space + 1, request.length()));
	return true;
}
