#include <iostream>

// #include "State.h"
// #include "ProtoSocket.h"
#include <memory>
#include <windows.h>
#include <set>
#include <vector>
#include "Producer_Consumer_Queue.h"
using namespace std;
using namespace CDH;

struct Test
{
	int i;
};
static volatile long count1 = 1000;
set<int> ssssssssss;
static ProducerConsumerQUeue<Test*> queue;

set<int> m;
vector<int> ms;

DWORD WINAPI ThreadFun(LPVOID lpThreadParameter)
{
	Test *t =new Test();
	t->i = count1;
	::InterlockedIncrement(&count1);
	ssssssssss.insert(count1);
	queue.Push(t);
	
	return 0;
}

DWORD WINAPI ThreadFunPOP(LPVOID lpThreadParameter)
{
	Test *t;
	while (queue.Pop(t))
	{
		// 		cout << t->i << endl;
		// 		count1--;
		m.insert(t->i);
		ms.push_back(t->i);
	}

	return 0;
}
int main()
{
	DWORD dw = 0;
	int length  = 1000;
	HANDLE *h = new	HANDLE[length];
	for (int i = 0; i < length; ++i)
	{
		h[i] = ::CreateThread(NULL, 0, ThreadFun, NULL, 0, &dw);
	}
	

	cout << "  count = " << count1 << endl;
	WaitForMultipleObjects(length, h, true, INFINITE);

	for (int i = 0; i < length; ++i)
	{
		h[i] = ::CreateThread(NULL, 0, ThreadFunPOP, NULL, 0, &dw);
	}

	WaitForMultipleObjects(length, h, true, INFINITE);

// 	cout << "done " <<  count1 << endl;
 	cout << m.size() << endl;
	cout << ms.size() << endl;
	return 0;
}