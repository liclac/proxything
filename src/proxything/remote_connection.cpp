#include <proxything/remote_connection.h>
#include <proxything/client_connection.h>
#include <proxything/config.h>
#include <boost/log/trivial.hpp>

using namespace proxything;
using namespace boost::asio;

remote_connection::remote_connection(io_service &service, std::shared_ptr<client_connection> client):
	m_service(service), m_socket(m_service), m_client(client),
	m_buf(PROXYTHING_REMOTE_BUFFER_SIZE)
{
	BOOST_LOG_TRIVIAL(trace) << "Remote Connection created";
}

remote_connection::~remote_connection()
{
	BOOST_LOG_TRIVIAL(trace) << "Remote Connection destroyed";
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
			if (ec == error::eof) {
				BOOST_LOG_TRIVIAL(debug) << "Remote connection closed";
			} else {
				BOOST_LOG_TRIVIAL(error) << "Couldn't read from remote: " << ec;
			}
		}
		
		BOOST_LOG_TRIVIAL(trace) << "Received " << size << " bytes";
		BOOST_LOG_TRIVIAL(trace) << "Buffer size: " << m_buf.size();
		
		if (size) {
			async_write(m_client->socket(), m_buf, [this, self](const boost::system::error_code &ec, std::size_t size) {
				if (ec) {
					if (ec == error::eof) {
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
