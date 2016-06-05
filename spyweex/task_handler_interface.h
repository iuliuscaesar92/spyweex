
#ifndef TASK_HANDLER_INTERFACE
#define TASK_HANDLER_INTERFACE

#include "request.hpp"
#include "reply.hpp"
#include <memory>
#include "async_operation.hpp"

namespace http {
	namespace server {
		class TaskHandlerInterface
		{		
			std::unique_ptr<TaskHandlerInterface> next;

		public:
			explicit TaskHandlerInterface(boost::asio::ip::tcp::socket& sock, boost::asio::io_service& io_ref):
				socket_(sock), io_ref_(io_ref) {}

			virtual bool execute(std::shared_ptr<request> req) = 0;

			virtual ~TaskHandlerInterface() {}

			void handleTask(std::shared_ptr<request> req);

			void setNextTask(std::unique_ptr<TaskHandlerInterface>&& nextElement);

			boost::asio::ip::tcp::socket& socket_;

			boost::asio::io_service& io_ref_;

			boost::shared_ptr<async_op> operations_queue_ptr;

			static boost::mutex async_write_mutex;
		};
	}
}
#endif