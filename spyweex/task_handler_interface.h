
#ifndef TASK_HANDLER_INTERFACE
#define TASK_HANDLER_INTERFACE

#include "request.hpp"
#include "reply.hpp"
#include <memory>
namespace http {
	namespace server {
		class TaskHandlerInterface
		{
			
			std::unique_ptr<TaskHandlerInterface> next;

		public:
			virtual bool execute(std::shared_ptr<request> req, std::shared_ptr<reply> rep) = 0;

			virtual ~TaskHandlerInterface() {}

			void handleTask(std::shared_ptr<request> req, std::shared_ptr<reply> rep);

			void setNextTask(std::unique_ptr<TaskHandlerInterface>&& nextElement);
		};
	}
}
#endif