#ifndef PROXYTHING_CLIENT_CONNECTION_H
#define PROXYTHING_CLIENT_CONNECTION_H

#include <boost/asio.hpp>
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
		client_connection(io_service &service, proxy_server &server);
		
		virtual ~client_connection();
		
		
		/**
		 * Call when a connection has been established.
		 */
		void connected();
		
		
		/// Returns the IO service
		inline io_service& service() { return m_service; }
		
		/// Returns the underlying socket
		inline ip::tcp::socket& socket() { return m_socket; }
		
		/// Returns the parent server
		inline proxy_server& server() { return m_server; }
		
	protected:
		/**
		 * Read and execute a command from the socket.
		 * 
		 * Calls itself after receiving a command, until EOF or an error occurs.
		 */
		void read_command();
		
		proxy_server &m_server;					///< Parent server
		io_service &m_service;					///< IO Service
		ip::tcp::socket m_socket;				///< Socket
		
		streambuf m_client_buffer;				///< Buffer for client commands
	};
}

#endif
