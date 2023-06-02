#define LOGGER_NAME "\033[92mIrcbot\033[0m"
#include "SimpleLogs.hpp"
#include "Bot.hpp"


Bot::Bot(std::string username, std::string nickname, std::string host, std::string port, std::string pass) : _host(host), _port(port), _pass(pass), _username(username), _nickname(nickname)
{
	int sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if (sockfd < 0)
		throw std::runtime_error("Error while opening socket.");

	struct sockaddr_in serv_address = {};

	bzero((char *) &serv_address, sizeof(serv_address));
	serv_address.sin_family = AF_INET;
	serv_address.sin_addr.s_addr = inet_addr(host.c_str());
	serv_address.sin_port = htons(std::atoi(port.c_str()));

	if (connect(sockfd, (struct sockaddr *) &serv_address, sizeof(serv_address)) < 0)
		throw std::runtime_error("Error while connecting to host.");
	
	this->_fd = sockfd;
	this->_isclosed = false;
}

Bot::~Bot() 
{
	close(this->_fd);
}

std::string    Bot::listen() 
{
	char        buffer[1024];
	int         size;
	std::string	req;

	memset(buffer, 0, 1024);
	size = recv(this->_fd, buffer, 1024, 0);
		
	if (size == 0) {
		this->_isclosed = true;
		return (std::string("\n"));
	}
	req += std::string(buffer);
	
	DEBUG("REQUEST: $" << buffer << "$");
	return (std::string(buffer));
}

void    Bot::run()
{
	std::string request;
	INFO("Bot Started.")
	INFO("Login started...")

	std::string pass = std::string("PASS ").append(this->_pass).append("\n");
	send(this->_fd, pass.c_str(), pass.length(), 0);

	std::string nick = std::string("NICK ").append(this->_nickname).append("\n");
	send(this->_fd, nick.c_str(), nick.length(), 0);

	std::string user = std::string("USER ").append(this->_username).append("\n");
	send(this->_fd, user.c_str(), user.length(), 0);
	INFO("Login completed.")

	INFO("Trying to join #welcome...")
	send(this->_fd, "JOIN #welcome\n", 15, 0);
	INFO("Joined #welcome")

	while (!this->_isclosed) {
		request = this->listen();
		this->handler(request);
	}

	INFO("Goodnight.")
	close(this->_fd);
}

void    Bot::handler(std::string request)
{
	if (request.find("PRIVMSG") != std::string::npos) {
		std::string user = request.substr(1, request.find(" "));
		if (request.find("ciao") != std::string::npos) {
			DEBUG ("Saying hi.")
			std::string resp = std::string("PRIVMSG #welcome").append(" :Ciao!\n\r");
			send(this->_fd, resp.c_str(), resp.length(), 0);
		}
	}
	else if (request.find("QUIT") != std::string::npos) {
		std::string user = request.substr(1, request.find(" "));
		DEBUG ("Saying bye.")
		std::string resp = std::string("PRIVMSG #welcome").append(" :Bye bye!\n\r");
		send(this->_fd, resp.c_str(), resp.length(), 0);
	}
}