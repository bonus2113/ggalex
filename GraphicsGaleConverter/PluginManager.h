#pragma once

#include <string>
#include <vector>
#include "Point.h"
#include "luabind/luabind.hpp"

struct lua_State;



class PluginManager
{
private:
	lua_State* m_pLuaState;

	struct ReturnWrapper {
		luabind::object Positions;
		int OutWidth;
		int OutHeight;
		ReturnWrapper(luabind::object _pos, int _width, int _height) : Positions(_pos), OutWidth(_width), OutHeight(_height) { }
	};

public:
	PluginManager(void);
	bool Initialize(const std::string& _pluginFile);
	std::vector<Point> GetPlacements(int _frameWidth, int _frameHeight, int _frameCount, int& _outWidth, int& _outHeight);
	void Shutdown(void);
	~PluginManager(void);
};

