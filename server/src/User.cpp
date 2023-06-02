#define LOGGER_NAME "\033[91mUSER\033[0m"
#include "User.hpp"
#include "SimpleLogs.hpp"

User::User(unsigned int id, int fd) : _id(id), _username(""), _nickname(""), _status(PASSWORD), _operator(false), _fd(fd)
{
	DEBUG("user object " << this->_id << " created.");
}

User::~User() {}

bool	User::isLogged()
{
	if (this->_status == LOGGED)
		return true;
	return false;
}

unsigned int	User::getId()
{
	return this->_id;
}

std::string	User::getUsername()
{
	return this->_username;
}

std::string	User::getNickname()
{
	return this->_nickname;
}

UserStatus	User::getStatus() const
{
	return this->_status;
}

bool	User::isOperator()
{
	return this->_operator;
}

void User::sendMessage(std::string msg) 
{
	DEBUG("Sent to user: "<<this->_nickname<<" $"<<msg.append("\r\n")<<"$")
	send(this->_fd, msg.append("\r\n").c_str(), msg.length() + 1, 0);
}

void User::setUsername(std::string username) 
{
	this->_username = username;
}

void User::setNickname(std::string nickname) 
{
	this->_nickname = nickname;
}

void User::setStatus(UserStatus status) 
{
	this->_status = status;
}

void	User::setOperator(bool status)
{
	this->_operator = status;
}

