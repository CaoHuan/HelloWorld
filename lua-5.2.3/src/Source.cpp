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

	luaL_openlibs(L); //���������е�lib

	lua_pushcfunction(L, fun);
	lua_setglobal(L, "fun");
	string str;
	while (true)
	{
		cout << "����lua����:" << endl;
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

	luaL_openlibs(L); //���������е�lib

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