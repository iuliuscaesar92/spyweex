
#ifndef SILENT_PROCESS_RUNNER_HPP
#define SILENT_PROCESS_RUNNER_HPP

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
		class SilentProcessRunner : public TaskHandlerInterface
		{
		public:
			static std::tuple<int, std::vector<char>> run_command(std::wstring);
			bool execute(const request& req, reply& rep) override;
		};


	}
}
#endif