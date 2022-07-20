
#include "location_block.hpp"
#include "directive_validation/directive_validation.hpp"


LocationBlock::LocationBlock(std::string data) {
    std::cerr << "In LocationBlock now" << std::endl;
    _check_list.uri = false;
    _check_list.autoindex = false;
    _check_list.root = false;
    _check_list.index = false;
    _check_list.client_max_body_size = false;
	_check_list.error_page = false;
    _check_list.fastcgi_pass = false;
	_check_list.allowed_methods = false;
    GetDirectiveValuePairs(data);
}

LocationBlock& LocationBlock::operator= (const LocationBlock& location_block) {
    if (this == &location_block)
        return (*this);
    _uri = location_block.GetUri();
    _autoindex = location_block.GetAutoindex();
    _root = location_block.GetRoot();
    _index = location_block.GetIndex();
    _client_max_body_size = location_block.GetClientMaxBodySize();
	_error_page = location_block.GetErrorPage();
    _fastcgi_pass = location_block.GetFastCGIPass();
	_allowed_methods = location_block.GetAllowedMethods();
    return (*this);
}
LocationBlock::LocationBlock(const LocationBlock& location_block) {
    _uri = location_block.GetUri();
    _autoindex = location_block.GetAutoindex();
    _root = location_block.GetRoot();
    _index = location_block.GetIndex();
    _client_max_body_size = location_block.GetClientMaxBodySize();
	_error_page = location_block.GetErrorPage();
    _fastcgi_pass = location_block.GetFastCGIPass();
	_allowed_methods = location_block.GetAllowedMethods();
}

int			            LocationBlock::IsDirective(std::string directive) {
	const std::string	directives[] = {"autoindex", "root", "index", "client_max_body_size", "error_page", "fastcgi_pass", "allowed_methods", "return"};
	
	std::cout << "directive: " << directive << std::endl;
    if (_check_list.uri == false) {
		return 8;
	}
	int	is_directive = std::find(directives, directives + 8, directive) - directives;
	if (is_directive < 0 || is_directive > 7)
		throw InvalidDirectiveException();
	else
		return (is_directive);
}

void				LocationBlock::SetValue(int directive, std::string input) {
	std::string		value;

	value = TrimValue(input);
	std::cout << "value: |" << value << "|" << std::endl;

	if (directive == 8) {
		_check_list.uri = true;
		_uri = uri_value(value);
	}
	else {
		switch(directive) {
			case 0: {
				if (_check_list.autoindex == true)
					throw MultipleAutoindexException();
				_check_list.autoindex = true;
				Autoindex	autoindex_value(value);
				_autoindex = autoindex_value.GetStatus();
				break ;
			}
			case 1: {
				if (_check_list.root == true)
					throw MultipleRootException();
				_check_list.root = true;
				Root root_value(value);
				_root = value;
				break ;
			}
			case 2: {
				if (_check_list.index == true)
					throw MultipleIndexException();
				_check_list.index = true;
				Index	index_value(value);
				_index = index_value.GetIndex();
				break ;
			}
			case 3: {
				if (_check_list.client_max_body_size == true)
					throw MultipleClientMaxBodySizeException();
				_check_list.client_max_body_size = true;
				ClientMaxBodySize	cmbs_value(value);
				_client_max_body_size = cmbs_value.GetValue();
				break ;
			}
            case 4: {
				_check_list.error_page = true;
				ErrorPage	error_page_value(value);
				_error_page.push_back(error_page_value);
				break ;
			}
            case 5: {
				if (_check_list.fastcgi_pass == true)
					throw MultipleFastCGIPassException();
				_check_list.fastcgi_pass = true;
				FastCGIPass fastcgi_pass_value(value);
				_fastcgi_pass = value;
				break ;
			}
            case 6: {
				if (_check_list.allowed_methods == true)
					throw MultipleAllowedMethodsException();
				_check_list.allowed_methods = true;
				AllowedMethods allowed_methods_value(value);
				_allowed_methods = allowed_methods_value.GetAllowedMethods();
				break ;
			}
            case 7: {
				if (_check_list.return_dir == true)
					throw MultipleReturnException();
				_check_list.allowed_methods = true;
				_allowed_methods = return_dir_value(value);
				break ;
			}
		}
	}
}

void			LocationBlock::CheckListVerification(){
    if (_check_list.autoindex == false) {
		_autoindex = false;
		std::cerr << "WARNING! No autoindex in locationblock detected. Default (off) have been set." << std::endl;
	}
    if (_check_list.root == false) {
		_root = "/var/www/html";
		std::cerr << "WARNING! No location root detected. Default (/var/www/html) have been set." << std::endl;
	}
	if (_check_list.index == false) {
		Index	input_value("index.php index.html index.htm index.nginx-debian.html");
		_index = input_value.GetIndex();
		std::cerr << "WARNING! No location index detected. Default (index.php index.html index.htm index.nginx-debian.html) have been set." << std::endl;
	}
	if (_check_list.client_max_body_size == false) {
		_client_max_body_size = 0;
		std::cerr << "WARNING! No client_max_body_size in locationblock detected. Default (0) have been set." << std::endl;
	}
	if (_check_list.allowed_methods == false)
		std::cerr << "WARNING! No allowed_methods in locationblock detected. Default (No methods allowed) have been set." << std::endl;
}

void                        LocationBlock::GetDirectiveValuePairs(std::string data) {
    size_t				key_start = 0;
	size_t				key_end = 0;
	size_t				value_end = 0;
	int					ret;
    int                 i = 0;
    
    while (data[i] != '}') {
		key_start = data.find_first_not_of(" \t\n\v\f\r", i);
		if (data[key_start] == '}') {
			break ;
		}
		key_end = data.find_first_of(" \t\n\v\f\r", key_start);
		ret = IsKey(data.substr(key_start, key_end - key_start));
		if (ret == 7) {
			std::cout << "uri block found" << std::endl;
            SetValue(ret, data.substr(key_start, key_end - key_start));
			value_end = data.find_first_of('{', key_end);
		}
		else {
            value_end = data.find_first_of(';', key_end);
		    SetValue(ret, data.substr(key_end, value_end - key_end));
        }
		i = value_end + 1;
	}
	CheckListVerification();
}

// getters
LocationUri					LocationBlock::GetUri() const {
    return this->_uri;
}

bool						LocationBlock::GetAutoindex() const {
    return this->_autoindex;
}

std::string					LocationBlock::GetRoot() const {
    return this->_root;
}

std::vector<std::string>		LocationBlock::GetIndex() const {
    return this->_index;
}

int							LocationBlock::GetClientMaxBodySize() const {
    return this->_client_max_body_size;
}

std::vector<ErrorPage>	    LocationBlock::GetErrorPage() const {
    return this->_error_page;
}

std::string	                LocationBlock::GetFastCGIPass() const {
    return this->_fastcgi_pass;
}

std::vector<std::string>	                LocationBlock::GetAllowedMethods() const {
    return this->_allowed_methods;
}