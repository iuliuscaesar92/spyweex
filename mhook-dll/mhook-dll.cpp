
#include "mhook-dll.h"
//namespace http {
//	namespace server {
		namespace privileges
		{

			MHOOKDLL_API void require()
			{
				TrueRtlQueryElevationFlags = (NTSTATUS(NTAPI *) (DWORD*)) GetProcAddress(GetModuleHandle(L"ntdll.dll"), "RtlQueryElevationFlags");
				if (Mhook_SetHook((PVOID*)&TrueRtlQueryElevationFlags, FakeRtlQueryElevationFlags))
				{
					//std::cout << "Hooked. Privileges aquired .";
					OutputDebugString((L"Hooked. Privileges aquired."));
				}
			}
			MHOOKDLL_API void release()
			{
				if (Mhook_Unhook((PVOID*)&TrueRtlQueryElevationFlags))
				{
					//std::cout << "Unhooked. Privileges released.";
					OutputDebugString((L"Unhooked. Privileges released."));
				}
			}
		}
//	}
//}