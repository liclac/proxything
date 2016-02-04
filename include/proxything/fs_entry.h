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
			get_service().async_open(get_implementation(), filename, mode, false, cb);
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
		 * Asynchronously opens a file for atomic writes.
		 * 
		 * When using atomic writes, an empty file will be opened, which is
		 * then moved over the destination either by async_close() or by the
		 * destruction of this object.
		 * 
		 * For performance reasons, you cannot atomically open a file for
		 * reading. This could be supported by simply copying the file
		 * currently at the destination to the temporary location. This would,
		 * however, add needless overhead for our use cases.
		 * 
		 * @param filename Filename to open
		 * @param mode     Open mode
		 * @param cb       Callback
		 */
		void async_open_atomic(const std::string &filename, std::ios_base::openmode mode, fs_service::OpenHandler cb)
		{
			get_service().async_open(get_implementation(), filename, mode, true, cb);
		}
		
		/**
		 * Asynchronously opens a file writing, using atomic writes.
		 * 
		 * @param filename Filename to open
		 * @param cb       Callback
		 */
		void async_open_atomic(const std::string &filename, fs_service::OpenHandler cb)
		{
			async_open_atomic(filename, std::ios_base::out|std::ios_base::trunc, cb);
		}
		
		/**
		 * Asynchronously closes the file.
		 * 
		 * @param cb Callback
		 */
		void async_close(fs_service::CloseHandler cb = nullptr)
		{
			get_service().async_close(get_implementation(), cb);
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
		
		/**
		 * Asynchronously writes some data.
		 * 
		 * @tparam BufsT   Constant buffer sequence type
		 * @param  buffers Buffers to read from
		 * @param  cb      Callback
		 */
		template<typename BufsT>
		void async_write_some(const BufsT &buffers, fs_service::WriteHandler cb)
		{
			get_service().async_write_some(get_implementation(), buffers, cb);
		}
	};
}

#endif
