#include <proxything/server.h>
#include <boost/log/trivial.hpp>

using namespace proxything;
using namespace boost::asio;

server::server(io_service &service, const po::variables_map &config):
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

server::~server() { }

void server::accept()
{
	BOOST_LOG_TRIVIAL(debug) << "Accepting new connection...";
	
	ip::tcp::socket socket(m_service);
	m_acceptor.async_accept(socket, [=](const boost::system::error_code &ec) {
		BOOST_LOG_TRIVIAL(info) << "-> Connection accepted!";
		accept();
	});
}
