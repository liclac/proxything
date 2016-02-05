#include <proxything/remote_connection.h>
#include <proxything/client_connection.h>
#include <proxything/fs_entry.h>
#include <proxything/config.h>
#include <boost/log/trivial.hpp>

using namespace proxything;

remote_connection::remote_connection(asio::io_service &service, std::shared_ptr<client_connection> client, std::shared_ptr<fs_entry> cache_file):
	m_service(service), m_socket(m_service), m_client(client),
	m_cache_file(cache_file), m_buf(PROXYTHING_REMOTE_BUFFER_SIZE)
{
	BOOST_LOG_TRIVIAL(trace) << "Remote Connection created";
}

remote_connection::~remote_connection()
{
	BOOST_LOG_TRIVIAL(trace) << "Remote Connection destroyed";
	
	// Pass a surrogate pointer to m_cache_file to the lambda to keep the file
	// in existence as it's being closed; shared_from_this() would work too,
	// but would keep the entire connection object in memory for no good reason
	auto cache_file_ptr = m_cache_file;
	m_cache_file->async_close([cache_file_ptr](const boost::system::error_code &ec) {
		if (ec) {
			BOOST_LOG_TRIVIAL(warning) << "Failed to commit cache: " << ec;
		} else {
			BOOST_LOG_TRIVIAL(trace) << "Cache committed";
		}
	});
}

void remote_connection::connected()
{
	BOOST_LOG_TRIVIAL(trace) << "Remote connection acknowledged";
	
	read_and_deliver();
}

void remote_connection::read_and_deliver()
{
	BOOST_LOG_TRIVIAL(debug) << "Reading from remote...";
	
	auto self = shared_from_this();
	
	async_read(m_socket, m_buf, [this, self](const boost::system::error_code &ec, std::size_t size) {
		if (ec) {
			if (ec == asio::error::eof) {
				BOOST_LOG_TRIVIAL(debug) << "Remote connection closed";
			} else {
				BOOST_LOG_TRIVIAL(error) << "Couldn't read from remote: " << ec;
			}
		}
		
		BOOST_LOG_TRIVIAL(trace) << "Received " << size << " bytes";
		BOOST_LOG_TRIVIAL(trace) << "Buffer size: " << m_buf.size();
		
		if (size) {
			auto cache_buf = std::make_shared<asio::streambuf>(size);
			buffer_copy(cache_buf->prepare(size), m_buf.data(), size);
			cache_buf->commit(size);
			async_write(*m_cache_file, *cache_buf, [this, cache_buf](const boost::system::error_code &ec, std::size_t size) {
				if (ec) {
					BOOST_LOG_TRIVIAL(warning) << "Couldn't write to " << m_cache_file->filename() << ": " << ec;
				} else {
					BOOST_LOG_TRIVIAL(trace) << "Cached " << size << " bytes";
				}
			});
			
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
