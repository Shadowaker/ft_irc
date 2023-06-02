
#include "Channel.hpp"

Channel::Channel(std::string name) : _name(name), _topic("default topic") {}

Channel::Channel(std::string name, User *creator) : _name(name), _topic("default topic")
{
	this->_ops.push_back(creator);
	this->_users.push_back(creator);
}

Channel::~Channel() {}

void	Channel::addUser(User *user)
{
	size_t		size = this->_users.size();
	for (size_t x = 0; x < size; x++)
	{
		if (this->_users[x]->getId() == user->getId())
			throw AlreadyInChannel();
	}

	if (this->_ops.empty()) {
		this->_ops.push_back(user);
	}

	this->_users.push_back(user);
	this->sendAllMessage(std::string(":").append(user->getNickname()).append("!").append(user->getUsername()).append("@127.0.0.1 JOIN #").append(this->_name), NULL);
	sendUserList(user);
}

void	Channel::addOperator(User *op)
{
	this->_ops.push_back(op);
}

void	Channel::rmUser(User *target)
{
	size_t	size = this->_users.size();

	for (size_t x = 0; x < size; x++)
	{
		if (this->_users[x]->getId() == target->getId())
		{
			this->_users.erase(_users.begin() + x);
			break ;
		}
	}
}

void	Channel::kickUser(User *kicker, User *target, std::string reason)
{
	std::string text = std::string(":").append(kicker->getNickname()).append(" KICK ").append(this->_name).append(" ").append(target->getNickname()).append(" :").append(reason);
	this->sendAllMessage(text, NULL);
	rmUser(target);
}

void	Channel::partUser(User *user, std::string message)
{
	std::string text = std::string(":").append(user->getNickname()).append("!").append(user->getUsername()).append("@127.0.0.1 PART #").append(this->_name).append(" :").append(message);
	this->sendAllMessage(text, NULL);
	this->rmUser(user);
}

bool	Channel::isInChannel(User *user)
{
	size_t	size = this->_users.size();

	for (size_t x = 0; x < size; x++)
	{
		if (this->_users[x]->getId() == user->getId())
			return (true);
	}
	return (false);
}

bool	Channel::isOperator(User *op)
{
	size_t	size = this->_ops.size();

	for (size_t x = 0; x < size; x++)
	{
		if (this->_ops[x]->getId() == op->getId())
			return true;
	}
	return false;
}

std::vector<User *>	Channel::getUsers()
{
	return this->_users;
}

std::vector<User *>	Channel::getOperators()
{
	return this->_ops;
}

void	Channel::sendAllMessage(std::string message, User *except)
{
	size_t		size = this->_users.size();

	for (size_t x = 0; x < size; x++) {
		if (this->_users[x] == except) {
			continue ;
		}
		this->_users[x]->sendMessage(message);
	}
}

std::string	Channel::getName()
{
	return this->_name;
}

void Channel::sendUserList(User *user) {
	std::string sb = std::string(":ircserv 353 ").append(user->getNickname()).append(" = #").append(this->_name).append(" :");
	for (size_t i = 0; i < this->_users.size(); i++) {
		if (this->isOperator(this->_users[i]))
			sb = sb.append("@");
		sb = sb.append(this->_users[i]->getNickname()).append("!").append(this->_users[i]->getUsername()).append("@127.0.0.1").append(" ");
	}
	user->sendMessage(sb);
	user->sendMessage(std::string(":ircserv 366 ").append(user->getNickname()).append(" #").append(this->_name).append(" :End of /NAMES list."));
}

std::string Channel::getTopic() {
	return this->_topic;
}