#include "response_handler.hpp"
#include "file_handler.hpp"

// Config file constructor
ResponseHandler::ResponseHandler(NginxConfig* config)
	:	_config(config), _request(NULL) {}

// Destructor
ResponseHandler::~ResponseHandler() {}

void ResponseHandler::HandleError(Request& request) {
	_request = &request;
	int	error_code = request.GetStatusCode();
	
	std::string	custom_error_page = FindCustomErrorPage(error_code);
	if (!custom_error_page.empty())
		return HandleCustomError(custom_error_page);
	// else
	// 	return HandleError
}

void ResponseHandler::HandleExpect(Request& request) {
	_request = &request;
}

void ResponseHandler::HandleRegular(Request& request) {
	_request = &request;

	_response.SetResolvedPath(_request->GetTargetConfig().GetResolvedPath());
	if (IsRedirected()) {
		SetLocation(_response.GetResolvedPath());
	}
	// check cgi
	// filehandler executor
}

std::string ResponseHandler::FindCustomErrorPage(int error_code) {
	std::map<int, std::string>	custom_error_pages;
	
	custom_error_pages = _request->GetTargetConfig().GetErrorPage();
	auto custom_page = custom_error_pages.find(error_code);
	// if a custom error page has been specified for that error code
	if (custom_page != custom_error_pages.end())
		return custom_page->second;
	return "";
}

void ResponseHandler::HandleCustomError(std::string const& error_page_path) {
	std::string	host = _request->GetTargetURI().GetHost();
	std::string	port = _request->GetTargetURI().GetPort();

	TargetConfig	error_target_config;
	error_target_config.Setup(_config, host, port, error_page_path);

	std::string	resolved_error_page_path = error_target_config.GetResolvedPath();
	try {
		std::fstream* file = _file_handler.GetFile(resolved_error_page_path);
		_response.SetFileStream(file);
	}
	catch (http::exception &e) {
		_request->SetStatusCode(e.which());
		HandleError(*_request);
	}
}

bool	ResponseHandler::IsRedirected() {
	ReturnDir return_dir = _request->GetTargetConfig().GetReturn();

	if (return_dir.IsSet()) {
		_request->SetStatusCode(return_dir.GetCode());
		return true;
	}
	return false;
}

void	ResponseHandler::SetStatusLine() {
	
}

void	ResponseHandler::SetServer() {
	_response.SetHeaderField("Server", "foodserv/1.0");
}

void ResponseHandler::SetDate() {
	char	buf[1000];
	time_t	now = time(0);
	struct tm	tm = *gmtime(&now);
	strftime(buf, sizeof(buf), "%a, %d %b %Y %H:%M:%S %Z", &tm);

	_response.SetHeaderField("Date", std::string(buf));
}

// void	ResponseHandler::SetLastModified() {

// }

// Only called if request is redirected or a POST request is executed (201 created).
void	ResponseHandler::SetLocation(std::string const& path) {
	_response.SetHeaderField("Location", path);
}

void	ResponseHandler::SetContentType() {

}

void	ResponseHandler::SetContentLength() {

}

void	ResponseHandler::SetConnection() {
	if (_request->GetStatusCode() != 100)
		_response.SetHeaderField("Connection", "close");
}

void	ResponseHandler::SetAllow() {
	if (_request->GetStatusCode() == 405)
		_response.SetHeaderField("Allow", GetAllowedMethodsString());
}

std::string	ResponseHandler::GetAllowedMethodsString() {
	std::vector<std::string> methods_vec = _request->GetTargetConfig().GetAllowedMethods();
	string	methods_str;

	for (auto it = methods_vec.begin(); it != methods_vec.end(); it++) {
		if (!methods_str.empty())
			methods_str += ", ";
		methods_str += *it;
	}
	return methods_str;
}


// void	ResponseHandler::ResolveTarget() {
// 	string	target = _request->GetTargetURI().GetPath();
// 	string	resolved_path = _request->GetTargetConfig().GetResolvedPath(target);
// 	response.SetResolvedTargetPath(resolved_target_path);
// }

