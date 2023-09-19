	/***********************************************************/
	/*                     Logger Implementation               */
	/***********************************************************/

	#include "logger.h"		// logger definition

	#include <string>
	#include <fstream>
	#include <thread>
	#include <mutex>
	#include <functional>
	#include <iostream>

	/***********************************************************/

	#define DEBUG	1

	class Logger_Class : public Logger 
	{
		friend class Reader_Class;
		friend std::shared_ptr<Logger> new_Logger(const char* filename);
	
	private:	// Logger private part

		std::fstream logfile;
		std::mutex locker;
		Logger_Class* thislogger;

		bool eof(size_t rpos)					// true if end od file
		{
			logfile.seekg(0, std::ios::end);			// go to the end
			size_t size = logfile.tellg();		// file size
			if (rpos == size)return true;		// current position == size?
			return false;
		}; 

		size_t read(size_t& rpos, std::string& s)	// read from position rpos
		{
			logfile.seekg(rpos, std::ios::beg);
			getline(logfile, s);
			return logfile.tellg();
		};

		// Internal Reader Class

		class Reader_Class : public Reader			// reader class
		{
		public:
			size_t rpos = 0;						// reaader position
			std::shared_ptr<Logger_Class> logger;

			virtual bool read(std::string& s)		// read string
			{
				bool result;
				logger->locker.lock();				// lock logger
				if (logger->eof(rpos))				// if end of file
					result = false;					// return false
				else 
				{
					rpos = logger->read(rpos, s);	// read from rpos
					result = true;
				};
				logger->locker.unlock();			// unlock logger
				return result;
			};

			virtual bool operator()(std::string& s) { return read(s); };

			Reader_Class(Logger_Class* lptr)
			{
				logger = std::shared_ptr<Logger_Class>(lptr);	// store logger pointer			
				rpos = 0;										// initial reading position
			};

			virtual ~Reader_Class() 
			{ 
				// cout << "reader closed" << endl; 
			};
		};

	public:		// Logger public part

		virtual void write(const char* s) 
		{
			locker.lock();							// exclusuve lock
			logfile << s << std::endl;				// write
			logfile.flush();						// flush immediately
			locker.unlock();						// exclusive unlock
		};

		Reader* reader()
		{
			Reader_Class* rptr = new Reader_Class(thislogger);
			return rptr;
		};

		Logger_Class() { thislogger = this; };

		virtual ~Logger_Class()						// Logger destructor closes the file
		{ 
			logfile.close();	// close logfile

#if DEBUG
			std::cout << "logfile closed" << std::endl;
#endif
		};	
	};

	/***********************************************************/

	std::shared_ptr<Logger> new_Logger(const char* filename)			// create Logger
	{
		std::ofstream out;							// try to create a log file      
		out.open(filename); 
		if (!out.is_open()) return nullptr;
		out.close();

		Logger_Class* logger = new Logger_Class();

		logger->logfile.open(filename);				// try to open logfile
		if (!(logger->logfile.is_open()))			// if not open
		{
			return nullptr;							// return nullptr
			delete logger;
		};
		
		return std::shared_ptr<Logger>(logger);		// return logger
	};

	/***********************************************************/


