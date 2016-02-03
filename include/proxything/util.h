#ifndef PROXYTHING_UTIL_H
#define PROXYTHING_UTIL_H

#include <boost/asio.hpp>
#include <functional>
#include <memory>

namespace proxything
{
	namespace util
	{
		using namespace boost::asio;
		
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
	}
}

#endif
