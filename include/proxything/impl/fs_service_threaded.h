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
	namespace asio = boost::asio;
	namespace fs = boost::filesystem;
	
	namespace impl
	{
		/**
		 * Threaded implementation of fs_service.
		 * 
		 * This uses a worker thread an an asio::io_service to queue up synchronous
		 * IO operations to be performed asynchronously. It uses strands to
		 * ensure that reads and writes are performed sequentially.
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
				std::fstream stream;		///< Underlying file stream
				std::string filename;		///< Filename
				
				bool atomic = false;		///< Are we using atomic writes?
				bool atomic_done = false;	///< Is the atomic write done?
				
				std::string temp_filename;	///< Temporary filename (atomic)
				std::mutex final_mutex;		///< Finalization mutex
				
				/// Strand to ensure sequential access
				std::unique_ptr<asio::io_service::strand> strand;
			};
			
			/**
			 * Constructor.
			 */
			fs_service_threaded():
				m_iservice(), m_iwork(new asio::io_service::work(m_iservice)),
				m_thread(boost::bind(&asio::io_service::run, &m_iservice))
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
			void construct(asio::io_service &service, implementation_type &impl)
			{
				impl.strand = std::unique_ptr<asio::io_service::strand>(new asio::io_service::strand(m_iservice));
			}
			
			/**
			 * Implementation for fs_service::destroy().
			 */
			void destroy(asio::io_service &service, implementation_type &impl)
			{
				impl.stream.close();
			}
			
			/**
			 * Implementation for fs_service::async_open().
			 */
			void async_open(asio::io_service &service, implementation_type &impl, const std::string &filename, std::ios_base::openmode mode, bool atomic, std::function<void(const boost::system::error_code &ec)> cb)
			{
				impl.strand->post([=, &service, &impl]{
					boost::system::error_code ec;
					
					impl.filename = filename;
					impl.atomic = atomic;
					
					if (impl.atomic) {
						impl.temp_filename = util::tmp_path();
					}
					
					impl.stream.open(!impl.atomic ? impl.filename : impl.temp_filename, mode);
					if (!impl.stream) {
						ec = boost::system::error_code(errno, boost::system::get_generic_category());
					}
					
					service.dispatch(boost::bind(cb, ec));
				});
			}
			
			/**
			 * Implementation for fs_service::async_close().
			 */
			void async_close(asio::io_service &service, implementation_type &impl, std::function<void(const boost::system::error_code &ec)> cb)
			{
				impl.strand->post([=, &service, &impl]{
					boost::system::error_code ec;
					
					impl.stream.close();
					
					if (impl.atomic) {
						fs::rename(impl.temp_filename, impl.filename, ec);
					}
					
					service.dispatch(boost::bind(cb, ec));
				});
			}
			
			/**
			 * Implementation for fs_service::async_read_some().
			 */
			template<typename BufsT>
			void async_read_some(asio::io_service &service, implementation_type &impl, const BufsT &buffers, std::function<void(const boost::system::error_code &ec, std::size_t size)> cb)
			{
				impl.strand->post([=, &service, &impl]{
					boost::system::error_code ec;
					std::size_t size = 0;
					
					for (auto it = buffers_begin(buffers); it != buffers_end(buffers); ++it) {
						char val;
						impl.stream.get(val);
						
						if (impl.stream.eof()) {
							ec = asio::error::eof;
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
			void async_write_some(asio::io_service &service, implementation_type &impl, const BufsT &buffers, std::function<void(const boost::system::error_code &ec, std::size_t size)> cb)
			{
				impl.strand->post([=, &service, &impl]{
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
			
			/**
			 * Implementation for fs_service::filename().
			 */
			std::string filename(const implementation_type &impl) const
			{
				return impl.filename;
			}
			
		protected:
			asio::io_service m_iservice;		///< Internal IO service
			asio::io_service::work *m_iwork;	///< Keeping the service alive
			ThreadT m_thread; 					///< Worker thread
		};
	}
}

#endif
