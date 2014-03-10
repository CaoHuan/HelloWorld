#include <iostream>
#include <fstream>
#include <string>
using namespace std;

extern "C"
{

#include <lua.h>
#include <lauxlib.h>
#include <lualib.h>
};

void TestLua();

int main()
{
	TestLua();
	return 0;
}

void TestLua()
{
	lua_State *L = luaL_newstate();
	luaopen_base(L); //
	luaopen_table(L); //
	luaopen_package(L); //
	luaopen_io(L); //
	luaopen_string(L); //

	luaL_openlibs(L); //打开以上所有的lib

	string str;
	while (true)
	{
		cout << "输入lua文件路径:" << endl;
		getline(cin, str, '\n');
		if (luaL_loadfile(L, str.c_str())
			|| lua_pcall(L, 0, 0, 0) )
		{
			const char * error = lua_tostring(L, -1) ;
			cout << string(error) << endl;
			return;
		}
		else
		{
			break;
		}

	}

	int a = 0;
	int b = 0;

	// 获取a的值
	lua_getglobal(L, "a");
	if (!lua_isnumber(L, -1))
	{
		cout << "-2 error" << lua_isnumber(L, -1) << lua_isnumber(L, -1) << endl;
		return ;
	}

	a = lua_tonumber(L, -1);

	// 获取b的值
	lua_getglobal(L, "b");
	if (!lua_isnumber(L, -1))
	{
		cout << "-1 error" << endl;
		return ;
	}
	
	b = lua_tonumber(L, -1);

	cout << "a = " << a << " b = " << b << endl;
	cout << "a + b = " << a + b << endl;

	lua_close(L);

}