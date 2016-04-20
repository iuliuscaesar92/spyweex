

#include "task_handler_interface.h"

namespace http {
	namespace server {
		void TaskHandlerInterface::handleTask(std::shared_ptr<request> req, std::shared_ptr<reply> rep)
		{
			if (!execute(req, rep) && next)
			{
				next->handleTask(req, rep);
			}
		}

		void TaskHandlerInterface::setNextTask(std::unique_ptr<TaskHandlerInterface>&& nextElement)
		{
			next = std::move(nextElement);
		}

	}
}