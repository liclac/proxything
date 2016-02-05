#ifndef PROXYTHING_REMOTE_CONNECTION_H
#define PROXYTHING_REMOTE_CONNECTION_H

#include <boost/asio.hpp>
#include <memory>

namespace proxything
{
	namespace asio = boost::asio;
	
	class client_connection;
	class fs_entry;
	
	/**
	 * A remote connection on a client's behalf.
	 */
	class remote_connection : public std::enable_shared_from_this<remote_connection>
	{
	public:
		/**
		 * Constructs a remote connection.
		 */
		remote_connection(asio::io_service &service, std::shared_ptr<client_connection> client, std::shared_ptr<fs_entry> cache_file);
		
		virtual ~remote_connection();
		
		
		
		/**
		 * Call when a connection has been established.
		 */
		void connected();
		
		
		
		/// Returns the IO service
		inline asio::io_service& service() { return m_service; }
		
		/// Returns the underlying socket
		inline asio::ip::tcp::socket& socket() { return m_socket; }
		
		/// Returns the parent client
		inline std::shared_ptr<client_connection> client() { return m_client; }
		
		/// Returns the cache file handle
		inline std::shared_ptr<fs_entry> cache_file() { return m_cache_file; }
		
	protected:
		/**
		 * Reads and delivers a chunk of data.
		 * 
		 * Calls itself upon completion, until EOF or an error occurs.
		 */
		void read_and_deliver();
		
		
		
		asio::io_service &m_service;							///< IO Service
		asio::ip::tcp::socket m_socket;						///< Socket
		
		std::shared_ptr<client_connection> m_client;	///< Parent connection
		std::shared_ptr<fs_entry> m_cache_file;			///< Cache file handle
		
		asio::streambuf m_buf;								///< Buffer
	};
}

#endif
