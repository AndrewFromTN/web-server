#include "Lua.h"
#include <string>
#include "stdio.h"
#include "Helpers.h"

extern "C" 
{
#include <lua.h>
#include <lualib.h>
#include <lauxlib.h>
}

static int HTML_OUT(lua_State* L)
{
	const char* msg = (char *) lua_tostring(L, -1);
	send_200(nullptr, msg, true);

	return 1;
}

bool execute_script(std::string script, const std::map<std::string, std::string> &data)
{
	lua_State* L = luaL_newstate();
	luaL_openlibs(L);

	int load = luaL_loadfile(L, script.c_str());
	if (load)
	{
		printf("Error loading lua script: %s", lua_tostring(L, -1));
		return false;
	}

	if (!data.empty())
	{
		lua_newtable(L);

		for(auto const& param: data)
		{
			lua_pushstring(L, param.first.c_str());
			lua_pushstring(L, param.second.c_str());
			lua_rawset(L, -3);
		}

		lua_setglobal(L, "Info");

		int* ud = (int*) lua_newuserdata(L, sizeof(int));
		*ud = 1;

		lua_pushcclosure(L, HTML_OUT, 1);
		lua_setglobal(L, "HTML_OUT");
	}

	int result = lua_pcall(L, 0, 1, 0);
	if (result)
	{
		printf("Error executing lua script: %s", lua_tostring(L, -1));
		return false;
	}

	lua_pop(L, 1);
	lua_close(L);

	return true;
}
