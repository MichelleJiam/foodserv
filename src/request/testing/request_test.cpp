#include <gtest/gtest.h>

#include "../uri.hpp"
#include "../uri_state_parser.hpp"
#include "../exception.hpp"

TEST(RequestTargetTest, ParsePathSimple) {
	URI	request_uri("/contact_form.php");
	EXPECT_EQ(request_uri.GetParsedURI(), "/contact_form.php");
	EXPECT_TRUE(request_uri.GetHost().empty());
	EXPECT_EQ(request_uri.GetPath(), "/contact_form.php");
	EXPECT_TRUE(request_uri.GetQuery().empty());
}

TEST(RequestTargetTest, ParseQuerySimple) {
	URI	request_uri("/where?q=now");
	EXPECT_EQ(request_uri.GetParsedURI(), "/where?q=now");
	EXPECT_TRUE(request_uri.GetHost().empty());
	EXPECT_EQ(request_uri.GetPath(), "/where");
	EXPECT_EQ(request_uri.GetQuery(), "q=now");
	request_uri = "/where?q=nowwithhash#";
	EXPECT_EQ(request_uri.GetParsedURI(), "/where?q=nowwithhash");
	EXPECT_TRUE(request_uri.GetHost().empty());
	EXPECT_EQ(request_uri.GetPath(), "/where");
	EXPECT_EQ(request_uri.GetQuery(), "q=nowwithhash");
}

TEST(RequestTargetTest, ParsePathMultiSlash) {
	URI	request_uri("/path/to/resource");
	EXPECT_EQ(request_uri.GetParsedURI(), "/path/to/resource");
	EXPECT_TRUE(request_uri.GetHost().empty());
	EXPECT_EQ(request_uri.GetPath(), "/path/to/resource");
	EXPECT_TRUE(request_uri.GetQuery().empty());
}

TEST(RequestTargetTest, ParsePathMultiQueryMulti) {
	URI	request_uri("/path/to/query?param1=value&param2=value2");
	EXPECT_EQ(request_uri.GetParsedURI(), "/path/to/query?param1=value&param2=value2");
	EXPECT_TRUE(request_uri.GetHost().empty());
	EXPECT_EQ(request_uri.GetPath(), "/path/to/query");
	EXPECT_EQ(request_uri.GetQuery(), "param1=value&param2=value2");
	request_uri = "/path/to/query?new=/google.com";
	EXPECT_EQ(request_uri.GetParsedURI(), "/path/to/query?new=/google.com");
	EXPECT_TRUE(request_uri.GetHost().empty());
	EXPECT_EQ(request_uri.GetPath(), "/path/to/query");
	EXPECT_EQ(request_uri.GetQuery(), "new=/google.com");
}

TEST(RequestTargetTest, ParsePercentEncoded) {
	URI	request_uri("/where%20/are/we?q=now?text=Hello+G%C3%BCnter");
	EXPECT_EQ(request_uri.GetParsedURI(), "/where%20/are/we?q=now?text=Hello+G%C3%BCnter");
	EXPECT_TRUE(request_uri.GetHost().empty());
	EXPECT_EQ(request_uri.GetPath(), "/where%20/are/we");
	EXPECT_EQ(request_uri.GetQuery(), "q=now?text=Hello+G%C3%BCnter");
	request_uri ="/where%c3/are/we?q=now";
	EXPECT_EQ(request_uri.GetParsedURI(), "/where%C3/are/we?q=now");
	EXPECT_TRUE(request_uri.GetHost().empty());
	EXPECT_EQ(request_uri.GetPath(), "/where%C3/are/we");
	EXPECT_EQ(request_uri.GetQuery(), "q=now");
}

TEST(RequestTargetTest, InvalidPathStart) {
	EXPECT_THROW({
		URI	request_uri("//thisisabadpath");
	}, BadRequestException);
	EXPECT_THROW({
		URI	request_uri("%25anotherbadpath");
	}, BadRequestException);
	EXPECT_THROW({
		URI	request_uri("yetanotherbadpath");
	}, BadRequestException);
}

TEST(RequestTargetTest, InvalidPathMiddle) {
	EXPECT_THROW({
		URI	request_uri("/path#/to");
	}, BadRequestException);
	EXPECT_THROW({
		URI	request_uri("/path^to");
	}, BadRequestException);
}

TEST(RequestTargetTest, InvalidPercent) {
	EXPECT_THROW({
		URI	request_uri("/path%2=to");
	}, BadRequestException);
	EXPECT_THROW({
		URI	request_uri("/path%$3to");
	}, BadRequestException);
	EXPECT_THROW({
		URI	request_uri("/path/to?q%2==1");
	}, BadRequestException);
	EXPECT_THROW({
		URI	request_uri("/path/to?q=1%*1");
	}, BadRequestException);
}

TEST(RequestTargetTest, InvalidQuery) {
	EXPECT_THROW({
		URI	request_uri("/path/to?q^=1");
	}, BadRequestException);
	EXPECT_THROW({
		URI	request_uri("/path/to?q=1^");
	}, BadRequestException);
}
