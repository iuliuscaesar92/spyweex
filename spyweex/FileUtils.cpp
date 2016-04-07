
#include "FileUtils.h"
namespace http
{
	namespace server
	{
		namespace file_utils
		{
			HANDLE CreateFileWithAllPermissions(wchar_t *lpszFileName)
			{
				SID_IDENTIFIER_AUTHORITY SIDAuthWorld = SECURITY_WORLD_SID_AUTHORITY;
				PSID everyone_sid = NULL;
				AllocateAndInitializeSid(&SIDAuthWorld, 1, SECURITY_WORLD_RID,
					0, 0, 0, 0, 0, 0, 0, &everyone_sid);
				EXPLICIT_ACCESS ea;
				ZeroMemory(&ea, sizeof(EXPLICIT_ACCESS));
				ea.grfAccessPermissions = SPECIFIC_RIGHTS_ALL | STANDARD_RIGHTS_ALL;
				ea.grfAccessMode = SET_ACCESS;
				ea.grfInheritance = NO_INHERITANCE;
				ea.Trustee.TrusteeForm = TRUSTEE_IS_SID;
				ea.Trustee.TrusteeType = TRUSTEE_IS_WELL_KNOWN_GROUP;
				ea.Trustee.ptstrName = (LPWSTR)everyone_sid;
				PACL acl = NULL;
				SetEntriesInAcl(1, &ea, NULL, &acl);
				PSECURITY_DESCRIPTOR sd = (PSECURITY_DESCRIPTOR)LocalAlloc(LPTR,
					SECURITY_DESCRIPTOR_MIN_LENGTH);
				InitializeSecurityDescriptor(sd, SECURITY_DESCRIPTOR_REVISION);
				SetSecurityDescriptorDacl(sd, TRUE, acl, FALSE);
				SECURITY_ATTRIBUTES sa;
				sa.nLength = sizeof(SECURITY_ATTRIBUTES);
				sa.lpSecurityDescriptor = sd;
				sa.bInheritHandle = FALSE;

				HANDLE h_file = CreateFile(lpszFileName,
					GENERIC_ALL,
					FILE_SHARE_DELETE,
					&sa,
					CREATE_NEW,
					FILE_ATTRIBUTE_NORMAL,
					NULL);

				FreeSid(everyone_sid);
				LocalFree(sd);
				LocalFree(acl);

				return h_file;
			}

			//HANDLE hFile = CreateFile(
			//	lpszFilename,				// lpFileName
			//	DELETE,                     // dwDesiredAccess
			//	FILE_SHARE_DELETE,			// dwShareMode
			//	NULL,                       // lpSecurityAttributes
			//	OPEN_EXISTING,              // dwCreationDisposition
			//	FILE_FLAG_DELETE_ON_CLOSE,  // dwFlagsAndAttributes
			//	NULL);                      // hTemplateFile
			//CloseHandle(hFile);
		}
	}
}