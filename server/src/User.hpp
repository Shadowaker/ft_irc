#ifndef USER_HPP
# define USER_HPP

# include <string>
# include <iostream>
# include <sys/socket.h>

enum UserStatus {
	PASSWORD,
	NICKNAME,
	USERNAME,
	LOGGED,
	TO_CLOSE
};

class User
{
	private:
		unsigned int	_id;
		std::string		_username;
		std::string		_nickname;
		UserStatus		_status;
		bool			_operator;
		int				_fd;

	public:
		User(unsigned int id, int fd);
		~User();

	bool			isLogged();
	unsigned int	getId();
	std::string		getUsername();
	std::string		getNickname();
	UserStatus		getStatus() const;
	bool			isOperator();

	void			sendMessage(std::string msg);

	void			setUsername(std::string username);
	void			setNickname(std::string nickname);
	void			setStatus(UserStatus status);
	void			setOperator(bool status);
};

#endif
