#include "response_handler.hpp"
#include "response_generator.hpp"
#include "../cgi/cgi_handler.hpp"
#include "../utils/config_utils.hpp"
#include <algorithm> // min
#include <sys/socket.h> // send

// Default constructor
ResponseHandler::ResponseHandler()
		:	_request(NULL), _is_done(false) {
	_response = Response::pointer(new Response);
}

// Destructor
ResponseHandler::~ResponseHandler() {}

bool	ResponseHandler::Ready() {
	return _response->IsComplete();
}

bool	ResponseHandler::IsDone() const {
	return _is_done;
}

void	ResponseHandler::Send(int fd) {
	std::string& send_buffer = _response->GetCompleteResponseString();

	size_t send_size = std::min((size_t)BUFFER_SIZE, send_buffer.size());

	ssize_t bytes_sent = send(fd, send_buffer.c_str(), send_size, 0);
	if (bytes_sent < 0)
		throw SendFailureException();

	if (_response->GetStatusCode() == 100)
		_response = Response::pointer(new Response); // create fresh Response object

	// if max size is being sent, buffer still contains more bytes to send,
	// so erase what's been sent.
	else if (bytes_sent == BUFFER_SIZE)
		send_buffer.erase(0, bytes_sent);
	else {
		send_buffer.clear();
		_is_done = true;
	}
}

void	ResponseHandler::HandleError(Request& request) {
	_request = &request;
	int	error_code = request.GetStatusCode();
	
	std::string	custom_error_page = FindCustomErrorPage(error_code);
	if (!custom_error_page.empty())
		return HandleCustomError(custom_error_page);
	else
		return HandleDefaultError(error_code);
}

void	ResponseHandler::HandleExpect(Request& request) {
	_request = &request;

	FormResponse();
}

void	ResponseHandler::HandleRegular(Request& request) {
	_request = &request;

	if (IsRedirected())
		return HandleRedirection();
	try {
		AssignResponseResolvedPath();
		if (IsHandledByCGI())
			HandleCGI();
		else
			HandleMethod();
		FormResponse();
	}
	catch (http::exception &e) {
		_request->SetStatusCode(e.which());
		HandleError(*_request);
	}
}

Response const&	ResponseHandler::GetResponse() {
	return *_response;
}

// Checks if request target path was able to be resolved.
// If so, assigns response path. If not, throws a 404: Not Found.
// Takes optional path argument for different path than request::resolved_path.
void	ResponseHandler::AssignResponseResolvedPath(std::string const& path) {
	std::string resolved_path;
	
	if (path.empty())
		resolved_path = _request->GetTargetConfig().GetResolvedPath();
	else
		resolved_path = path;

	if (resolved_path.empty())
		throw NotFoundException();
	else if (IsDirectory(resolved_path) && !IsValidDirectory(resolved_path))
		throw ForbiddenException();
	else
		_response->SetResolvedPath(resolved_path);
	_response->SetRequestTarget(_request->GetTargetString());
}

std::string	ResponseHandler::FindCustomErrorPage(int error_code) {
	std::map<int, std::string>	custom_error_pages;
	
	custom_error_pages = _request->GetTargetConfig().GetErrorPage();
	auto custom_page = custom_error_pages.find(error_code);
	// if a custom error page has been specified for that error code
	if (custom_page != custom_error_pages.end())
		return custom_page->second;
	return "";
}

void	ResponseHandler::HandleCustomError(std::string const& error_page_path) {
	std::string root = _request->GetTargetConfig().GetRoot();
	std::string resolved_error_path = root + error_page_path;

	try {
		AssignResponseResolvedPath(resolved_error_path);
		HandleMethod();
		FormResponse();
	}
	catch (http::exception &e) {
		int new_error_code = e.which();
		// prevent infinite error loop with custom error pages
		if (new_error_code == _request->GetStatusCode())
			HandleDefaultError(new_error_code);
		else {
			_request->SetStatusCode(new_error_code);
			HandleError(*_request);
		}
	}
}

void	ResponseHandler::HandleDefaultError(int error_code) {
	try {
		std::string error_page_html(GetServerErrorPage(error_code));
		std::istream* body_stream = CreateStreamFromString(error_page_html);
		if (body_stream == NULL)
			throw InternalServerErrorException();
		_response->SetBodyStream(body_stream);
		_response->SetHeaderField("Content-Type", "text/html");
		FormResponse();
	}
	catch (http::exception &e) {
		_request->SetStatusCode(e.which());
	HandleError(*_request);
	}
}

bool	ResponseHandler::IsRedirected() {
	ReturnDir return_dir = _request->GetTargetConfig().GetReturn();

	if (return_dir.IsSet()) {
		_response->SetStatusCode(return_dir.GetCode());
		return true;
	}
	return false;
}

void	ResponseHandler::HandleRedirection() {
	_response->SetResolvedPath(_request->GetTargetConfig().GetResolvedPath());
	FormResponse();
}

bool	ResponseHandler::IsHandledByCGI() {
	return _request->GetTargetConfig().GetCGIPass().IsSet();
}

void	ResponseHandler::HandleCGI() {
	CGIHandler	cgi_handler;
	std::istream* body_stream = cgi_handler.Execute(_request, *_response);
	_response->SetBodyStream(body_stream);
}

// Assumes _response->_resolved_path has been set already.
void	ResponseHandler::HandleMethod() {
	FileHandler::Method method = DetermineMethod();
	std::istream* body_stream = _file_handler.ExecuteMethod(*_response, method);
	_response->SetBodyStream(body_stream);
}

FileHandler::Method ResponseHandler::DetermineMethod() {
	if (_request->GetStatusCode() > 399) // error status
		return FileHandler::Method::GetError;
	else if (_request->GetTargetConfig().MustGenerateIndex() == true) {
		return FileHandler::Method::GetGeneratedIndex;
	}
	else {
		std::string method = _request->GetMethod();
		if (method == "GET")
			return FileHandler::Method::Get;
		else if (method == "POST") {
			_response->SetMessageBody(_request->GetMessageBody());
			return FileHandler::Method::Post;
		}
		else if (method == "DELETE")
			return FileHandler::Method::Delete;
		else
			throw InternalServerErrorException();
	}
}

void	ResponseHandler::FormResponse() {
	ResponseGenerator	response_generator;

	response_generator.FormResponse(*_response, _request);
}
