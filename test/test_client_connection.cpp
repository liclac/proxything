#include <catch.hpp>
#include <proxything/proxy_server.h>
#include <proxything/client_connection.h>
#include <stdexcept>

using namespace proxything;
using namespace boost::asio;

SCENARIO("commands can be parsed")
{
	io_service service;
	auto server = std::make_shared<proxy_server>(service);
	auto client = std::make_shared<client_connection>(service, server);
	
	GIVEN("gibberish input")
	{
		THEN("it should throw an exception")
		{
			CHECK_THROWS_AS(client->parse("invalid lol"), std::invalid_argument);
		}
	}
	
	GIVEN("an invalid address")
	{
		THEN("it should throw an exception")
		{
			CHECK_THROWS_AS(client->parse("abc123:1324"), std::invalid_argument);
		}
	}
	
	GIVEN("a non-numeric port")
	{
		THEN("it should throw an exception")
		{
			CHECK_THROWS_AS(client->parse("127.0.0.1:aaaa"), std::invalid_argument);
		}
	}
	
	GIVEN("a port out of range")
	{
		WHEN("the port is 0")
		{
			THEN("it should throw an exception")
			{
				CHECK_THROWS_AS(client->parse("127.0.0.1:0"), std::invalid_argument);
			}
		}
		
		WHEN("the port is negative")
		{
			THEN("it should throw an exception")
			{
				CHECK_THROWS_AS(client->parse("127.0.0.1:-1"), std::invalid_argument);
				CHECK_THROWS_AS(client->parse("127.0.0.1:-37"), std::invalid_argument);
			}
		}
		
		WHEN("the port is too large")
		{
			THEN("it should throw an exception")
			{
				CHECK_THROWS_AS(client->parse("127.0.0.1:65536"), std::invalid_argument);
				CHECK_THROWS_AS(client->parse("127.0.0.1:123456789"), std::invalid_argument);
			}
		}
	}
	
	GIVEN("an ipv4 address and port")
	{
		ip::tcp::endpoint ep = client->parse("127.0.0.1:1234");
		
		THEN("the address should be correct")
		{
			CHECK(ep.address().to_string() == "127.0.0.1");
		}
		
		THEN("the port should be correct")
		{
			CHECK(ep.port() == 1234);
		}
	}
	
	GIVEN("an ipv6 address and port")
	{
		ip::tcp::endpoint ep = client->parse("2001:4860:4860::8888:1234");
		
		THEN("the address should be correct")
		{
			CHECK(ep.address().to_string() == "2001:4860:4860::8888");
		}
		
		THEN("the port should be correct")
		{
			CHECK(ep.port() == 1234);
		}
	}
	
	GIVEN("a [bracketed] ipv6 address and port")
	{
		ip::tcp::endpoint ep = client->parse("[2001:4860:4860::8888]:1234");
		
		THEN("the address should be correct")
		{
			CHECK(ep.address().to_string() == "2001:4860:4860::8888");
		}
		
		THEN("the port should be correct")
		{
			CHECK(ep.port() == 1234);
		}
	}
}
