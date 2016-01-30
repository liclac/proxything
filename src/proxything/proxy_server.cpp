#include <proxything/proxy_server.h>
#include <proxything/client_connection.h>
#include <boost/log/trivial.hpp>
#include <memory>

using namespace proxything;
using namespace boost::asio;

proxy_server::proxy_server(io_service &service, const po::variables_map &config):
	m_service(service), m_config(config), m_acceptor(m_service)
{
	std::string host = m_config["host"].as<std::string>();
	unsigned short port = m_config["port"].as<unsigned short>();
	
	ip::tcp::endpoint endpoint(ip::address::from_string(host), port);
	m_acceptor.open(endpoint.protocol());
	m_acceptor.set_option(ip::tcp::acceptor::reuse_address(true));
	m_acceptor.bind(endpoint);
	m_acceptor.listen();
	
	BOOST_LOG_TRIVIAL(info) << "Listening on " << endpoint.address().to_string() << ":" << endpoint.port();
	
	accept();
}

proxy_server::~proxy_server() { }

void proxy_server::accept()
{
	BOOST_LOG_TRIVIAL(debug) << "Accepting new connection...";
	
	auto client = std::make_shared<client_connection>(m_service, *this);
	m_acceptor.async_accept(client->socket(), [=](const boost::system::error_code &ec) {
		if (ec) {
			BOOST_LOG_TRIVIAL(warning) << "Error accepting connection: " << ec;
			accept();
			return;
		}
		
		BOOST_LOG_TRIVIAL(info) << "Connection accepted!";
		client->connected();
		
		accept();
	});
}
