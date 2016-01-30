#ifndef PROXYTHING_CLIENT_CONNECTION_H
#define PROXYTHING_CLIENT_CONNECTION_H

#include <boost/asio.hpp>
#include <memory>

namespace proxything
{
	using namespace boost::asio;
	
	class server;
	
	/**
	 * A connection from a client.
	 */
	class client_connection : public std::enable_shared_from_this<client_connection>
	{
	public:
		/**
		 * Constructs a client connection 
		 */
		client_connection(io_service &service, server &server);
		
		virtual ~client_connection();
		
		
		
		/**
		 * Read a command from the socket.
		 */
		void read_command();
		
		
		
		inline ip::tcp::socket& socket() { return m_socket; }		///< Getter for m_socket
		
	protected:
		server &m_server;						///< Parent server
		io_service &m_service;					///< IO Service
		ip::tcp::socket m_socket;				///< Socket
		
		streambuf m_client_buffer;				///< Buffer for client commands
	};
}

#endif
