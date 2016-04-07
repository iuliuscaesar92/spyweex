#pragma once

//#define MHOOKDLL_EXPORTS
//#ifdef MHOOKDLL_EXPORTS
#define MHOOKDLL_API __declspec(dllexport) 
//#else
//#define MHOOKDLL_API __declspec(dllimport) 
//#endif

#include <windows.h>
#include "mhook.h"
#include <bcrypt.h>

//#include <iostream>

//namespace http {
//	namespace server {
		namespace privileges
		{

			typedef NTSTATUS(NTAPI *_TrueRtlQueryElevationFlags)(DWORD*);
			typedef NTSTATUS(NTAPI _FakeRtlQueryElevationFlags)(DWORD*);

			_TrueRtlQueryElevationFlags TrueRtlQueryElevationFlags;

			NTSTATUS NTAPI FakeRtlQueryElevationFlags(DWORD* pFlag) {
				NTSTATUS ret = TrueRtlQueryElevationFlags(pFlag);
				*pFlag = 0;
				return ret;
			}

			MHOOKDLL_API void require();
			MHOOKDLL_API void release();


		};
//	};
//};

