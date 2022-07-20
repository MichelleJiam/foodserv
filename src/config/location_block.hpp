
#ifndef LOCATION_BLOCK_HPP
# define LOCATION_BLOCK_HPP

#include <iostream>
#include <map>
#include <vector>
#include <algorithm>
#include "../utils.hpp"
#include "directive_validation/directive_valiation.hpp"

struct s_components
{
	bool	uri;
	bool	autoindex;
	bool	root;
	bool	index;
	bool	client_max_body_size;
	bool	error_page;
	bool	fastcgi_pass;
	bool	allowed_methods;
	bool	return_dir;
}	t_flags; // check list of found keywords in locationblock

// actually this is called a context (sad face)
class LocationBlock {
	private:
		t_flags						_check_list;
		LocationUri					_uri;
		bool						_autoindex;
		std::string					_root;
		std::vector<std::string>	_index;
		int							_client_max_body_size;
		std::vector<ErrorPage>		_error_page;
		std::string					_fastcgi_pass;
		AllowedMethods				_allowed_methods;
		ReturnDir					_return_dir;
		LocationBlock(){};
		void						GetDirectiveValuePairs(std::string data);
		int							IsDirective(std::string directive);
		void						SetValue(int directive, std::string input);
		void						CheckListVerification();

	public:
		LocationBlock(std::string data);
		~LocationBlock(){};

		// check if something has been set or not
		bool						IsSet(std::string key);
		// getters
		t_flags						GetFlags() const;
		std::string					GetUri() const;
		bool						GetAutoindex() const;
		std::string					GetRoot() const;
		std::vector<std::string>	GetIndex() const;
		int							GetClientMaxBodySize() const;
		std::string					GetFastCGIPass() const;
		std::vector<ErrorPage>		GetErrorPage() const;
		std::vector<std::string>	GetAllowedMethods() const;

		// exception classes
		class InvalidDirectiveException : public std::exception
		{
			public:
				const char *what() const throw() {
					return "ERROR! Invalid directive detected in LocationBlock.";
				}
		};
		class MultipleAutoindexException : public std::exception
		{
			public:
				const char *what() const throw() {
					return "ERROR! Multiple autoindex directives detected in Location block.";
				}
		};
		class MultipleRootException : public std::exception
		{
			public:
				const char *what() const throw() {
					return "ERROR! Multiple root directives detected in Location block.";
				}
		};
		class MultipleIndexException : public std::exception
		{
			public:
				const char *what() const throw() {
					return "ERROR! Multiple index directives detected in Location block.";
				}
		};
		class MultipleErrorPageException : public std::exception
		{
			public:
				const char *what() const throw() {
					return "ERROR! Multiple error_page directives detected in Location block.";
				}
		};
		class MultipleFastCGIPassException : public std::exception
		{
			public:
				const char *what() const throw() {
					return "ERROR! Multiple fastcgi_pass directives detected in Location block.";
				}
		};
		class MultipleAllowedMethodsException : public std::exception
		{
			public:
				const char *what() const throw() {
					return "ERROR! Multiple allowed_methods directives detected in Location block.";
				}
		};
		class MultipleClientMaxBodySizeException : public std::exception
		{
			public:
				const char *what() const throw() {
					return "ERROR! Multiple Client_max_body_size directives detected in Location block.";
				}
		};
		class BadURIException : public std::exception
		{
			public:
				const char *what() const throw() {
					return "ERROR! Invalid URI detected in Location block.";
				}
		};
		class MultipleReturnException : public std::exception
		{
			public:
				const char *what() const throw() {
					return "ERROR! multiple return directive detected in Location block.";
				}
		};
		class InvalidDirectiveSetCheckException : public std::exception
		{
			public:
				const char *what() const throw() {
					return "ERROR! Trying to check if a nonexistent directive has been set in Location block.";
				}
		};
};

#endif