#ifndef PROXYTHING_FS_ENTRY_H
#define PROXYTHING_FS_ENTRY_H

#include <proxything/fs_service.h>
#include <boost/asio.hpp>
#include <functional>
#include <ios>

namespace proxything
{
	using namespace boost::asio;
	
	/**
	 * A filesystem entry.
	 */
	class fs_entry : public basic_io_object<fs_service>
	{
	public:
		/**
		 * Constructor.
		 * 
		 * @param  service Parent IO service
		 */
		explicit fs_entry(io_service &service):
			basic_io_object<fs_service>(service) { }
		
		virtual ~fs_entry() { };
		
		/**
		 * Asynchronously opens the file.
		 * 
		 * @param filename Filename to open
		 * @param mode     Open mode
		 * @param cb       Callback
		 */
		void async_open(const std::string &filename, std::ios_base::openmode mode, fs_service::OpenHandler cb)
		{
			get_service().async_open(get_implementation(), filename, mode, cb);
		}
		
		/**
		 * Asynchronously opens the file for reading.
		 * 
		 * @param filename Filename to open
		 * @param cb       Callback
		 */
		void async_open(const std::string &filename, fs_service::OpenHandler cb)
		{
			async_open(filename, std::ios_base::in|std::ios_base::binary, cb);
		}
		
		/**
		 * Asynchronously reads some data.
		 * 
		 * @tparam BufsT   Mutable buffer type
		 * @param  buffers Buffers to read into
		 * @param  cb      Callback
		 */
		template<typename BufsT>
		void async_read_some(const BufsT &buffers, fs_service::ReadHandler cb)
		{
			get_service().async_read_some(get_implementation(), buffers, cb);
		}
	};
}

#endif
