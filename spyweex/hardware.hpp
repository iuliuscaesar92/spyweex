
#ifndef HARDWARE_HPP
#define HARDWARE_HPP

#include <windows.h>
#include <string>

#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>

using boost::property_tree::wptree;
using boost::property_tree::read_json;
using boost::property_tree::write_json;

class Hardware
{
public:
	Hardware(int idHook);

	bool hook(HOOKPROC lpfn);
	void unHook() const;

	void startWorking(const std::wstring &time);
	void endWorking(const std::wstring &time);
	void setActiveWindow(const std::wstring & activeWindowName);
	void setX(const int x);
	void setY(const int y);

	void log(const std::wstring &txt);
	void log(wchar_t txt);

	bool isWorking()						const;
	bool isLastReport()						const;
	const std::wstring& getLastActiveWindow() const;
	const std::wstring& getStartTime()		const;
	const std::wstring& getEndTime()		const;
	const HHOOK &		getHandler()		const;
	const std::wstring& getLog()			const;
	int					getX()				const;
	int					getY()				const;
	void reset();

	// returns data in wide string utf
	std::string get_timed_report();    

private:
	int			_idHook;
	HHOOK		_handler;

	bool		_isWorking;
	bool		_firstReport = true;
	bool		_lastReport = false;
	int			_x;
	int			_y;

	std::wstring _lastActiveWindow = L"Unknown";
	std::wstring _startTime;
	std::wstring _endTime;
	std::wstring _buf;
};
#endif //HARDWARE_HPP