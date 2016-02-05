#ifndef PROXYTHING_PROXY_SERVER_H
#define PROXYTHING_PROXY_SERVER_H

#include <boost/asio.hpp>
#include <boost/program_options.hpp>
#include <memory>

namespace proxything
{
	namespace asio = boost::asio;
	namespace po = boost::program_options;
	
	/**
	 * A TCP server.
	 */
	class proxy_server : public std::enable_shared_from_this<proxy_server>
	{
	public:
		/**
		 * Constructs a stopped server.
		 */
		proxy_server(asio::io_service &service);
		
		virtual ~proxy_server();
		
		
		
		/**
		 * Listen on the specified port.
		 * 
		 * @param host Host to bind to
		 * @param port Port to bind to
		 */
		void listen(const std::string &host, unsigned short port);
		
		/**
		 * Start accepting connections.
		 */
		void accept();
		
		
		
		/// Returns the IO service
		inline asio::io_service& service() { return m_service; }
		
		/// Returns the acceptor
		inline asio::ip::tcp::acceptor& acceptor() { return m_acceptor; }
		
	protected:
		
		asio::io_service &m_service;			///< IO Service
		asio::ip::tcp::acceptor m_acceptor;		///< Acceptor for new connections
	};
}

#endif
