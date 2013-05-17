#include "PluginManager.h"
#include <fstream>
extern "C"
{
	#include "lua.h"
	#include "lualib.h"
	#include "lauxlib.h"
}

template<class T>
luabind::object VectorToTable(lua_State* L, std::vector<T> const& v){
	luabind::object result = luabind::newtable(L);
	int n = 1;
	for (std::vector<T>::const_iterator iter(v.begin()), end(v.end()); iter != end; ++iter)
	{
		result[n++] = *iter;
	}

	return result;
}

template<class T>
std::vector<T> TableToVector(luabind::object _table){
	
	std::vector<T> result;

	if(luabind::type(_table) ==  LUA_TTABLE)
	{
		for (luabind::iterator i(_table), end; i != end; ++i)
		{
			result.push_back(luabind::object_cast<T>(*i));
		} 
	}
	return result;
}

PluginManager::PluginManager(){
	m_pLuaState = nullptr;
}

bool PluginManager::Initialize(const std::string& _pluginFile){
	std::ifstream luaFile(_pluginFile);
	if(luaFile) {
		m_pLuaState = luaL_newstate();
		luaL_openlibs(m_pLuaState);
		luabind::open(m_pLuaState);

		luabind::module(m_pLuaState) [
			luabind::class_<Point>("Point")
				.def(luabind::constructor<int, int>())
				.def(luabind::constructor<const Point&>())
				.def_readwrite("X", &Point::X)
				.def_readwrite("Y", &Point::Y)
		];
			
		luabind::module(m_pLuaState) [
			luabind::class_<ReturnWrapper>("ReturnWrapper")
				.def(luabind::constructor<luabind::object, int, int>())
		];

		int result = luaL_dofile(m_pLuaState, _pluginFile.c_str());
		if(result != 0) {
			Shutdown();
			return false;
		}
		//std::cout << luabind::call_function<int>(luaState, "sum", VectorToTable(luaState, numbers)) << std::endl;
		return true;
	}
	return false;
}

std::vector<Point> PluginManager::GetPlacements(int _frameWidth, int _frameHeight, int _frameCount, int& _outWidth, int& _outHeight){
	ReturnWrapper returnValues = luabind::call_function<ReturnWrapper>(m_pLuaState, "getPlacements", _frameWidth, _frameHeight, _frameCount);
	_outWidth = returnValues.OutWidth;
	_outHeight = returnValues.OutHeight;
	return TableToVector<Point>(returnValues.Positions);
}

void PluginManager::Shutdown(void){
	if(m_pLuaState){
		lua_close(m_pLuaState);
		m_pLuaState = nullptr;
	}
}


PluginManager::~PluginManager(void){
	Shutdown();
}
