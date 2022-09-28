#ifndef FILE_HANDLER_HPP
#define FILE_HANDLER_HPP

#include <iostream>
#include "response.hpp"

class FileHandler {
	public:
		enum class Method {
			Get = 1,
			GetError,
			GetGeneratedIndex,
			Post,
			Delete
		};

		FileHandler();
		~FileHandler();

		std::istream*	GetFile(std::string const& file_path);
		std::istream*	CreateFile(std::string const& file_path);
		std::string		GetExtension(std::string const& file_path) const;
		std::istream*	ExecuteMethod(Response& response, Method method);
	
	private:
		void			GetFileHandlingError(void);
		std::istream*	ExecuteGet(Response& response, bool error_page = false);
		std::istream*	ExecuteGetGeneratedIndex(Response& response);
		std::string		GetTargetDir(std::string const& dir_path);
		std::string		FormatLine(struct dirent* dir_entry, std::string const& path);
		std::istream*	ExecutePost(std::string const& file_path);
		std::istream*	ExecuteDelete(std::string const& file_path);
};

#endif /* FILE_HANDLER_HPP */
