#define LOGGER_NAME "\033[95mMAIN\033[0m"
#include "SimpleLogs.hpp"
#include "IRCServer.hpp"
#include <cstdlib>

int main(int argc, char **argv) {
	
	if (argc != 3) {
		ERROR("Invalid number of arguments, usage: " << argv[0] << " <port> <password>")
		return (0);
	}

	int port = std::atoi(argv[1]);
	if (port <= 0 || port >= 65535) {
		ERROR(argv[1] << " is not a valid port");
		return (0);
	}

	std::string password = argv[2];
	IRCServer server(port, password);
	try {
		server.open();
	}
	catch (IRCServer::IRCServerException &a) {
		ERROR(a.what())
	}
}
