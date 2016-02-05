#include <catch.hpp>
#include <proxything/proxy_server.h>

using namespace proxything;

SCENARIO("listening works")
{
	io_service service;
	proxy_server server(service);
	
	GIVEN("a local host and port")
	{
		std::string host = "127.0.0.1";
		unsigned short port = 12345;
		
		WHEN("you start listening")
		{
			server.listen(host, port);
			
			THEN("the acceptor should be running")
			{
				CHECK(server.acceptor().local_endpoint().address().to_string() == host);
				CHECK(server.acceptor().local_endpoint().port() == port);
			}
		}
	}
}
