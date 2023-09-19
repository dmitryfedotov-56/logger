	#pragma once
	/***********************************************************/
	/*                     Logger Definition                   */
	/***********************************************************/

	#include <string>
	#include <memory>

	/***********************************************************/

	class Logger {
	public:

		class Reader
		{
		public:

			virtual bool operator()(std::string& s) = 0;	// reader operator
		};

		virtual void write(const char* s) = 0;				// logfile write

		virtual Reader* reader() = 0;						// get logfile reader
		virtual ~Logger() {};
	};

	std::shared_ptr<Logger> new_Logger(const char* filename);		// create Logger

	/***********************************************************/

