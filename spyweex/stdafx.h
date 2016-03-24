// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently, but
// are changed infrequently
//

#pragma once

#include "targetver.h"

#define WIN32_LEAN_AND_MEAN             // Exclude rarely-used stuff from Windows headers
// Windows Header Files:
#include <windows.h>

// C RunTime Header Files
#include <stdlib.h>
#include <malloc.h>
#include <memory.h>
#include <tchar.h>

#include "reply.hpp"
#include "request.hpp"
#include "request_handler.hpp"
#include "request_parser.hpp"
#include "connection.hpp"
#include "connection_manager.hpp"
#include "mime_types.hpp"
#include "reply.hpp"
#include "server.hpp"
#include "header.hpp"

// for desktop screenshot
#include <iostream>
#include <string>
#include <gdiplus.h>
// end


//comments

// TODO: reference additional headers your program requires here
