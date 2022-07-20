#include "request_parser.hpp"

/*

HandleRequest
|_ ParseRequest
	|_ ParseRequestLine (into struct)
		|_ ParseMethod
		|_ ParseTarget
		|_ ParseHTTP
	|_ ParseHeaderField (into hash table)
		|_ ParseFieldName
		|_ ParseFieldValue
	|_ ParseReceivedforContentIndication
	if expecting message body
		|_ ParseHeaderMessage
	|_ ValidateRequest
CreateResponse
|_ 

	r_Start = 0,
	r_Method,
	r_Target,
	r_Version,
	r_Version_Done,
	r_Field,
	r_Header_Done,
	r_MsgBody,
	r_Done,
	r_Invalid
*/

/*
	Transition table for request parser
	MT = Method, URI, VR = Version, FN = FieldName, FS = FieldStart, FV = FieldValue

	// TCh       VCh       SP       WHTSP    URI       HTTP     ht       :       \n       \0
	{  MT,       x,        x,       x,       x,        x,       x,       x,       x,       x     }, // Start
	{  MT,       x,        URI,     x,       x,        x,       x,       x,       x,       x     }, // Method
	{  x,        x,        VR,      x,       URI,      x,       x,       URI,     x,       x     }, // URI
	{  x,        x,        x,       x,       x,        VR,      x,       x,       FS,      DONE  }, // Version
	{  FN,       x,        x,       x,       x,        x,       x,       x,       DONE,    x     }, // FieldStart
	{  FN,       x,        x,       x,       x,        x,       x,       FV,      x,       x     }, // FieldName
	{  x,        FV,       FV,      DONE,    x,        x,       FV,      x,       FS,      DONE  }, // FieldValue
*/

#define DEBUG 0 // TODO: REMOVE

// Default constructor
RequestParser::RequestParser() : _bytes_read(0) {}

// C-string constructor
RequestParser::RequestParser(char const* buffer) : _bytes_read(0) {
	Parse(buffer);
}

// Destructor
RequestParser::~RequestParser() {}

void	RequestParser::Parse(char const* buffer) {
	string	request(buffer);

	ParseString(request);
}

RequestState	RequestParser::SetStartState() const {
	return r_Start;
}

RequestState	RequestParser::GetNextState(size_t pos) {
	static RequestState (RequestParser::*table[])(size_t pos) = {
			&RequestParser::StartHandler,
			&RequestParser::MethodHandler,
			// &RequestParser::MethodDoneHandler,
			&RequestParser::TargetHandler,
			// &RequestParser::TargetDoneHandler,
			&RequestParser::VersionHandler,
			&RequestParser::VersionDoneHandler,
			&RequestParser::FieldHandler,
			&RequestParser::HeaderDoneHandler,
			// &RequestParser::MessageBodyHandler,
			nullptr
	};
	if (DEBUG) cout << "[GetNextState] pos: " << pos << " state: " << cur_state << " in [pos]: " << input[pos] << endl; // DEBUG
	return (this->*table[cur_state])(pos);
}

void	RequestParser::InvalidStateCheck() const {
	if (cur_state == r_Invalid)
		throw BadRequestException();
}

bool	RequestParser::DoneStateCheck() {
	if (cur_state == r_Done) {
		return true;
	}
	return false;
}

// bool	RequestParser::NotDone(size_t pos) const {
// 	return 
// }

void	RequestParser::IncrementCounter(size_t& pos) {
	pos = _bytes_read;
}

void	RequestParser::PreParseCheck() {

}

void	RequestParser::AfterParseCheck(size_t pos) {
	// if (cur_state == r_Done && pos < input.size() - 1)
	// 	throw BadRequestException();
	cout << "Parsed method: " << _request_line.method << endl; // DEBUG
	cout << "Target input: " << _request_line.target.GetInputURI() << endl; // DEBUG
	cout << "Parsed target: " << _request_line.target.GetURIDebug() << endl; // DEBUG
	cout << "Parsed version: " << _request_line.version << endl;  // DEBUG
	cout << "Parsed headers:\n";
	for (map<string,string>::iterator it = _header_fields.begin();
		it != _header_fields.end(); it++)
			cout << "\tfield: [" << it->first << "] | value: [" << it->second << "]\n";
	(void)pos;
}

// Returns position of `to_find` within string `s` in terms of its distance from
// `start`, so value can be used immediately with `substr()`, skipping arithmetic.
// If no such character is found in string, returns `start`;
static size_t	GetEndPos(string s, char to_find, size_t start) {
	size_t	end = s.find_first_of(to_find, start);
	if (end == string::npos || end == start)
		return start;
	return end - start;
}

// Checks if start is a valid TChar (as stated by RFC ABNF rules).
RequestState	RequestParser::StartHandler(size_t pos) {
	if (DEBUG) cout << "[StartHandler]\n";
	if (IsTChar(input[pos]))
		return r_Method;
	return r_Invalid;
}

