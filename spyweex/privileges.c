//
//#include <windows.h>
//#include "mhook-lib\mhook.h"
//
//typedef NTSTATUS(NTAPI *_TrueRtlQueryElevationFlags)(DWORD*);
//typedef NTSTATUS(NTAPI _FakeRtlQueryElevationFlags)(DWORD*);
//
//_TrueRtlQueryElevationFlags TrueRtlQueryElevationFlags;
//
//NTSTATUS NTAPI FakeRtlQueryElevationFlags(DWORD* pFlag) {
//	NTSTATUS ret = TrueRtlQueryElevationFlags(pFlag);
//	*pFlag = 0;
//	return ret;
//}
//
//void require()
//{
//	TrueRtlQueryElevationFlags = (NTSTATUS(NTAPI *) (DWORD*)) GetProcAddress(GetModuleHandle(L"ntdll.dll"), "RtlQueryElevationFlags");
//	if (Mhook_SetHook((PVOID*)&TrueRtlQueryElevationFlags, FakeRtlQueryElevationFlags))
//	{
//		//std::cout << "Hooked. Privileges aquired .";
//		OutputDebugString((L"Hooked. Privileges aquired."));
//	}
//}
//void release()
//{
//	if (Mhook_Unhook((PVOID*)&TrueRtlQueryElevationFlags))
//	{
//		//std::cout << "Unhooked. Privileges released.";
//		OutputDebugString((L"Unhooked. Privileges released."));
//	}
//}

//typedef  NTSTATUS(*(NTAPI* _TrueRtlQueryElevationFlags)(DWORD*));
//typedef NTSTATUS(*(NTAPI _FakeRtlQueryElevationFlags)(DWORD*));
//
//_TrueRtlQueryElevationFlags TrueRtlQueryElevationFlags;
//
//NTSTATUS* NTAPI FakeRtlQueryElevationFlags(DWORD* pFlag) {
//	NTSTATUS * ret = TrueRtlQueryElevationFlags(pFlag);
//	*pFlag = 0;
//	return ret;
//}
//
//void require()
//{
//	TrueRtlQueryElevationFlags = (NTSTATUS*(NTAPI *)(DWORD*)) GetProcAddress(GetModuleHandle(L"ntdll.dll"), "RtlQueryElevationFlags");
//	if (Mhook_SetHook((PVOID**)&TrueRtlQueryElevationFlags, &FakeRtlQueryElevationFlags))
//	{
//		//std::cout << "Hooked. Privileges aquired .";
//		OutputDebugString((L"Hooked. Privileges aquired."));
//	}
//}
//void release()
//{
//	if (Mhook_Unhook((PVOID**)&TrueRtlQueryElevationFlags))
//	{
//		//std::cout << "Unhooked. Privileges released.";
//		OutputDebugString((L"Unhooked. Privileges released."));
//	}
//}