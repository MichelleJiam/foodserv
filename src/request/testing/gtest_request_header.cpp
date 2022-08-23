#include <gtest/gtest.h>
#include <string>

#include "../exception.hpp"
#include "../header_field_validator.hpp"
#include "../request.hpp"

static string GET_RL = "GET /hello.txt HTTP/1.1\r\n";
static string GET_RL_Host = "GET /hello.txt HTTP/1.1\r\nHost: localhost\r\n";
static string DEL_RL_Host = "DELETE /hello.txt HTTP/1.1\r\nHost: localhost\r\n";
static string POST_RL_Host = "POST /hello HTTP/1.1\r\nHost: localhost\r\n";

static NginxConfig config("/Users/mjiam/Desktop/42/webserv/foodserv/config_files/default.conf");

// Helper function used by ValidHeaders test to construct and call
// HeaderFieldValidator on passed request string. Returns result of
// HeaderFieldValidator::Process().
static int	ConstructAndProcess(string request_str) {
	Request request(&config);
	request.Parse(request_str.c_str());
	HeaderFieldValidator header_validator;

	return header_validator.Process(&config, request);
}

TEST(RequestHeaderValidatorTest, ValidHeaders) {
	int status = -1;
	status = ConstructAndProcess(GET_RL_Host + "Expect: 100-continue\n\n");
	EXPECT_EQ(status, hv_Done);

	status = ConstructAndProcess(GET_RL_Host + "Content-Length: 0\n\n");
	EXPECT_EQ(status, hv_Done);

	status = ConstructAndProcess(DEL_RL_Host + "Content-Length: 0\n\n");
	EXPECT_EQ(status, hv_Done);

	status = ConstructAndProcess(POST_RL_Host + "Transfer-Encoding: chunked\n\n0\r\n\n");
	EXPECT_EQ(status, hv_MessageChunked);

	status = ConstructAndProcess(POST_RL_Host + "Content-Length: 5\n\nHello");
	EXPECT_EQ(status, hv_MessageExpected);
}

TEST(RequestHeaderValidatorTest, InvalidHost) {
	// multiple Hosts
	EXPECT_THROW({
		string req_str = GET_RL + "Host: localhost, www.example.net\n\n";
		Request request(&config);
	request.Parse(req_str.c_str());
		HeaderFieldValidator header_validator;

		header_validator.Process(&config, request);
	}, BadRequestException);
	// missing Host
	EXPECT_THROW({
		string req_str = GET_RL + "Expect: 100-continue\n\n";
		Request request(&config);
	request.Parse(req_str.c_str());
		HeaderFieldValidator header_validator;

		header_validator.Process(&config, request);
	}, BadRequestException);
	EXPECT_THROW({
		string req_str = GET_RL + "Host: \n\n";
		Request request(&config);
	request.Parse(req_str.c_str());
		HeaderFieldValidator header_validator;

		header_validator.Process(&config, request);
	}, BadRequestException);
	// bad Host path
	EXPECT_THROW({
		string req_str = GET_RL + "Host: /example.com\n\n";
		Request request(&config);
	request.Parse(req_str.c_str());
		HeaderFieldValidator header_validator;

		header_validator.Process(&config, request);
	}, BadRequestException);
}

TEST(RequestHeaderValidatorTest, InvalidExpect) {
	EXPECT_THROW({
		string req_str = GET_RL_Host + "Expect: 101-continue\n\n";
		Request request(&config);
	request.Parse(req_str.c_str());
		HeaderFieldValidator header_validator;

		header_validator.Process(&config, request);
	}, ExpectationFailedTypeException);
}

TEST(RequestHeaderValidatorTest, InvalidContentEncoding) {
	EXPECT_THROW({
		string req_str = POST_RL_Host + "Content-Encoding: gzip\n\n";
		Request request(&config);
	request.Parse(req_str.c_str());
		HeaderFieldValidator header_validator;

		header_validator.Process(&config, request);
	}, UnsupportedMediaTypeException);
	EXPECT_THROW({
		string req_str = POST_RL_Host + "Content-Encoding: \n\n";
		Request request(&config);
	request.Parse(req_str.c_str());
		HeaderFieldValidator header_validator;

		header_validator.Process(&config, request);
	}, UnsupportedMediaTypeException);
}

