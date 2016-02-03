#ifndef PROXYTHING_UTIL_H
#define PROXYTHING_UTIL_H

#include <boost/asio.hpp>
#include <boost/filesystem.hpp>
#include <stdexcept>
#include <sstream>
#include <functional>
#include <memory>

namespace proxything
{
	namespace util
	{
		using namespace boost::asio;
		namespace fs = boost::filesystem;
		
		/**
		 * Wrapper for a work-bound callback.
		 * 
		 * A work-bound callback is bound to an io_service::work instance, and
		 * thus prevents the io_service from terminating before its associated
		 * task is completed.
		 */
		template<typename R, typename... Args>
		std::function<R(Args...)> work_bound(io_service& service, std::function<R(Args...)> fn)
		{
			auto work = std::make_shared<io_service::work>(service);
			return [work, fn](Args&&... args) {
				return fn(std::forward<Args>(args)...);
			};
		}
		
		/**
		 * A "smart pointer" type thing for a temporary file.
		 */
		class tmp_file
		{
		public:
			/**
			 * Creates a temporary file.
			 */
			tmp_file(): m_path((fs::temp_directory_path() / fs::unique_path()).string())
			{
				std::ofstream stream(m_path);
				if (!stream) {
					std::stringstream ss;
					ss << "Couldn't open: " << m_path << " (" << errno << ")";
					throw std::runtime_error(ss.str());
				}
				stream << "Lorem ipsum dolor sit amet" << std::endl;
			}
			
			virtual ~tmp_file()
			{
				fs::remove(m_path);
			}
			
			/**
			 * Returns the path to the file.
			 */
			inline const std::string& path() { return m_path; }
			
		protected:
			/// Path to the file
			std::string m_path;
		};
	}
}

#endif
