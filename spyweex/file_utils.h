#pragma once

#ifndef HTTP_CONNECTION_HPP
#define HTTP_CONNECTION_HPP

#include "AccCtrl.h"
#include "Aclapi.h"

namespace http
{
	namespace server
	{
		namespace file_utils
		{
			HANDLE CreateFileWithAllPermissions(wchar_t *);
		}
	}
}


#endif