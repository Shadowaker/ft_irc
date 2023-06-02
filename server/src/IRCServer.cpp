#define LOGGER_NAME "\033[92mIRCServer\033[0m"
#include "IRCServer.hpp"
#include "SimpleLogs.hpp"

static std::string	freplace(std::string str, std::string src, std::string pst)
{
	std::string	res;
	std::string buff;

	size_t	size = str.size();

	for (size_t x=0; x < size; x++)
	{
		if (str[x] == src[0])
		{
			buff = str.substr(x, src.size());
			if (!buff.compare(src))
			{
				res += pst;
				x += src.size() - 1;
			}
		}
		else
			res += str[x];
	}
	return (res);
}

IRCServer::IRCServer(int port, std::string password) : port(port), password(password) {}

IRCServer::~IRCServer() {
	for (std::map<std::string, Channel *>::iterator x = channels.begin(); x != channels.end(); x++)
		delete x->second;
	
	size_t	end = this->users.size();

	for (size_t x = 0; x < end; x++) {
		delete users[x];
	}
}

void	IRCServer::socketInit()
{
	DEBUG(" Init options...")
	int something = 1;
	this->socket_fd = socket(AF_INET, SOCK_STREAM, 6);

	if (this->socket_fd == -1) {
		throw IRCServer::SocketError();
	}

	if (setsockopt(this->socket_fd, SOL_SOCKET, SO_REUSEADDR, &something, sizeof(something)) == -1) {
		throw IRCServer::SocketOptError();
	}

	if (fcntl(this->socket_fd, F_SETFL, O_NONBLOCK) == -1) {
		throw IRCServer::SocketOptErrorAsync();
	}

	struct sockaddr_in address;
	address.sin_family = AF_INET;
	address.sin_addr.s_addr = INADDR_ANY;
	address.sin_port = htons(this->port);

	if (bind(this->socket_fd, (const struct sockaddr *) &address, sizeof(address)) == -1) {
		throw IRCServer::SocketBindError();
	}

	if (listen(this->socket_fd, address.sin_port) < 0) {
		throw IRCServer::ListenError();
	}
	DEBUG(" \033[92m[✓]\033[0m")
}

void	IRCServer::pollInit()
{
	memset(this->poll_fds, 0, sizeof(this->poll_fds));
	this->poll_fds[0].fd = this->socket_fd;
	this->poll_fds[0].events = POLLIN;
	this->n_poll_fds = 1;
}

void	IRCServer::newConnection()
{
	PRINT("\n\n---------------------------------\n\n")
	DEBUG(" Listening socket is readable.");

	int	new_fd = 1;

	while (new_fd != -1 && new_fd != 0)
	{
		new_fd = accept(this->socket_fd, NULL, NULL);

		if (new_fd == -1)
		{
			if (errno != EWOULDBLOCK)
				WARNING(" CAN'T ACCEPT NEW CLIENT")
			break;
		}

		INFO(" New incoming connection -> " << new_fd);
		this->poll_fds[this->n_poll_fds].fd = new_fd;
		this->poll_fds[this->n_poll_fds].events = POLLIN;
		this->addUser(this->n_poll_fds, this->poll_fds[this->n_poll_fds].fd);
		this->n_poll_fds++;
	}
}

bool	IRCServer::handler(int i, char *buffer)
{
	int			rec;
	std::string	temp = "";
	bool		close_conn = false;
	bool		compress_array = false;

	PRINT("\n\n---------------------------------\n\n")
	DEBUG(" Fd " << this->poll_fds[i].fd << " is readable.");
	this->client = i;

	while (true)
	{
		memset(buffer, 0, 1024);
		rec = recv(this->poll_fds[i].fd, buffer, sizeof(buffer), MSG_DONTWAIT);
		if (rec < 0)
		{
			if (errno != EWOULDBLOCK) {
				WARNING(" RECV: CLIENT NOT REACHABLE.")
				close_conn = true;
			}
			break;
		}

		/* Check to see if the connection has been closed		*/
		if (rec == 0)
		{
			close_conn = true;
			INFO(" Fd " << this->poll_fds[i].fd << " Connection closed.");
			this->rmUser(this->client);
			break ;
		}

		/* Data received										*/
		DEBUG(" " << rec << " bytes received.");
		DEBUG(" BUFFER: " << "$" << buffer << "$");

		std::ostringstream	client_id;
		client_id << this->client - 1;

		temp = buffer;
		if (temp.find("\n") == std::string::npos)
		{
			if (this->requests.find(client_id.str()) == this->requests.end())
				this->requests[client_id.str()] = temp;
			else
				this->requests[client_id.str()] = this->requests[client_id.str()] + temp;
		}
		else
		{
			if (this->requests.find(client_id.str()) != this->requests.end())
			{
				this->request = this->requests[client_id.str()] + temp;
				size_t nl_pos = this->request.find("\n");

				if (nl_pos != this->request.length()) {
					this->requests[client_id.str()] = this->request.substr(nl_pos + 1, this->request.length());
					this->request = this->request.substr(0, nl_pos);
				}
				else {
					this->requests.erase(client_id.str());
				}
			}
			else
				this->request = temp;
			this->request = freplace(this->request, "\r", "");
			this->request = freplace(this->request, "\n", "");

			INFO(" REQUEST: $" << this->request << "$");
			CommandRouter().route(this->users[this->client - 1], this, this->request);
			if (this->users[this->client - 1]->getStatus() == TO_CLOSE) {
				INFO(" Client " << this->client - 1 << " Connection quitted.");
				close_conn = true;
				this->rmUser(this->client);
				break ;
			}
		}
		break ;
	}
	if (close_conn)
	{
		close(this->poll_fds[i].fd);
		this->poll_fds[i].fd = -1;
		compress_array = true;
	}
	return compress_array;
}

