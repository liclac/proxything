#include <proxything/client_connection.h>
#include <proxything/remote_connection.h>
#include <proxything/file_responder.h>
#include <proxything/proxy_server.h>
#include <proxything/config.h>
#include <boost/log/trivial.hpp>
#include <sstream>
#include <iostream>

using namespace proxything;
using namespace boost::asio;

client_connection::client_connection(io_service &service, std::shared_ptr<proxy_server> server):
	m_service(service), m_socket(m_service),
	m_server(server), m_cache(m_service),
	m_buf(PROXYTHING_CLIENT_BUFFER_SIZE)
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

ip::tcp::endpoint client_connection::parse(const std::string &cmd) const
{
	// Find the : delimiting the address and port
	int colon_at = cmd.rfind(':');
	if (colon_at == std::string::npos) {
		throw std::invalid_argument("Format: IP:port");
	}
	
	// Split the command into host and port
	std::string address_s = cmd.substr(0, colon_at);
	std::string port_s = cmd.substr(colon_at + 1);
	BOOST_LOG_TRIVIAL(trace) << "Split: host=" << address_s << ", port=" << port_s;
	
	// Handle [bracketed] IPv6 addresses
	if (address_s[0] == '[' && address_s[address_s.size() - 1] == ']') {
		address_s = address_s.substr(1, address_s.size() - 2);
		BOOST_LOG_TRIVIAL(trace) << "Bracketed IPv6 address: " << address_s;
	}
	
	// Parse the address
	ip::address address;
	try {
		address = ip::address::from_string(address_s);
	} catch (boost::system::system_error &e) {
		throw std::invalid_argument("Given address is not valid");
	}
	BOOST_LOG_TRIVIAL(trace) << "Address: " << address.to_string();
	
	// Parse the port
	int port;
	try {
		port = stoi(port_s);
	} catch (std::invalid_argument) {
		throw std::invalid_argument("Port is not a valid number");
	}
	BOOST_LOG_TRIVIAL(trace) << "Port: " << port;
	
	// Validate the port range
	if (port < 1 || port > 65535) {
		throw std::invalid_argument("Valid ports are 1-65535");
	}
	
	return ip::tcp::endpoint(address, port);
}

void client_connection::connect_remote(ip::tcp::endpoint endpoint, std::shared_ptr<fs_entry> cache_file)
{
	BOOST_LOG_TRIVIAL(info) << "Connecting to remote: " << endpoint.address().to_string() << ":" << endpoint.port();
	BOOST_LOG_TRIVIAL(debug) << "Caching to: " << cache_file->filename();
	auto remote = std::make_shared<remote_connection>(m_service, shared_from_this(), cache_file);
	remote->socket().async_connect(endpoint, [=](const boost::system::error_code &ec) {
		remote->connected();
	});
}

void client_connection::serve_file(std::shared_ptr<fs_entry> file)
{
	BOOST_LOG_TRIVIAL(info) << "Serving local response";
	auto responder = std::make_shared<file_responder>(m_service, shared_from_this(), file);
	responder->start();
}

void client_connection::read_command()
{
	// Retain the connection to keep it from getting deleted mid-transaction
	auto self = shared_from_this();
	
	BOOST_LOG_TRIVIAL(trace) << "Awaiting command...";
	async_read_until(m_socket, m_buf, "\r\n", [&, self](const boost::system::error_code &ec, std::size_t size) {
		if (ec) {
			if (ec == error::eof) {
				BOOST_LOG_TRIVIAL(info) << "Connection closed";
			} else {
				BOOST_LOG_TRIVIAL(warning) << "Failed to read command: " << ec;
			}
			
			return;
		}
		
		BOOST_LOG_TRIVIAL(trace) << "Parsing command...";
		
		std::istream cmd_s(&m_buf);
		std::string cmd;
		std::getline(cmd_s, cmd);
		
		BOOST_LOG_TRIVIAL(info) << "Command received: " << cmd;
		
		ip::tcp::endpoint endpoint;
		try {
			endpoint = parse(cmd);
			m_cache.async_lookup(endpoint, [=](bool hit, const boost::system::error_code &ec, std::shared_ptr<fs_entry> f) {
				if (ec) {
					// Log the error, but try to proceed anyways; at worst, it'll cause performance
					// degradation, which is better than ceasing to function
					BOOST_LOG_TRIVIAL(error) << "Cache error: " << ec;
				}
				
				if (hit) {
					serve_file(f);
				} else {
					connect_remote(endpoint, f);
				}
			});
		} catch (std::invalid_argument &e) {
			BOOST_LOG_TRIVIAL(error) << "Invalid command: " << e.what();
			
			std::stringstream msg_s;
			msg_s << "ERROR: " << e.what() << "\r\n";
			std::string msg = msg_s.str();
			async_write(m_socket, buffer(msg), [&, self](const boost::system::error_code &ec, std::size_t size) {
				if (ec) {
					BOOST_LOG_TRIVIAL(error) << "Couldn't write error to client: " << ec;
				}
			});
		}
		
		read_command();
	});
}
