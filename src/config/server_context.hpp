#ifndef SERVER_CONTEXT_HPP
# define SERVER_CONTEXT_HPP
#include <iostream>
#include <algorithm>
#include <map>
#include "location_context.hpp"
#include "server_name.hpp"
#include "directive_validation/directive_validation.hpp"
#include "../utils/config_utils.hpp"
#include "config_interface.hpp"
#include "listen.hpp"
#include <vector>
#include <string>

class ServerContext : public ConfigValues {
	private:

		size_t	_amount_location_context;
		bool	bool_listen;
		bool	bool_server_name;

		size_t								_server_nb;
		std::vector<LocationContext>		_location_contexts;
		std::pair<std::string, std::string>	_listen;
		std::vector<std::string>			_server_name;

		size_t					FindLocationContextEnd(std::string config_file, size_t start);
		//overridden base class functions
		void					GetDirectiveValuePairs(size_t *start_position, std::string config_file); // in this case i do not use override as i want to use it differently.
		
		// wrappers for configvalues
		void					SetServerRoot(std::string value);
		void					SetServerIndex(std::string value);
		void					SetServerCMBS(std::string value);
		void					SetServerErrorPage(std::string value);
		void					SetServerAutoindexDir(std::string value);
		void					SetServerReturn(std::string value);

		// verification: in server_context_verification (along with HasLocation)
		virtual int				IsDirective(std::string const directive) override;
		int         			GetDirective(std::string const directive);
		void					DoubleDirectiveCheck(int const directive);
		// setters
		size_t					FindValue(int const directive, std::string config_file, size_t key_end);
		virtual void			SetValue(int const directive, std::string value) override;
		
		void					SetLocation(std::string trimmed_value);
		void					SetListen(std::string trimmed_value);
		void					SetServerName(std::string trimmed_value);
		void					CopyValues(const ServerContext& obj);

	public:
		ServerContext();
		ServerContext(size_t *start, std::string config_file, size_t server_nb); // uses a pointer so it can skip through the server bits on its own when it returns
		ServerContext & operator= (const ServerContext &server_context);
		~ServerContext(){};
		
		// check if set
		bool									HasLocation(std::string const target);
		virtual bool							IsSet(std::string key) override;
		//getters
		std::vector<LocationContext>			GetLocationContexts() const;
		std::pair<std::string, std::string>		GetListen() const;
		std::string								GetIPAddress() const;
		std::string								GetPortNumber() const;
		std::vector<std::string>				GetServerNameVector() const;
};

#endif