// Checks input string for first space-delimited string and checks it against
// vector of accepted method strings. If it's not in the list, throws NotImplementedException.
// Otherwise saves string and increments `bytes_read`.
RequestState	RequestParser::MethodHandler(size_t pos) {
	if (DEBUG) cout << "[MethodHandler]\n";

	static vector<string>	methods = { "GET", "POST", "DELETE" };

	size_t	method_end = GetEndPos(input, ' ', pos);
	if (method_end == pos) // if required space ending not found
		throw BadRequestException();

	string	method = input.substr(pos, method_end);
	if (find(methods.begin(), methods.end(), method) == methods.end())
		throw NotImplementedException();
	_request_line.method = method;
	_bytes_read += method_end + 1;
	// return r_Method_Done;
	return r_Target;
}

// // Checks if input[pos] is a valid delimiter according to the `valid` fn passed.
// // If so, returns the next state.
// RequestState	RequestParser::ValidDelimiter(bool (*valid)(char), size_t pos) {
// 	if (valid(input[pos])) {
// 		RequestState next_state = static_cast<RequestState>(cur_state + 1); // goes to next state in enum
// 		return next_state;
// 	}
// 	return r_Invalid;
// }

// // Checks if method is followed by a Space. If yes, transitions to Target state.
// RequestState RequestParser::MethodDoneHandler(size_t pos) {
// 	if (DEBUG) cout << "method: " << _request_line.method << endl;

// 	// _bytes_read += 1;
// 	return ValidDelimiter(IsSpace, pos);
// }

// Validates request target through URI::Parse().
RequestState	RequestParser::TargetHandler(size_t pos) {
	if (DEBUG) cout << "[TargetHandler]\n";

	size_t	target_end = GetEndPos(input, ' ', pos);
	if (target_end == pos) // if required space ending not found
		throw BadRequestException();
	_request_line.target = input.substr(pos, target_end); // calls on RequestTargetParser
	_bytes_read += target_end + 1;
	return r_Version;
	// return r_Target_Done;
}

// // Checks if target is followed by a Space. If yes, transitions to Version state.
// RequestState RequestParser::TargetDoneHandler(size_t pos) {
// 	if (DEBUG) cout << "target: " << _request_line.target.GetURIDebug() << endl;

// 	_bytes_read += 1;
// 	return ValidDelimiter(IsSpace, pos);
// }

static size_t	GetCRLFPos(string const& input, size_t pos) {
	size_t	nl_pos = GetEndPos(input, '\n', pos);
	size_t	cr_pos = GetEndPos(input, '\r', pos);

	if (nl_pos == pos)
		throw BadRequestException();
	if (cr_pos != pos && (nl_pos - cr_pos == 1)) // if \r is found and precedes \n
		return cr_pos;
	return nl_pos;
}

// Checks if HTTP version is valid (only 1.1 is accepted).
RequestState	RequestParser::VersionHandler(size_t pos) {
	if (DEBUG) cout << "[Version Handler]\n";

	size_t	version_end = GetCRLFPos(input, pos);
	string	version = input.substr(pos, version_end);
	// cout << "version: [" << version << "]\n";
	if (version.front() != 'H' || !isdigit(version.back()))
		throw BadRequestException();
	if (version != "HTTP/1.1")
		throw HTTPVersionNotSupportedException();
	_request_line.version = version;
	_bytes_read += version_end; // TODO: consider folding VersionDone into here
	return r_Version_Done;
}

// Used by VersionDoneHandler and FieldDoneHandler to check for
// valid line breaks (CRLF or LF as specified by RFC 7230).
static size_t	ValidLineBreaks(string input, size_t pos) {
	if (input[pos] == '\r' && input[pos + 1] == '\n')
		return 2;
	else if (input[pos] == '\n')
		return 1;
	else
		return 0;
}

// Checks if version is followed by CRLF/LF. If yes, transition to FieldStart.
RequestState	RequestParser::VersionDoneHandler(size_t pos) {
	size_t	line_breaks = ValidLineBreaks(input, pos);

	if (line_breaks == 0)
		return r_Invalid;
	else {
		_bytes_read += line_breaks;
		return r_Field;
	}
}

RequestState	RequestParser::FieldHandler(size_t pos) {
	if (DEBUG) cout << "[Field Handler] at: [" << input[pos] << "]\n";

	size_t	field_end = input.find("\n\n", pos);
	if (field_end == string::npos) // if header is not correctly ended by empty line
		throw BadRequestException();

	_header_parser.Parse(_header_fields, input.substr(pos, field_end));
	return r_Header_Done;
}

RequestState	RequestParser::HeaderDoneHandler(size_t pos) {
	if (DEBUG) cout << "[Header Done Handler] at: " << input[pos] << endl;

	// Check parsed headers if message is expected
	return r_Done;
}

string	RequestParser::GetMethod() {
	return _request_line.method;
}

string	RequestParser::GetTarget() {
	return _request_line.target.Get();
}

string	RequestParser::GetVersion() {
	return _request_line.version;
}

string	RequestParser::GetHeaderFieldValue(string field_name) {
	NormalizeString(tolower, field_name, 0); // normalizes field name for case-insensitive searching
	map<string, string>::iterator	found =  _header_fields.find(field_name);
	if (found == _header_fields.end())
		return NO_VAL;
	return found->second;
}

string	RequestParser::GetMessageBody() {
	return _msg_body;
}
#undef DEBUG // REMOVE