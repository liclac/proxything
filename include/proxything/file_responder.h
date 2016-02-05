#ifndef PROXYTHING_FILE_RESPONDER_H
#define PROXYTHING_FILE_RESPONDER_H

#include <boost/asio.hpp>
#include <memory>

namespace proxything
{
	namespace asio = boost::asio;
	
	class client_connection;
	class fs_entry;
	
	/**
	 * Sends the contents of a file to a client.
	 */
	class file_responder : public std::enable_shared_from_this<file_responder>
	{
	public:
		/**
		 * Constructs a file responder.
		 */
		file_responder(asio::io_service &service, std::shared_ptr<client_connection> client, std::shared_ptr<fs_entry> file);
		
		virtual ~file_responder();
		
		
		
		/**
		 * Start sending the file.
		 */
		void start();
		
		
		
		/// Returns the IO service
		inline asio::io_service& service() { return m_service; }
		
		/// Returns the parent client
		inline std::shared_ptr<client_connection> client() { return m_client; }
		
		/// Returns the file handle
		inline std::shared_ptr<fs_entry> file() { return m_file; }
		
	protected:
		/**
		 * Read and deliver a chunk of data.
		 * 
		 * Calls itself upon completion, until EOF or an error occurs.
		 */
		void read_and_deliver();
		
		
		
		asio::io_service &m_service;							///< IO Service
		std::shared_ptr<client_connection> m_client;	///< Parent connection
		std::shared_ptr<fs_entry> m_file;				///< File handle
		
		asio::streambuf m_buf;								///< Buffer
	};
}

#endif
