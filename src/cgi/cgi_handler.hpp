#ifndef CGI_HANDLER_HPP
# define CGI_HANDLER_HPP

#include "cgi.hpp"
#include "../response/response.hpp"
#include "../request/parser/header_field_parser.hpp"
#include "../utils/utils.hpp"

#include <string>

class CGIHandler {
	private:
		CGI					_cgi;
		std::istream 		*_content;

		void				ExecuteCGI(Request *request);
		size_t				SetHeaders(Response* response);
		std::string			RetrieveBody(size_t start);
		std::istream*		ToIStream(std::string body);
		void				SetStatus(Response* response);
		void				CreateBodyStream(std::string body);

	public:
		CGIHandler();
		~CGIHandler(){};
		std::istream*	Execute(Request *request, Response& response);
		std::string		GetContent() const;
};

#endif