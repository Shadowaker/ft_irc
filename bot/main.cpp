# include "Bot.hpp"

int main(int argc, char **argv) 
{
	try {
		if (argc < 3) {
			throw std::runtime_error("Usage: ./ircbot <hostname> <port> [pass]");
		}
		Bot bot("bot", "bottino", argv[1], argv[2], argv[3]);
		bot.run();
	}
	catch (const std::exception &ex) {
		std::cerr << ex.what() << std::endl;
	}
}