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

namespace http {
namespace server {

request_handler::request_handler(const std::string& doc_root)
  : doc_root_(doc_root)
{
	rootHandler = std::make_unique<ScreenshotTaker>();
	std::unique_ptr<TaskHandlerInterface> cmdHandler = std::make_unique<CommandPromptExecutor>();
	rootHandler->setNextTask(std::move(cmdHandler));
}


void request_handler::handle_request(const request& req, reply& rep)
{
	rootHandler->execute(req, rep);
}

//void request_handler::handle_request(const request& req, reply& rep)
//{
//  
//  std::string extension = "jpg";
//  int code; std::vector<char> buffer;
//
//  std::tie(code, buffer) = ScreenshotTaker::TakeScreenshot(100);
//
//  // Open the file to send back.
//  if (buffer.empty())
//  {
//    rep = reply::stock_reply(reply::internal_server_error);
//    return;
//  }
//  // Fill out the reply to be sent to the client.
//  
//  rep.status = reply::ok;
//  
//  std::string s(buffer.data(), buffer.size());
//  rep.content.append(s);
//
//  //char buf[512];
//  //while (is.read(buf, sizeof(buf)).gcount() > 0)
//  //  rep.content.append(buf, is.gcount());
//  
//  rep.headers.resize(3);
//  rep.headers[0].name = "Tag";
//  rep.headers[0].value = std::string(req.dictionary_headers.at("Tag"));
//  rep.headers[1].name = "Content-Type";
//  rep.headers[1].value = mime_types::extension_to_type(extension);
//  rep.headers[2].name = "Content-Length";
//  rep.headers[2].value = std::to_string(rep.content.size());
//
//}

bool request_handler::url_decode(const std::string& in, std::string& out)
{
  out.clear();
  out.reserve(in.size());
  for (std::size_t i = 0; i < in.size(); ++i)
  {
    if (in[i] == '%')
    {
      if (i + 3 <= in.size())
      {
        int value = 0;
        std::istringstream is(in.substr(i + 1, 2));
        if (is >> std::hex >> value)
        {
          out += static_cast<char>(value);
          i += 2;
        }
        else
        {
          return false;
        }
      }
      else
      {
        return false;
      }
    }
    else if (in[i] == '+')
    {
      out += ' ';
    }
    else
    {
      out += in[i];
    }
  }
  return true;
}

} // namespace server
} // namespace http