TEST(RequestHeaderValidatorTest, InvalidTransferEncoding) {
	// not-implemented transfer encoding format
	EXPECT_THROW({
		string req_str = POST_RL_Host + "Transfer-Encoding: compress\n\n";
		Request request(&config);
	request.Parse(req_str.c_str());
		HeaderFieldValidator header_validator;

		header_validator.Process(&config, request);
	}, NotImplementedException);
	// not-implemented transfer encoding format
	EXPECT_THROW({
		string req_str = POST_RL_Host + "Transfer-Encoding: compress, chunked\n\n";
		Request request(&config);
	request.Parse(req_str.c_str());
		HeaderFieldValidator header_validator;

		header_validator.Process(&config, request);
	}, NotImplementedException);
	// T-E definition for not-allowed method
	EXPECT_THROW({
		string req_str = GET_RL_Host + "Transfer-Encoding: chunked\n\n";
		Request request(&config);
	request.Parse(req_str.c_str());
		HeaderFieldValidator header_validator;

		header_validator.Process(&config, request);
	}, BadRequestException);
	// Content-Length also defined
	EXPECT_THROW({
		string req_str = POST_RL_Host + "Transfer-Encoding: chunked\nContent-Length: 42\n\n";
		Request request(&config);
	request.Parse(req_str.c_str());
		HeaderFieldValidator header_validator;

		header_validator.Process(&config, request);
	}, BadRequestException);
}

TEST(RequestHeaderValidatorTest, InvalidContentLength) {
	// C-L definition for not-allowed method
	EXPECT_THROW({
		string req_str = GET_RL_Host + "Content-Length: 42\n\n";
		Request request(&config);
	request.Parse(req_str.c_str());
		HeaderFieldValidator header_validator;

		header_validator.Process(&config, request);
	}, BadRequestException);
	// invalid value (lower limit)
	EXPECT_THROW({
		string req_str = POST_RL_Host + "Content-Length: -42\n\n";
		Request request(&config);
	request.Parse(req_str.c_str());
		HeaderFieldValidator header_validator;

		header_validator.Process(&config, request);
	}, BadRequestException);
	// invalid value (upper limit)
	EXPECT_THROW({
		string req_str = POST_RL_Host + "Content-Length: 1048577\n\n";
		Request request(&config);
	request.Parse(req_str.c_str());
		HeaderFieldValidator header_validator;

		header_validator.Process(&config, request);
	}, PayloadTooLargeException);
	// invalid value
	EXPECT_THROW({
		string req_str = POST_RL_Host + "Content-Length: 42a\n\n";
		Request request(&config);
	request.Parse(req_str.c_str());
		HeaderFieldValidator header_validator;

		header_validator.Process(&config, request);
	}, BadRequestException);
	// multiple different values
	EXPECT_THROW({
		string req_str = POST_RL_Host + "Content-Length: 42, 53\n\n";
		Request request(&config);
	request.Parse(req_str.c_str());
		HeaderFieldValidator header_validator;

		header_validator.Process(&config, request);
	}, BadRequestException);
	// multiple identical values
	EXPECT_THROW({
		string req_str = POST_RL_Host + "Content-Length: 42, 42\n\n";
		Request request(&config);
	request.Parse(req_str.c_str());
		HeaderFieldValidator header_validator;

		header_validator.Process(&config, request);
	}, BadRequestException);
	// Transfer-Encoding also defined
	EXPECT_THROW({
		string req_str = POST_RL_Host + "Transfer-Encoding: chunked\nContent-Length: 42\n\n";
		Request request(&config);
	request.Parse(req_str.c_str());
		HeaderFieldValidator header_validator;

		header_validator.Process(&config, request);
	}, BadRequestException);
}
