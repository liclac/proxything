#ifndef PROXYTHING_CACHE_MANAGER_H
#define PROXYTHING_CACHE_MANAGER_H

#include <proxything/fs_entry.h>
#include <boost/asio.hpp>
#include <boost/filesystem.hpp>
#include <boost/algorithm/string.hpp>
#include <sstream>

namespace proxything
{
	using namespace boost::asio;
	namespace fs = boost::filesystem;
	
	/**
	 * Cache manager.
	 */
	class cache_manager
	{
	public:
		/**
		 * Callback type for lookups.
		 */
		typedef std::function<void(bool hit, const boost::system::error_code &ec, std::shared_ptr<fs_entry> f)> LookupHandler;
		
		/**
		 * Constructs a cache manager.
		 */
		cache_manager(io_service &service):
			m_service(service), m_path(fs::temp_directory_path())
		{
			
		}
		
		virtual ~cache_manager() { }
		
		/**
		 * Returns a cache filename for the given endpoint.
		 * 
		 * @param  endpoint Endpoint
		 */
		std::string filename_for(const ip::tcp::endpoint &endpoint)
		{
			std::string address_str = endpoint.address().to_string();
			boost::replace_all(address_str, ".", "-");
			boost::replace_all(address_str, ":", "-");
			
			std::stringstream ss;
			ss << "proxything_cache_" << address_str << "_" << endpoint.port();
			std::string filename = ss.str();
			
			return filename;
		}
		
		/**
		 * Look up a file in the cache.
		 * 
		 * If the file exists, the callback is called with hit=true and a file
		 * opened for reading. Otherwise, it's called with hit=false and a file
		 * opened for atomic writing.
		 * 
		 * The filename to open is determined by path() and filename_for().
		 * 
		 * @param endpoint Endpoint to open the cache for
		 * @param cb       Callback
		 */
		void async_lookup(const ip::tcp::endpoint &endpoint, LookupHandler cb)
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
		
		/// Returns the path to the cache files.
		const fs::path& path() const { return m_path; }
		
	protected:
		io_service &m_service;	///< IO Service
		
		fs::path m_path;		///< Path for cache files
	};
}

#endif
