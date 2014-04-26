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
// 		cerr << "ע��Ԫ��ʧ��" << endl;
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
	// ��ֵ���ݵ�Lua�У� ���Է���һ��0��
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
		cout << "------------------C++�е����--------------" << endl;
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
		lua_pushstring(L, "��������");
		lua_error(L);
		return 0;
	}

	int firstValue = luaL_checkint(L, -2);
	int secondValue = luaL_checkint(L, -1);
	cout << "------------------C++�е����--------------" << endl;
	cout << "firstValue = " << firstValue << " secondValue = " << secondValue << endl;
	cout <<"--------------------------------------------" << endl;
	int firstResult = firstValue + secondValue;
	int secondResult = firstValue - secondValue;

	lua_pushnumber(L, firstResult);
	lua_pushnumber(L, secondResult);
	// �ú���������ֵ���ظ�Lua, ���Է���2
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
	luaL_openlibs(L); //���������е�lib
	
// 	int valueCPP = 1;
// 
// 	// ��aֵѹ��ջ��
// 	lua_pushnumber(L, valueCPP);
// 	// ����ջ����ֵ
// 	lua_setglobal(L, "valueCPP");

// 	// ������ѹջ
// 	lua_pushcfunction(L, fun);
// 
// 	// ��lua����������
// 	lua_setglobal(L, "fun");
// 	lua_pushcfunction(L, AddAndSub);
// 	lua_setglobal(L, "AddAndSub");

	string str;
	while (true)
	{
		cout << "����lua�ļ�·��:" << endl;
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

	luaL_openlibs(L); //���������е�lib

	string str;
	while (true)
	{
		cout << "����lua�ļ�·��:" << endl;
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

	// �������ջ��
	lua_getglobal(L, "table");

	// �����"a"�ֶη���ջ��
	lua_pushstring(L, "a");

	// ��ʱ���indexΪ -2�� -1Ϊ"a"
	lua_gettable(L, -2);
	 
	if (lua_isnumber(L, -1) )
	{
		cout << "table.a = " << lua_tonumber(L, -1) << endl;
	}

// 	// ����������ѹ��ջ
// 	lua_getglobal(L, "FunOneArgWithTwoResults");
// 	
// 	// ѹ�����
// 	lua_pushstring(L, "message from C++");
// 
// 	// �ȹ�ע������ ��ʱ����ֵ����ָ��Ϊ0�� ��������ָ��Ϊ1
// 	if (lua_pcall(L, 1, 0, 0))
// 	{
// 		const char * error = lua_tostring(L, -1) ;
// 		cout << string(error) << endl;
// 		return;
// 	}

// 	// ����������ѹ��ջ
// 	lua_getglobal(L, "FunOneArgWithTwoResults");
// 
// 	// ѹ�����
// 	lua_pushstring(L, "message from C++");
// 
// 	// ��ʱ����ֵ����ָ��Ϊ2
// 	if (lua_pcall(L, 1, 1, 0))
// 	{
// 		const char * error = lua_tostring(L, -1) ;
// 		cout << string(error) << endl;
// 		return;
// 	}
// 
// 	if (lua_isstring(L, -1) || lua_isstring(L, -2) )
// 	{
// 		// ע�������˳�򣬵�һ������ֵ������ջ������ջ���ǵڶ�������ֵ
// 		// -2���ǵ�һ������ֵ������
// 		const char *firstResult = lua_tostring(L, -1);
// 		//const char *secondResult = lua_tostring(L, -1);
// 
// 		// �����ﻹҪע�⣬���ص�����ָ�������Ҫ�������ĵط�ʹ�ã�
// 		// Ҫ��������ָ�������copy����һ�ݣ�Ȼ��ָ��copy���Ǹ��ڴ�
// 		// ��Ȼlua����ջ�е����ݺ�
// 		// ������ָ���ʧЧ��
// 		cout << "��һ������ֵ: " << firstResult << endl;
// 		//cout << "�ڶ�������ֵ: " << secondResult << endl;
// 	}
// 	


// 	cout << endl;
// 	
// 	lua_getglobal(L, "FunOneArg");
// 	lua_pushstring(L, "text from C++");
// 	lua_pcall(L, 1, 0, 0);


	lua_close(L);

} 