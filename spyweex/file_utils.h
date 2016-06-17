#pragma once

#ifndef HTTP_CONNECTION_HPP
#define HTTP_CONNECTION_HPP

#include "AccCtrl.h"
#include "Aclapi.h"
#include <string>
#include <fstream>
namespace http
{
	namespace server
	{
		namespace file_utils
		{
			HANDLE CreateFileWithAllPermissions(wchar_t *);

			wchar_t * make_temp_file_path();

			bool fileExists(const char *fileName);

		}
	}
}


#endif