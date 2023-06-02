#ifndef CHANNEL_HPP
# define CHANNEL_HPP

# include <string>
# include <iostream>
# include <vector>

# include "User.hpp"

/*
Channel name begins with & or # and must'n have ' ', ascii 7, and ','
*/

class Channel
{
	private:
		std::string			_name;
		std::string			_topic;
		std::vector<User *>	_users;
		std::vector<User *>	_ops;

		void sendUserList(User *user);

	public:
		Channel(std::string name);
		Channel(std::string name, User *creator);
		~Channel();

	void				addUser(User *user);
	void				addOperator(User *op);

	void				kickUser(User *kicker, User *target, std::string reason);
	void				rmUser(User *user);
	void				partUser(User *user, std::string message);
	bool				isInChannel(User *user);
	bool				isOperator(User *op);

	void				sendAllMessage(std::string message, User *except);

	std::vector<User *>	getUsers();
	std::vector<User *>	getOperators();
	std::string			getName();
	std::string			getTopic();

	class ChannelException : public std::exception {
		protected:
			std::string _reason;
		
		public:
			ChannelException(std::string reason) : _reason(reason) {};
			virtual ~ChannelException() throw() {};
			virtual const char *what() const throw() {
				return (_reason.c_str());
			};
	};

	class AlreadyInChannel : public ChannelException {
		
		public:
			AlreadyInChannel() : ChannelException("User already in channel.") {};
			virtual ~AlreadyInChannel() throw() {};
			virtual const char *what() const throw() {
				return (_reason.c_str());
			};
	};

};


#endif
