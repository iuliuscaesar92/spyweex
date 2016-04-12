

#include "task_handler_interface.h"

namespace http {
	namespace server {
		void TaskHandlerInterface::handleTask(const request& req, reply& rep)
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