void	IRCServer::compressArray()
{
	if (this->n_poll_fds > 2)
	{
		for (int i = 0; i < this->n_poll_fds; i++)
		{
			if (this->poll_fds[i].fd == -1)
			{
				for(int	j = i; j < this->n_poll_fds; j++)
					this->poll_fds[j].fd = this->poll_fds[j+1].fd;
				i--;
				this->n_poll_fds--;
			}
		}
	}
	else
		this->n_poll_fds--;
}

void	IRCServer::loop()
{
	char		buffer[1024];
	int			rc;
	int			compress_array = false;

	while (true)
	{
		rc = poll(this->poll_fds, this->n_poll_fds, INT_MAX);
		if (rc == -1)
			throw IRCServer::PollError();

		if (rc == 0)
			throw IRCServer::PollTimeout();

		for (int i = 0; i < this->n_poll_fds; i++)
		{
			if (this->poll_fds[i].fd == this->socket_fd)
				this->newConnection();
			else {
				try {
					compress_array = this->handler(i, buffer);
				}
				catch(const IRCServer::KillServer &e) {
					this->compressArray();
					throw IRCServer::KillServer();
				}
			}
			if (compress_array)
			{
				compress_array = false;
				this->compressArray();
			}
		}
	}
}

void	IRCServer::open()
{
	INFO("Starting IRCServer...")

	try
	{
		this->socketInit();				// Setting Socket Options
	}
	catch (IRCServer::IRCServerException &a)
	{
		throw IRCServer::IRCServerException(a.what());
	}

	this->pollInit();					// Initializing Poll structure
	this->channelCreate("welcome");		// Create 'Welcome' channel

	try {
		this->loop();
	}
	catch (IRCServer::PollTimeout &a) {
		WARNING (" Feeling a little lonely...")
	}
	catch (IRCServer::KillServer &a) {
		WARNING (" Kill signal received.")
	}
	catch (IRCServer::IRCServerException &a) {
		ERROR(" CONNECTION ABORTED." << std::endl)
		ERROR(a.what())
	}
	
	INFO (" Shutting down...")
	for (int i = 0; i < this->n_poll_fds; i++) {
		if(this->poll_fds[i].fd >= 0)
			close(this->poll_fds[i].fd);
	}

	INFO(" SERVER CLOSED.")
}

void	IRCServer::addUser(unsigned int user_id, int fd)
{
	User	*user = new User(user_id, fd);

	this->users.push_back(user);
}

void	IRCServer::rmUser(unsigned int user_id)
{
	User	*tmp;
	size_t	end = this->users.size();

	for (size_t x = 0; x < end; x++)
	{
		if (this->users[x]->getId() == user_id)
		{
			tmp = this->users[x];
			this->users.erase(this->users.begin() + x);
			delete tmp;
		}
	}
}

void	IRCServer::sendMessage(std::string nickname, std::string message)
{
	size_t	end = this->users.size();

	for (size_t x = 0; x < end; x++)
	{
		if (this->users[x]->getNickname() == nickname) {
			this->users[x]->sendMessage(message);
			return ;
		}
	}
	throw IRCServer::UserNotFound();
}

void	IRCServer::channelCreate(std::string name)
{
	if (this->channels.find(name) != this->channels.end()) {
		throw ChannelExist();
	}
	this->channels[name] = new Channel(name);
}

void	IRCServer::channelsClear()
{
	std::map<std::string, Channel *>::iterator end = this->channels.end();
	for (std::map<std::string, Channel *>::iterator x = this->channels.begin(); x != end; x++)
		delete x->second;
}

void	IRCServer::channelDelete(std::string name)
{
	delete this->channels[name];
	this->channels.erase(name);
}

std::string IRCServer::getPassword() const
{
	return this->password;
}

std::vector<User *> IRCServer::getUsers()
{
	return this->users;
}

User* IRCServer::getUserByNickname(std::string nickname) {
	size_t	size = users.size();

	for (size_t i = 0; i < size; i++) {
		if (users[i]->getNickname() == nickname) {
			return users[i];
		}
	}
	return NULL;
}

std::map<std::string, Channel *> IRCServer::getChannels()
{
	return (this->channels);
}

void	IRCServer::broadcastMessage(std::string message, User *except)
{
	size_t		size = this->users.size();

	for (size_t x = 0; x < size; x++) {
		if (this->users[x] != except) {
			this->users[x]->sendMessage(message);
		}
	}
}
