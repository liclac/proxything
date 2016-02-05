#ifndef PROXYTHING_CACHE_MANAGER_H
#define PROXYTHING_CACHE_MANAGER_H

#include <boost/asio.hpp>
#include <boost/filesystem.hpp>
#include <memory>
#include <string>

namespace proxything
{
	namespace asio = boost::asio;
	namespace fs = boost::filesystem;
	
	class fs_entry;
	
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
		cache_manager(asio::io_service &service);
		
		virtual ~cache_manager();
		
		/**
		 * Returns a cache filename for the given endpoint.
		 * 
		 * @param  endpoint Endpoint
		 */
		std::string filename_for(const asio::ip::tcp::endpoint &endpoint);
		
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
		void async_lookup(const asio::ip::tcp::endpoint &endpoint, LookupHandler cb);
		
		/// Returns the path to the cache files.
		const fs::path& path() const { return m_path; }
		
	protected:
		asio::io_service &m_service;	///< IO Service
		
		fs::path m_path;		///< Path for cache files
	};
}

#endif
