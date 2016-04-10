
#ifndef STRING_UTILS_HPP
#define STRING_UTILS_HPP
#include <vector>

namespace http
{
	namespace server
	{
		namespace string_utils
		{
			template<typename T> std::vector<T> split(const T & str, const T & delimiters) 
			{
				std::vector<T> v;
				T::size_type start = 0;
				auto pos = str.find_first_of(delimiters, start);
				while (pos != T::npos) {
					if (pos != start) // ignore empty tokens
						v.emplace_back(str, start, pos - start);
					start = pos + 1;
					pos = str.find_first_of(delimiters, start);
				}
				if (start < str.length()) // ignore trailing delimiter
					v.emplace_back(str, start, str.length() - start); // add what's left of the string
				return v;
			}

			inline std::string join(const std::vector<std::string>& elements, const char* const separator)
			{
				switch (elements.size())
				{
				case 0:
					return "";
				case 1:
					return elements[0];
				default:
					std::ostringstream os;
					std::copy(elements.begin(), elements.end() - 1, std::ostream_iterator<std::string>(os, separator));
					os << *elements.rbegin();
					return os.str();
				}
			}

			inline std::wstring join(const std::vector<std::wstring>& elements, WCHAR* separator)
			{
				switch (elements.size())
				{
				case 0:
					return L"";
				case 1:
					return elements[0];
				default:
					std::wostringstream os;
					std::copy(elements.begin(), elements.end() - 1, std::ostream_iterator<std::wstring, WCHAR>(os, separator));
					os << *elements.rbegin();
					return os.str();
				}
			}
		}
	}
}

#endif
