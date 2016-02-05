#ifndef PROXYTHING_APP_H
#define PROXYTHING_APP_H

#include <boost/asio.hpp>
#include <boost/program_options.hpp>
#include <vector>
#include <thread>
#include <memory>

namespace proxything
{
	using namespace boost::asio;
	namespace po = boost::program_options;
	
	class proxy_server;
	
	/**
	 * CLI application frontend for Proxy Thing.
	 */
	class app
	{
	public:
		/**
		 * Constructor.
		 */
		app();
		
		virtual ~app();
		
		
		
		/**
		 * Parses arguments and calls app::run(po::variables_map).
		 * 
		 * @param  argc Argument count
		 * @param  argv Argument values
		 * @return      Exit code
		 */
		int run(int argc, char **argv);
		
		/**
		 * Main entry point.
		 * 
		 * @param  args Commandline arguments
		 * @return      Exit code
		 */
		int run(po::variables_map args);
		
		/**
		 * Parses commandline arguments.
		 * 
		 * @param  argc Argument count
		 * @param  argv Argument values
		 * @return      A parsed map of arguments
		 */
		po::variables_map parse_args(int argc, char **argv);
		
		/**
		 * Prints a help message to the user.
		 */
		void print_help();
		
		/**
		 * Initializes application logging.
		 * 
		 * @param args Arguments
		 */
		void init_logging(po::variables_map args);
		
		/**
		 * Initializes application services.
		 * 
		 * Without explicit initialization, services are lazily initialized
		 * when requested.
		 * 
		 * @param args Arguments
		 */
		void init_services(po::variables_map args);
		
		/**
		 * Initializes the proxy server.
		 * 
		 * @param args Arguments
		 */
		void init_server(po::variables_map args);
		
		/**
		 * Initializes background threads.
		 * 
		 * @param args Arguments
		 */
		void init_threads(po::variables_map args);
		
		
		
		/// Returns the option definitions
		inline po::options_description& options() { return m_options; }
		
		/// Returns the IO Service
		inline io_service& service() { return m_service; }
		
		/// Returns all running background threads
		inline std::vector<std::thread>& threads() { return m_threads; }
		
		/// Returns the proxy server
		inline std::shared_ptr<proxy_server> server() { return m_server; }
		
	protected:
		po::options_description m_options;		///< Option definitions
		io_service m_service;					///< IO Service
		std::vector<std::thread> m_threads;		///< Threads
		std::shared_ptr<proxy_server> m_server;	///< Server
	};
}

#endif
