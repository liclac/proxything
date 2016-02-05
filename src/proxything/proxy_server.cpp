#include <proxything/proxy_server.h>
#include <proxything/client_connection.h>
#include <boost/log/trivial.hpp>
#include <memory>

using namespace proxything;

proxy_server::proxy_server(asio::io_service &service):
	m_service(service), m_acceptor(m_service) { }

proxy_server::~proxy_server() { }

void proxy_server::listen(const std::string &host, unsigned short port)
{
	asio::ip::tcp::endpoint endpoint(asio::ip::address::from_string(host), port);
	m_acceptor.open(endpoint.protocol());
	m_acceptor.set_option(asio::ip::tcp::acceptor::reuse_address(true));
	m_acceptor.bind(endpoint);
	m_acceptor.listen();
	
	BOOST_LOG_TRIVIAL(info) << "Listening on " << endpoint.address().to_string() << ":" << endpoint.port();
}

void proxy_server::accept()
{
	BOOST_LOG_TRIVIAL(debug) << "Accepting new connection...";
	
	auto self = shared_from_this();
	
	auto client = std::make_shared<client_connection>(m_service, self);
	m_acceptor.async_accept(client->socket(), [&, client](const boost::system::error_code &ec) {
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
