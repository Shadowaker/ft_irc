#ifndef UTILS_HPP
# define UTILS_HPP

# include <string>
# define WHITESPACE " \n\r\t\f\v"

/* Everything to string */
template <typename T>
std::string toString(T val) {
	std::stringstream stream;
	stream << val;
	return stream.str();
}

#endif