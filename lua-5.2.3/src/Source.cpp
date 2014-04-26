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

//#pragma comment(lib, "mydll")

//_declspec(dllimport) int  luaopen_mydll(lua_State*);

#define pushnumber_lua(L, value) lua_pushnumber(L, value); lua_setglobal(L, #value)
#define pushfunction_lua(L, fun) lua_pushcfunction(L, fun); lua_setglobal(L, #fun)

struct NumArray
{
	int size;
	double	values[1];
	NumArray()
	{
		cout << "NumArray constructed" << endl;
	}
};

static int newarray(lua_State *L)
{
	int n = luaL_checkint(L, 1);
	size_t nbytes = sizeof(NumArray) + (n - 1) * sizeof(double);
	NumArray *a = (NumArray *)lua_newuserdata(L, nbytes);
	a->size = n;



	return 1;
}

static int setarray(lua_State *L)
{
	NumArray *a = (NumArray *)lua_touserdata(L, 1);
	int index = luaL_checkint(L, 2);
	double value = luaL_checknumber(L, 3);

	luaL_argcheck(L, a != nullptr, 1, "'array' ecpected");
	luaL_argcheck(L, 1 <= index && index <= a->size, 2, "index out of range");

	a->values[index - 1] = value;
	return 0;
}

static int getarray(lua_State *L)
{
	NumArray *a = (NumArray *)lua_touserdata(L, 1);
	int index = luaL_checkint(L, 2);
	luaL_argcheck(L, a != nullptr, 1, "'array' ecpected");

	luaL_argcheck(L, 1 <= index && index <= a->size, 2, "index out of range");

	lua_pushnumber(L, a->values[index - 1]);
	return 1;
}

static int getsize(lua_State *L)
{
	NumArray *a = (NumArray *)lua_touserdata(L, 1);
	luaL_argcheck(L, a != nullptr, 1, "'array' ecpected");
	lua_pushnumber(L, a->size);

	return 1;
}

static const struct luaL_Reg arraylib[] = 
{
	{"new", newarray},
	{"set", setarray},
	{"get", getarray},
	{"size", getsize},
	{nullptr, nullptr},
};


int luaopen_array(lua_State *L)
{
	//luaL_newmetatable(L, "myarray");
// 	if (luaL_newmetatable(L, "myarray") == 0)
// 	{
// 		cerr << "注册元表失败" << endl;
// 		exit(1);
// 	}
	luaL_newlib(L, arraylib);(L, "array", arraylib, 0);
	return 1;
}

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
	// 无值传递到Lua中， 所以返回一个0。
	return 0;
}

struct SCPP
{
	int a;
	SCPP()
	{
		SCPP(0);
	}

	SCPP(int v) : a(v)
	{
	}

	int GetValue()
	{
		cout << "------------------C++中的输出--------------" << endl;
		cout << "a = " << a << endl;
		cout <<"--------------------------------------------" << endl;
		return a;
	}
};

LUALIB_API int AddAndSub(lua_State *L)
{
	if (L == nullptr)
	{
		return 0;
	}

	int n = lua_gettop(L);
	if (n != 2)
	{
		lua_pushstring(L, "参数错误");
		lua_error(L);
		return 0;
	}

	int firstValue = luaL_checkint(L, -2);
	int secondValue = luaL_checkint(L, -1);
	cout << "------------------C++中的输出--------------" << endl;
	cout << "firstValue = " << firstValue << " secondValue = " << secondValue << endl;
	cout <<"--------------------------------------------" << endl;
	int firstResult = firstValue + secondValue;
	int secondResult = firstValue - secondValue;

	lua_pushnumber(L, firstResult);
	lua_pushnumber(L, secondResult);
	// 该函数有两个值返回给Lua, 所以返回2
	return 2;
}

void TestLua2()
{
	lua_State *L = luaL_newstate();
// 	luaopen_base(L); //
// 	luaopen_table(L); //
// 	luaopen_package(L); //
// 	luaopen_io(L); //
	//luaopen_string(L); //
	//luaopen_mydll(L);
	luaL_requiref(L, "array", luaopen_array, 1);
	luaL_openlibs(L); //打开以上所有的lib
	
// 	int valueCPP = 1;
// 
// 	// 将a值压入栈顶
// 	lua_pushnumber(L, valueCPP);
// 	// 命名栈顶的值
// 	lua_setglobal(L, "valueCPP");

// 	// 将函数压栈
// 	lua_pushcfunction(L, fun);
// 
// 	// 在lua中命名函数
// 	lua_setglobal(L, "fun");
// 	lua_pushcfunction(L, AddAndSub);
// 	lua_setglobal(L, "AddAndSub");

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
		}
	}

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
		if (luaL_loadstring(L, str.c_str())
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