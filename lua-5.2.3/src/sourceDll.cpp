
extern "C"
{

#include "lua.h"
#include "lauxlib.h"
#include "lualib.h"
};


static int Test(lua_State *L)
{
	lua_pushnumber(L, 1);
	printf("This is a message from c++ dll\n");
	return 1;
}

static const struct luaL_Reg mydlllib[] = {
	{"Test", Test},
	{NULL,NULL},
};

extern "C" int __declspec(dllexport)luaopen_LuaDll(lua_State *L)
{
	printf("luaopen_LuaDll invoked\n");
	luaL_newlib(L, mydlllib); // 5.2之前使用luaL_register(L, "modulename", modulename);

	return 1;
}
