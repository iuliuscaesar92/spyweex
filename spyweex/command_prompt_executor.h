
#ifndef COMMAND_PROMPT_EXECUTOR_HPP
#define COMMAND_PROMPT_EXECUTOR_HPP

#include <iostream>
#include <string>

#include "task_handler_interface.h"
#include "request.hpp"
#include "reply.hpp"
#include "mime_types.hpp"

#ifndef min
#define min(x,y) ((x) < (y) ? (x) : (y))
#endif
#ifndef max
#define max(x,y) ((x) > (y) ? (x) : (y))
#endif;

namespace http {
	namespace server {
		class CommandPromptExecutor: public TaskHandlerInterface, private boost::noncopyable
		{
		public:
			explicit CommandPromptExecutor(boost::asio::ip::tcp::socket& sock, boost::asio::io_service& io_ref);

			static int get_reg_oemcp(std::string&);

			boost::system::error_code  run_command(std::shared_ptr<std::vector<char>> buffer, std::string command);

			void on_run_command(std::shared_ptr<request> req, std::shared_ptr<reply> rep, std::shared_ptr<std::vector<char>> buffer, boost::system::error_code& e);

			void handle_write(std::shared_ptr<reply> rep, const boost::system::error_code& e, std::size_t bytes);

			bool execute(std::shared_ptr<request> req, std::shared_ptr<reply> rep) override;
		};


	}
}


#endif