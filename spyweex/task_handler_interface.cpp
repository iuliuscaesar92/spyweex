

#include "task_handler_interface.h"
#include "server.hpp"

namespace http {
	namespace server {
		void TaskHandlerInterface::handleTask(std::shared_ptr<request> req)
		{
			if (!execute(req) && next)
			{
				next->handleTask(req);
			}
		}

		void TaskHandlerInterface::setNextTask(std::unique_ptr<TaskHandlerInterface>&& nextElement)
		{
			next = std::move(nextElement);
		}

		boost::mutex TaskHandlerInterface::async_write_mutex;
	}
}
