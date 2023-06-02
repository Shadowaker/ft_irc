#ifndef SIMPLELOGS_HPP
# define SIMPLELOGS_HPP

# include <iostream>
# define DEBUG_ENABLED false
 
# define RED	"\033[91m"
# define GREEN "\033[92m"
# define YELLOW "\033[93m"
# define BLUE "\033[94m"
# define MAGENTA "\033[95m"
# define BLANK "\033[0m"
# define CYAN "\033[96m"
 
 
// Logging levels
# define LOG(x, y) std::cout << "[" << x << "]\t" << LOGGER_NAME << ": " << y << std::endl;
# define INFO(x) LOG(BLUE " INFO  " BLANK, x)
# define WARNING(x) LOG(YELLOW "WARNING" BLANK, x)
# define ERROR(x) LOG(RED " ERROR " BLANK, x)
# define DEBUG(x) if (DEBUG_ENABLED) LOG(GREEN " DEBUG " BLANK, x)
# define PRINT(x) if (DEBUG_ENABLED) std::cout << "" << x << "";

#endif