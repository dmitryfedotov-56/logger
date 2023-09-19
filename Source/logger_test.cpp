	/***********************************************************/
	/*                     Logger Test                         */
	/***********************************************************/

	#define _CRT_SECURE_NO_WARNINGS

	#include "logger.h"		// logger definition

	#include <time.h>
	#include <iostream>

	#include <thread>
	#include <mutex>

	Logger* logger = nullptr;
	bool finish = false;

	/***********************************************************/
	/*                    Writer Thread                        */
	/***********************************************************/

	#define BUFFSIZE	40

	void writer_Thread(std::shared_ptr<Logger> logger, int n)	// writer thread
	{
		struct tm* local;
		char s[BUFFSIZE];
		std::string message = "writer" + std::to_string(n) + " ";	// message header
		std::string m = "";

		for (int i = 0; i < 2; i++)
		{
			std::this_thread::sleep_for(std::chrono::seconds(1));	// wait a second
			time_t timestamp = time(nullptr);						// get timestamp
			local = localtime(&timestamp);
			strftime(s, BUFFSIZE, "%d.%m.%Y %H:%M:%S", local);
			m = message + s;										// form message
			logger->write(m.c_str());								// log message
		};
	};

	/***********************************************************/
	/*                    Reader Thread                        */
	/***********************************************************/

	std::mutex locker;	// mutex to lock console

	void print_Message(std::string& s, int n)
	{
		locker.lock();
		std::string message = "reader" + std::to_string(n) + " " + s;
		std::cout << message << std::endl;
		locker.unlock();
	};

	/***********************************************************/

	void reader_Thread(std::shared_ptr<Logger> logger, int n, bool* finish)	// reader thread 
	{
		std::string s;
		Logger::Reader* read = logger->reader();

		std::string m = "";

		while (!(*finish))
		{
			if ((*read)(s))						// if a new line available
				print_Message(s, n);			// print line
		};
		while ((*read)(s))						// print 
			print_Message(s, n);				// the remaining part

		delete read;
	};

	/***********************************************************/
	/*                            Main                         */
	/***********************************************************/

	void test()		// test procedure
	{
		std::shared_ptr<Logger> logger(new_Logger("test.log"));	// create logger
		if (logger == nullptr)
		{
			std::cout << "logger not open" << std::endl;
			exit(0);
		};

		std::thread w1(writer_Thread, logger, 1);			// start writer1
		std::thread w2(writer_Thread, logger, 2);			// start writer2

		std::thread r1(reader_Thread, logger, 1, &finish);	// start reader1
		std::thread r2(reader_Thread, logger, 2, &finish);	// start reader2

		w1.join();		// wait for writer1 completion
		w2.join();		// wait fir writer2 completion

		finish = true;	// writers are finished

		r1.join();		// wait for reader1 completion
		r2.join();		// wait for reader2 completion
	};

	/***********************************************************/

	#include <conio.h>

	int main() 
	{
		test();
		std::cout << "test finished, press any key" << std::endl;
		char c = _getch();
		return 0;
	};

	/***********************************************************/
