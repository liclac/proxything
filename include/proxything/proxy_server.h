#ifndef PROXYTHING_PROXY_SERVER_H
#define PROXYTHING_PROXY_SERVER_H

#include <boost/asio.hpp>
#include <boost/program_options.hpp>

namespace proxything
{
	using namespace boost::asio;
	namespace po = boost::program_options;
	
	/**
	 * A TCP server.
	 */
	class proxy_server
	{
	public:
		/**
		 * Constructs and starts a server.
		 * 
		 * @param service IO Service to run on
		 * @param host    Host to bind to
		 * @param port    Port to bind to
		 */
		proxy_server(io_service &service, const std::string &host, unsigned short port);
		
		/**
		 * Constructs a stopped server.
		 */
		proxy_server(io_service &service);
		
		virtual ~proxy_server();
		
		
		
		/**
		 * Starts the server.
		 * 
		 * @param host Host to bind to
		 * @param port Port to bind to
		 */
		void start(const std::string &host, unsigned short port);
		
		
		
		/// Returns the IO service
		inline io_service& service() { return m_service; }
		
	protected:
		/**
		 * Accepts a single connection.
		 */
		void accept();
		
		io_service &m_service;				///< IO Service
		ip::tcp::acceptor m_acceptor;		///< Acceptor for new connections
	};
}

#endif
