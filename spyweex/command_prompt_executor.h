
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
		class CommandPromptExecutor: public TaskHandlerInterface
		{
		public:
			static int get_reg_oemcp(std::string&);
			static std::tuple<int, std::vector<char>> run_command(std::string);
			bool execute(std::shared_ptr<request> req, std::shared_ptr<reply> rep) override;
		};


	}
}


#endif