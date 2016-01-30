#ifndef PROXYTHING_REMOTE_CONNECTION_H
#define PROXYTHING_REMOTE_CONNECTION_H

#include <boost/asio.hpp>
#include <memory>

namespace proxything
{
	using namespace boost::asio;
	
	class client_connection;
	
	/**
	 * A remote connection on a client's behalf.
	 */
	class remote_connection : public std::enable_shared_from_this<remote_connection>
	{
	public:
		/**
		 * Constructs a remote connection.
		 */
		remote_connection(io_service &service, std::shared_ptr<client_connection> client);
		
		virtual ~remote_connection();
		
		
		
		/**
		 * Call when a connection has been established.
		 */
		void connected();
		
		
		
		/// Returns the IO service
		inline io_service& service() { return m_service; }
		
		/// Returns the underlying socket
		inline ip::tcp::socket& socket() { return m_socket; }
		
		/// Returns the parent client
		inline std::shared_ptr<client_connection> client() { return m_client; }
		
	protected:
		/**
		 * Reads and delivers a chunk of data.
		 * 
		 * Calls itself upon completion, until EOF or an error occurs.
		 */
		void read_and_deliver();
		
		
		
		io_service &m_service;							///< IO Service
		ip::tcp::socket m_socket;						///< Socket
		
		std::shared_ptr<client_connection> m_client;	///< Parent connection
		
		streambuf m_buf;								///< Buffer
	};
}

#endif
