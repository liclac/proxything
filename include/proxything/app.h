namespace proxything
{
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
		
		/**
		 * Destructor.
		 */
		virtual ~app();
		
		/**
		 * Main entry point.
		 * 
		 * @param  argc Argument count
		 * @param  argv Argument values
		 * @return      Exit code
		 */
		int run(int argc, char **argv);
	};
}
