#include "uri_state_parser.hpp"

URIStateParser::URIStateParser(URI& uri) : _uri(&uri) {}

URIStateParser::~URIStateParser() {}

/*
	//  /        %        ?       PCh       Hex       #       \0
	{  PT,       x,       x,       x,        x,       x,       x     }, // Start
	{  PT,       PR,      QR,      PT,       PT,      x,       DONE  }, // Path
	{  x,        x,       x,       x,        PH,      x,       x     }, // Percent
	{  PT/QR,    PR,      QR,      PT/QR,    PT/QR,   x/DONE,  DONE  }, // PercentDone
	{  QR,       PR,      QR,      QR,       QR,      DONE,    DONE  }, // Query

	// static URIState const uri_transitions[10][256] 
*/

// Optional `part` argument for later parsing of host field or query field.
void	URIStateParser::Parse(string uri_string, URIPart part) {
	_part = part;
	_uri_input = uri_string;
	if (_part == pt_Host)
		ParseHost(uri_string);
	ParsePathOriginForm(uri_string);
}

void	URIStateParser::ParseHost(string const& uri_string) {
	(void)uri_string;
}

// Finite state machine parser that parses URI string input byte-by-byte,
// using a jump table of handler functions for each state, which sets
// the next state based on current input character.
void	URIStateParser::ParsePathOriginForm(string const& uri_string) {
	static URIState (URIStateParser::*uri_jumptable[10])(char uri_char) = {
			&URIStateParser::StartHandler,
			&URIStateParser::PathHandler,
			&URIStateParser::QueryHandler,
			&URIStateParser::PercentHandler,
			&URIStateParser::PercentDoneHandler,
			nullptr
	};

	if (uri_string.size() > 8190)
		throw URITooLongException();

	URIState state = st_Start;
	for (size_t i = 0; i < uri_string.size(); i++) {
		state = (this->*uri_jumptable[state])(uri_string[i]);
		_buffer += uri_string[i];
		if (state == st_Invalid)
			throw BadRequestException();
		else if (state == st_Done)
			break;
		// TODO: check if need to throw when finish state is indicated but still characters in string
	}
	FillPathQueryFields();
}

void	URIStateParser::FillPathQueryFields() {
	size_t path = _buffer.find_first_of("/");
	size_t query = _buffer.find_first_of("?");
	size_t query_end = _buffer.find_last_not_of("#");
	if (path != string::npos)
		_uri->SetPath(_buffer.substr(path, query));
	if (query != string::npos)
		_uri->SetQuery(_buffer.substr(query + 1, query_end - query));
}

// Starting state transition handler. Only accepts '/' according to origin form rules.
URIState		URIStateParser::StartHandler(char uri_char) {
	if (uri_char == '/')
		return st_Path;
	return st_Invalid;
}

// Handles transition after '/' input indicating path section in URI.
// Always checks that no 2 consecutive '/' are given, which is only used
// by authority URI components.
URIState		URIStateParser::PathHandler(char uri_char) {
	_part = pt_Path;
	switch (uri_char) {
		case '\0':
			return st_Done;
		case '%':
			return st_Percent;
		case '?':
			return st_Query;
		case '/':
			if (_buffer.back() != '/')
				return st_Path;
		default:
			if (IsPChar(uri_char))
				return st_Path;
			else
				return st_Invalid;
	}
}

// Handles transition after '?' input indicating queries is found.
// '#' is accepted alternative to EOL signaling end of query string.
URIState		URIStateParser::QueryHandler(char uri_char) {
	_part = pt_Query;
	switch (uri_char) {
		case '\0': case '#':
			return st_Done;
		case '%':
			return st_Percent;
		case '/': case '?':
			return st_Query;
		default:
			if (IsPChar(uri_char))
				return st_Query;
			else
				return st_Invalid;
	}
}

// Handles transition after percent-encoding has been found (% input).
// Checks if subsequent 2 characters are valid hexadecimal digits.
URIState		URIStateParser::PercentHandler(char uri_char) {
	if (_buffer.back() == '%' && IsHexDig(uri_char))
		return st_Percent;
	else if (IsHexDig(_buffer.back()) && IsHexDig(uri_char))
		return st_Percent_Done;
	return st_Invalid;
}

// Handles transition after a valid %HH sequence.
// PChar & '/' input trigger transition to either Query or Path state
// depending on if we're at the Path or Query part of the URI.
// "return URIState(_path)" leverages equivalency between pt_Path & pt_Query values
// in URIPart enum and st_Path & Query state values in the URIState enum.
// So if we're at Path part, we return the Path state. Ditto for query.
URIState		URIStateParser::PercentDoneHandler(char uri_char) {
	switch (uri_char) {
		case '\0':
			return st_Done;
		case '#':
			if (_part == pt_Query)
				return st_Done;
		case '%':
			return st_Percent;
		case '?':
			return st_Query;
		case '/':
			return URIState(_part); // pt_Path == st_Path; pt_Query == st_Query
		default:
			if (IsPChar(uri_char))
				return URIState(_part);
			else
				return st_Invalid;
	}
}
