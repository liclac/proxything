#include <proxything/file_responder.h>
#include <proxything/client_connection.h>
#include <proxything/fs_entry.h>
#include <proxything/config.h>
#include <boost/log/trivial.hpp>

using namespace proxything;

file_responder::file_responder(asio::io_service &service, std::shared_ptr<client_connection> client, std::shared_ptr<fs_entry> file):
	m_service(service), m_client(client), m_file(file),
	m_buf(PROXYTHING_FILE_BUFFER_SIZE)
{
	
}

file_responder::~file_responder()
{
	
}

void file_responder::start()
{
	BOOST_LOG_TRIVIAL(trace) << "Serving file: " << m_file->filename();
	
	read_and_deliver();
}

void file_responder::read_and_deliver()
{
	BOOST_LOG_TRIVIAL(debug) << "Reading from file...";
	
	auto self = shared_from_this();
	
	async_read(*m_file, m_buf, [this, self](const boost::system::error_code &ec, std::size_t size) {
		if (ec) {
			if (ec == asio::error::eof) {
				BOOST_LOG_TRIVIAL(debug) << "Hit the end of the file";
			} else {
				BOOST_LOG_TRIVIAL(error) << "Couldn't read from remote: " << ec;
			}
		}
		
		BOOST_LOG_TRIVIAL(trace) << "Read " << size << " bytes";
		BOOST_LOG_TRIVIAL(trace) << "Buffer size: " << m_buf.size();
		
		if (size) {
			async_write(m_client->socket(), m_buf, [this, self](const boost::system::error_code &ec, std::size_t size) {
				if (ec) {
					if (ec == asio::error::eof) {
						BOOST_LOG_TRIVIAL(debug) << "Client connection closed during write";
					} else {
						BOOST_LOG_TRIVIAL(error) << "Couldn't write response: " << ec;
					}
				}
				
				BOOST_LOG_TRIVIAL(trace) << "Sent " << size << " bytes";
				BOOST_LOG_TRIVIAL(trace) << "Buffer size: " << m_buf.size();
				
				if (!ec) {
					read_and_deliver();
				}
			});
		} else if (!ec) {
			read_and_deliver();
		}
	});
}
