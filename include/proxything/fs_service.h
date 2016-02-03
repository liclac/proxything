#ifndef PROXYTHING_FS_SERVICE_H
#define PROXYTHING_FS_SERVICE_H

#include <proxything/impl/fs_service_threaded.h>
#include <proxything/util.h>
#include <boost/asio.hpp>
#include <thread>
#include <memory>

namespace proxything
{
	using namespace boost::asio;
	
	/**
	 * Service for asynchronous filesystem access.
	 * 
	 * This is a very barebones implementation, but it has the basic design
	 * down and is sufficient for our purposes.
	 */
	class fs_service : public io_service::service
	{
		/**
		 * The concrete implementation used.
		 * 
		 * Currently, only a thread-based implementation is provided, but it
		 * would be trivial to add implementations using AIO or Windows OVERLAP
		 * instead.
		 */
		typedef impl::fs_service_threaded<std::thread> impl_type;
		
		/// An instance of impl_type
		impl_type m_impl;
		
	public:
		/// Service ID
		static boost::asio::io_service::id id;
		
		/**
		 * Implementation-defined type for IO objects.
		 * 
		 * @see proxything::fs_entry
		 */
		typedef impl_type::implementation_type implementation_type;
		
		/**
		 * Callback type for opening a file.
		 * 
		 * @param ec Error code
		 */
		typedef std::function<void(const boost::system::error_code &ec)> OpenHandler;
		
		/**
		 * Constructor.
		 * 
		 * @param  service Parent IO service
		 */
		explicit fs_service(io_service &service):
			io_service::service(service) { }
		
		virtual ~fs_service() { };
		
		/**
		 * Constructs a new entry.
		 * 
		 * @param impl Implementation
		 */
		void construct(implementation_type& impl)
		{
			m_impl.construct(impl);
		}
		
		/**
		 * Destroy an entry.
		 * 
		 * @param impl Implementation
		 */
		void destroy(implementation_type& impl)
		{
			m_impl.destroy(impl);
		}
		
		/**
		 * Asynchronously opens a file.
		 * 
		 * @param impl     Implementation
		 * @param filename Filename
		 * @param cb       Callback
		 */
		void async_open(implementation_type& impl, const std::string &filename, OpenHandler cb)
		{
			m_impl.async_open(impl, filename, util::work_bound(get_io_service(), cb));
		}
		
	protected:
		/**
		 * Free all user handlers.
		 */
		void shutdown_service() { };
	};
}

#endif
