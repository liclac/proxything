#ifndef PROXYTHING_IMPL_FS_SERVICE_THREADED_H
#define PROXYTHING_IMPL_FS_SERVICE_THREADED_H

#include <proxything/util.h>
#include <boost/asio.hpp>
#include <boost/bind.hpp>
#include <boost/filesystem.hpp>
#include <fstream>
#include <mutex>

namespace proxything
{
	using namespace boost::asio;
	namespace fs = boost::filesystem;
	
	namespace impl
	{
		/**
		 * Threaded implementation of fs_service.
		 * 
		 * This uses a worker thread an an io_service to queue up synchronous
		 * IO operations to be performed asynchronously.
		 * 
		 * @tparam ThreadT Thread type to use (std::thread or boost::thread)
		 */
		template<typename ThreadT>
		class fs_service_threaded
		{
		public:
			/**
			 * Implementation for IO Objects.
			 */
			struct implementation_type
			{
				/// File stream connected to the underlying file
				std::fstream stream;
				
				/// Filename
				std::string filename;
				
				/// Is the file opened for atomic writes?
				bool atomic = false;
				
				/// Is the atomic operation finished?
				bool atomic_finished = false;
				
				/// Temporary filename for atomic writes
				std::string temp_filename;
				
				/// Mutex used for finalization
				std::mutex mutex;
			};
			
			/**
			 * Constructor.
			 */
			fs_service_threaded():
				m_iservice(), m_iwork(new io_service::work(m_iservice)),
				m_thread(boost::bind(&io_service::run, &m_iservice))
			{
				
			}
			
			/**
			 * Destructor.
			 */
			virtual ~fs_service_threaded()
			{
				// Delete the work object to let the IO Service shut down once
				// it runs out of work
				delete m_iwork;
				
				// Wait for it to do so
				m_thread.join();
			}
			
			/**
			 * Implementation for fs_service::construct().
			 */
			void construct(io_service &service, implementation_type &impl)
			{
				
			}
			
			/**
			 * Implementation for fs_service::destroy().
			 */
			void destroy(io_service &service, implementation_type &impl)
			{
				finalize(impl);
			}
			
			/**
			 * Implementation for fs_service::async_open().
			 */
			void async_open(io_service &service, implementation_type &impl, const std::string &filename, std::ios_base::openmode mode, bool atomic, std::function<void(const boost::system::error_code &ec)> cb)
			{
				m_iservice.post([=, &service, &impl]{
					boost::system::error_code ec;
					
					impl.filename = filename;
					impl.atomic = atomic;
					
					if (!atomic) {
						impl.stream.open(filename, mode);
					} else {
						impl.temp_filename = util::tmp_path();
						impl.stream.open(impl.temp_filename, mode);
					}
					
					if (!impl.stream) {
						ec = boost::system::error_code(errno, boost::system::get_generic_category());
					}
					
					service.dispatch(boost::bind(cb, ec));
				});
			}
			
			/**
			 * Implementation for fs_service::async_close().
			 */
			void async_close(io_service &service, implementation_type &impl, std::function<void(const boost::system::error_code &ec)> cb)
			{
				m_iservice.post([=, &service, &impl]{
					boost::system::error_code ec;
					finalize(impl, ec);
					
					service.dispatch(boost::bind(cb, ec));
				});
			}
			
			/**
			 * Implementation for fs_service::async_read_some().
			 */
			template<typename BufsT>
			void async_read_some(io_service &service, implementation_type &impl, const BufsT &buffers, std::function<void(const boost::system::error_code &ec, std::size_t size)> cb)
			{
				m_iservice.post([=, &service, &impl]{
					boost::system::error_code ec;
					std::size_t size = 0;
					
					for (auto it = buffers_begin(buffers); it != buffers_end(buffers); ++it) {
						char val;
						impl.stream.get(val);
						
						if (impl.stream.eof()) {
							ec = error::eof;
							break;
						} else if (!impl.stream) {
							ec = boost::system::error_code(errno, boost::system::get_generic_category());
							break;
						}
						
						*it = val;
						++size;
					}
					
					service.dispatch(boost::bind(cb, ec, size));
				});
			}
			
			/**
			 * Implementaiton for fs_service::async_write_some().
			 */
			template<typename BufsT>
			void async_write_some(io_service &service, implementation_type &impl, const BufsT &buffers, std::function<void(const boost::system::error_code &ec, std::size_t size)> cb)
			{
				m_iservice.post([=, &service, &impl]{
					boost::system::error_code ec;
					std::size_t size = 0;
					
					impl.stream.clear();
					for (auto it = buffers_begin(buffers); it != buffers_end(buffers); ++it) {
						impl.stream.put(*it);
						if (!impl.stream) {
							ec = boost::system::error_code(errno, boost::system::get_generic_category());
							break;
						}
						++size;
					}
					
					service.dispatch(boost::bind(cb, ec, size));
				});
			}
			
		protected:
			/**
			 * Finalizes a file.
			 * 
			 * This will close the file stream, and move the file to its designated location.
			 */
			void finalize(implementation_type &impl)
			{
				std::lock_guard<std::mutex> lock(impl.mutex);
				
				impl.stream.close();
				
				if (impl.atomic && !impl.atomic_finished) {
					fs::rename(impl.temp_filename, impl.filename);
					impl.atomic_finished = true;
				}
			}
			
			/**
			 * Finalizes a file.
			 * 
			 * This version will populate ec instead of throwing an exception
			 * if the operation fails.
			 */
			void finalize(implementation_type &impl, boost::system::error_code &ec)
			{
				try {
					finalize(impl);
				} catch(boost::system::system_error &e) {
					ec = e.code();
				}
			}
			
			io_service m_iservice;		///< Internal IO service
			io_service::work *m_iwork;	///< Keeping the service alive
			ThreadT m_thread; 			///< Worker thread
		};
	}
}

#endif
