#ifndef PROXYTHING_SERVER_H
#define PROXYTHING_SERVER_H

#include <boost/asio.hpp>
#include <boost/program_options.hpp>

namespace proxything
{
	using namespace boost::asio;
	namespace po = boost::program_options;
	
	/**
	 * A TCP server.
	 */
	class server
	{
	public:
		/**
		 * Constructs a server.
		 * 
		 * @param service IO Service to run on
		 * @param config  Configuration
		 */
		server(io_service &service, const po::variables_map &config);
		
		/// Destructor
		virtual ~server();
		
	protected:
		/**
		 * Accepts a single connection.
		 */
		void accept();
		
		io_service &m_service;				///< IO Service
		ip::tcp::acceptor m_acceptor;		///< Acceptor for new connections
		
		/// Configuration
		const po::variables_map &m_config;
	};
}

#endif
