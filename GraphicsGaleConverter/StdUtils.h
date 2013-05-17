#pragma once

#include <sstream>
#include <windows.h>

inline std::string IntToStr(int _n) {
	std::stringstream ss;
	ss << _n;
	return ss.str();
}

inline std::wstring ToWideString(const std::string& str){
	int stringLength = MultiByteToWideChar(CP_ACP, 0, str.data(), str.length(), 0, 0);
	std::wstring wstr(stringLength, 0);
	MultiByteToWideChar(CP_ACP, 0,  str.data(), str.length(), &wstr[0], stringLength);
	return wstr;
}
