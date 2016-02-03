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
			void async_open(io_service &service, implementation_type &impl, const std::string &filename, std::function<void(const boost::system::error_code &ec)> cb)
			{
				m_iservice.post([=, &service, &impl]{
					impl.stream.open(filename);
					
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
			
		protected:
			io_service m_iservice;		///< Internal IO service
			io_service::work *m_iwork;	///< Keeping the service alive
			ThreadT m_thread; 			///< Worker thread
		};
	}
}

#endif
