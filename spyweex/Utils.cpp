
#include "utils.hpp"

namespace http
{
	namespace server
	{
		namespace utils
		{
			std::wstring get_time()
			{
				time_t roxtime;
				struct tm	stime;

				time(&roxtime);
				gmtime_s(&stime, &roxtime);
				std::ostringstream strs(stime.tm_year % 100 + 2000);
				strs << (stime.tm_year % 100 + 2000)
					<< '-' << (stime.tm_mon)
					<< '-' << (stime.tm_mday)
					<< ' ' << (stime.tm_hour % 24)
					<< ':' << (stime.tm_min)
					<< " UTC";
				std::string str = strs.str();
				std::wstring wstr(str.begin(), str.end());
				return wstr;
			}
		}

	}
}
