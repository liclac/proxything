#include <proxything/fs_entry.h>
#include <proxything/cache_manager.h>
#include <boost/algorithm/string.hpp>
#include <sstream>

using namespace proxything;

cache_manager::cache_manager(asio::io_service &service):
	m_service(service), m_path(fs::temp_directory_path()) { }

cache_manager::~cache_manager() { }

std::string cache_manager::filename_for(const asio::ip::tcp::endpoint &endpoint)
{
	std::string address_str = endpoint.address().to_string();
	boost::replace_all(address_str, ".", "-");
	boost::replace_all(address_str, ":", "-");
	
	std::stringstream ss;
	ss << "proxything_cache_" << address_str << "_" << endpoint.port();
	std::string filename = ss.str();
	
	return filename;
}

void cache_manager::async_lookup(const asio::ip::tcp::endpoint &endpoint, LookupHandler cb)
{
	std::string filename = (m_path / filename_for(endpoint)).string();
	
	auto f = std::make_shared<fs_entry>(m_service);
	f->async_open(filename, [=](const boost::system::error_code &ec) {
		if (!ec) {
			cb(true, ec, f);
		} else {
			f->async_open_atomic(filename, [=](const boost::system::error_code &ec) {
				cb(false, ec, f);
			});
		}
	});
}
