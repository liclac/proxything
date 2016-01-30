#include <proxything/client_connection.h>
#include <proxything/proxy_server.h>
#include <proxything/config.h>
#include <boost/log/trivial.hpp>
#include <iostream>

using namespace proxything;
using namespace boost::asio;

client_connection::client_connection(io_service &service, proxy_server &server):
	m_service(service), m_socket(m_service), m_server(server),
	m_client_buffer(PROXYTHING_CLIENT_COMMAND_BUFFER_SIZE)
{
	BOOST_LOG_TRIVIAL(trace) << "Client Connection created";
}

client_connection::~client_connection()
{
	BOOST_LOG_TRIVIAL(trace) << "Client Connection destroyed";
}

void client_connection::connected()
{
	BOOST_LOG_TRIVIAL(trace) << "Connection acknowledged";
	read_command();
}

void client_connection::read_command()
{
	// Retain the connection to keep it from getting deleted mid-transaction
	auto self = shared_from_this();
	
	BOOST_LOG_TRIVIAL(trace) << "Awaiting command...";
	async_read_until(m_socket, m_client_buffer, "\r\n", [&, self](const boost::system::error_code &ec, std::size_t size) {
		if (ec) {
			if (ec == error::eof) {
				BOOST_LOG_TRIVIAL(info) << "Connection closed";
			} else {
				BOOST_LOG_TRIVIAL(warning) << "Failed to read command: " << ec;
			}
			
			return;
		}
		
		BOOST_LOG_TRIVIAL(trace) << "Parsing command...";
		
		std::istream cmd_s(&m_client_buffer);
		std::string cmd;
		std::getline(cmd_s, cmd);
		
		BOOST_LOG_TRIVIAL(info) << "Command received: " << cmd;
		
		read_command();
	});
}
