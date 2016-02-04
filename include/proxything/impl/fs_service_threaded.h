#ifndef PROXYTHING_IMPL_FS_SERVICE_THREADED_H
#define PROXYTHING_IMPL_FS_SERVICE_THREADED_H

#include <boost/asio.hpp>
#include <boost/bind.hpp>
#include <fstream>

namespace proxything
{
	using namespace boost::asio;
	
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
				
			}
			
			/**
			 * Implementation for fs_service::async_open().
			 */
			void async_open(io_service &service, implementation_type &impl, const std::string &filename, std::ios_base::openmode mode, std::function<void(const boost::system::error_code &ec)> cb)
			{
				m_iservice.post([=, &service, &impl]{
					impl.stream.open(filename, mode);
					
					if (impl.stream.good()) {
						service.dispatch([=]{
							cb({});
						});
					} else {
						service.dispatch([=]{
							cb(boost::system::error_code(errno, boost::system::get_generic_category()));
						});
					}
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
			
		protected:
			io_service m_iservice;		///< Internal IO service
			io_service::work *m_iwork;	///< Keeping the service alive
			ThreadT m_thread; 			///< Worker thread
		};
	}
}

#endif
