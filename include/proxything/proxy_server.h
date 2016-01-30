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
		 * @param config  Configuration
		 */
		proxy_server(io_service &service, const po::variables_map &config);
		
		/**
		 * Constructs a stopped server.
		 */
		proxy_server(io_service &service);
		
		virtual ~proxy_server();
		
		
		
		/// Returns the IO service
		inline io_service& service() { return m_service; }
		
		/// Returns the server's configuration
		inline po::variables_map& config() { return m_config; }
		
	protected:
		/**
		 * Accepts a single connection.
		 */
		void accept();
		
		io_service &m_service;				///< IO Service
		ip::tcp::acceptor m_acceptor;		///< Acceptor for new connections
		
		/// Configuration
		po::variables_map m_config;
	};
}

#endif
