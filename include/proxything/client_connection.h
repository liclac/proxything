#ifndef PROXYTHING_CLIENT_CONNECTION_H
#define PROXYTHING_CLIENT_CONNECTION_H

#include <proxything/cache_manager.h>
#include <boost/asio.hpp>
#include <string>
#include <memory>

namespace proxything
{
	using namespace boost::asio;
	
	class proxy_server;
	
	/**
	 * A connection from a client.
	 */
	class client_connection : public std::enable_shared_from_this<client_connection>
	{
	public:
		/**
		 * Constructs a client connection.
		 */
		client_connection(io_service &service, std::shared_ptr<proxy_server> server);
		
		virtual ~client_connection();
		
		
		/**
		 * Call when a connection has been established.
		 */
		void connected();
		
		
		
		/**
		 * Parses a command into an endpoint.
		 * 
		 * @param  cmd  The received line
		 * @return      A parsed endpoint
		 * 
		 * @throws std::invalid_argument Invalid command
		 */
		ip::tcp::endpoint parse(const std::string &cmd) const;
		
		/**
		 * Fetches remote data from the specified host.
		 * 
		 * @param endpoint   Endpoint to connect to
		 * @param cache_file Cache file to write to
		 * @see proxything::remote_connection
		 */
		void connect_remote(ip::tcp::endpoint endpoint, std::shared_ptr<fs_entry> cache_file);
		
		
		
		/// Returns the IO service
		inline io_service& service() { return m_service; }
		
		/// Returns the underlying socket
		inline ip::tcp::socket& socket() { return m_socket; }
		
		/// Returns the parent server
		inline std::shared_ptr<proxy_server> server() { return m_server; }
		
	protected:
		/**
		 * Read and execute a command from the socket.
		 * 
		 * Calls itself after receiving a command, until EOF or an error occurs.
		 */
		void read_command();
		
		io_service &m_service;					///< IO Service
		ip::tcp::socket m_socket;				///< Socket
		
		std::shared_ptr<proxy_server> m_server;	///< Parent server
		cache_manager m_cache;					///< Cache manager
		
		streambuf m_buf;						///< Buffer for client commands
	};
}

#endif
