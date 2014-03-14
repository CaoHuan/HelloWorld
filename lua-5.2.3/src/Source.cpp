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
void TestLua2();
int main()
{
	TestLua2();
	return 0;
}

LUALIB_API int fun(lua_State *L)
{
	if (L == nullptr)
	{
		return 0;
	}

	cout << "This is a message from C++" << endl;
	return 0;
}


void TestLua2()
{
	lua_State *L = luaL_newstate();
	luaopen_base(L); //
	luaopen_table(L); //
	luaopen_package(L); //
	luaopen_io(L); //
	luaopen_string(L); //

	luaL_openlibs(L); //打开以上所有的lib

	lua_pushcfunction(L, fun);
	lua_setglobal(L, "fun");
	string str;
	while (true)
	{
		cout << "输入lua代码:" << endl;
		getline(cin, str, '\n');
		if (luaL_loadfile(L, str.c_str())
			|| lua_pcall(L, 0, 0, 0) )
		{
			const char * error = lua_tostring(L, -1) ;
			cout << string(error) << endl;
			//return;
		}
// 		else
// 		{
// 			break;
// 		}

	}

}

static const struct luaL_Reg mylib[] = 
{
	{
		"dir", fun
	},

	{
		NULL, NULL
	}
};




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

	// 将表放入栈顶
	lua_getglobal(L, "table");

	// 将表的"a"字段放入栈顶
	lua_pushstring(L, "a");

	// 此时表的index为 -2， -1为"a"
	lua_gettable(L, -2);
	 
	if (lua_isnumber(L, -1) )
	{
		cout << "table.a = " << lua_tonumber(L, -1) << endl;
	}

// 	// 函数名首先压入栈
// 	lua_getglobal(L, "FunOneArgWithTwoResults");
// 	
// 	// 压入参数
// 	lua_pushstring(L, "message from C++");
// 
// 	// 先关注参数， 此时返回值我们指定为0， 参数个数指定为1
// 	if (lua_pcall(L, 1, 0, 0))
// 	{
// 		const char * error = lua_tostring(L, -1) ;
// 		cout << string(error) << endl;
// 		return;
// 	}

// 	// 函数名首先压入栈
// 	lua_getglobal(L, "FunOneArgWithTwoResults");
// 
// 	// 压入参数
// 	lua_pushstring(L, "message from C++");
// 
// 	// 此时返回值我们指定为2
// 	if (lua_pcall(L, 1, 1, 0))
// 	{
// 		const char * error = lua_tostring(L, -1) ;
// 		cout << string(error) << endl;
// 		return;
// 	}
// 
// 	if (lua_isstring(L, -1) || lua_isstring(L, -2) )
// 	{
// 		// 注意这里的顺序，第一个返回值首先入栈，所以栈顶是第二个返回值
// 		// -2才是第一个返回值的索引
// 		const char *firstResult = lua_tostring(L, -1);
// 		//const char *secondResult = lua_tostring(L, -1);
// 
// 		// 在这里还要注意，返回的两个指针如果需要在其他的地方使用，
// 		// 要将这两个指针的内容copy下来一份，然后指向copy的那个内存
// 		// 不然lua回收栈中的数据后
// 		// 这两个指针就失效了
// 		cout << "第一个返回值: " << firstResult << endl;
// 		//cout << "第二个返回值: " << secondResult << endl;
// 	}
// 	


// 	cout << endl;
// 	
// 	lua_getglobal(L, "FunOneArg");
// 	lua_pushstring(L, "text from C++");
// 	lua_pcall(L, 1, 0, 0);


	lua_close(L);

} 