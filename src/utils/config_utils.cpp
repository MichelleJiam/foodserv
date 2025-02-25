#include "config_utils.hpp"

size_t		ft_atosize_t(std::string value)
{
	size_t	number = 0;
	size_t	i = value.find_first_not_of(" \t\n\v\f\r");
 
	if (value[i] == '-' || value[i] == '+')
	{
		if (value[i] == '-')
			throw NegativeNumberException();
		i++;
	}
	while (std::isdigit(value[i]) && value[i])
	{
		number = (number * 10) + (value[i] - '0');
		i++;
	}
	return (number);
}

std::string	TrimValue(std::string value){
	size_t	start = 0;
	size_t	end = 0;

	start = value.find_first_not_of(" \t\n\v\f\r");
	if (start == std::string::npos)
		return (NULL);
	end = value.find_last_not_of(" \t\n\v\f\r");
	return (value.substr(start, end - start + 1));
}

std::vector<std::string>	ToStringVector(std::string str) {
	size_t	start = 0;
	size_t	end = 0;
	std::string key;
	std::vector<std::string> vec;

	for (std::string::size_type i = 0; i < str.size(); i++) {
		start = str.find_first_not_of(" \t\n\v\f\r", i);
		if (start == std::string::npos)
			break ;
		end = str.find_first_of(" \t\n\v\f\r", start);
		key = str.substr(start, end - start);
		vec.push_back(key);
		if (end == std::string::npos)
			break ;
		i = end;
	}
	return (vec);
}

size_t		CountArguments(std::string str) {
	size_t	i = 0;
	size_t	start = 0;
	size_t	end = 0;
	size_t	arguments = 0;

	while (str[i]) {
		start = str.find_first_not_of(" \t\n\v\f\r", i);
		if (start == std::string::npos)
			return (arguments);
		end = str.find_first_of(" \t\n\v\f\r", start);
		arguments++;
		if (end == std::string::npos)
			break ;
		i = end;
	}
	return (arguments);
}

bool	IsNumber(std::string input) {
	size_t	i = 0;

	while (i < input.size()) {
		if (std::isdigit(input[i]) == 0)
			return false;
		i++;
	}
	return true;
}

bool	IsUri(std::string input) {
	if (input[0] == '/')
		return true;
	return false;
}

bool	IsDirectory(std::string input) {
	if (input[input.size() - 1] == '/')
		return true;
	else
		return false;
}

bool IsValidHTTPCode(size_t code) {
	if (code < 100 || code > 599)
		return false;
	return true;
}

int		IsValidHTTPMethod(std::string method) {
	const std::string	methods[] = {"GET", "POST", "DELETE"};
	int is_method = std::find(methods, methods + 3, method) - methods;
	if (is_method < 0 || is_method > 2)
		throw AllowedMethods::BadMethodException();
	return (is_method);
}

bool HasContent(char end_char, size_t key_end, size_t value_end, std::string config) {
	size_t i = 0;
	while(std::isspace(config[key_end + i]) && key_end + i < value_end) {
		i++;	
	}
	if (config[key_end + i] == end_char)
		return false;
	return true;
}
