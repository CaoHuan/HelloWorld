#pragma once

#include "Common.h"
_CDH_BEGIN

/*
带写权限的生产者——消费者队列。考虑到完成端口在写入数据时， 会有多个线程，所以写操作需要线程同步。而读操作是由一个线程进行，所以没有在读的地方做同步
补充：后面做了扩展，增加了*Ex方法，不带Ex后缀的pop和push方法供单线程使用，带后缀的供多线程使用。 已测试。
*/
template<typename DATA_>
class ProducerConsumerQUeue
{
	struct Node
	{
		DATA_ data;
		Node *next;
	};

	Node	*mpHead; // 头结点
	Node	*mpTail; // 尾结点
	long	mWriteLock; // 写锁
	long	mReadLock;
	//CRITICAL_SECTION sec;

public:
	int count;
	ProducerConsumerQUeue() : mWriteLock(0)
							 ,mReadLock(0)
	{
		mpHead = new Node;

		mpHead->next = NULL;
		mpTail = mpHead;
		count = 0;
		//InitializeCriticalSectionAndSpinCount(&sec, SpinCount);
	}

	// 多线程写
	bool PushEx(DATA_& data)
	{

		GetWritePermission();
//			EnterCriticalSection(&sec);
		Push(data);
			++count;
//		LeaveCriticalSection(&sec);
			ReleaseWritePermission();
			return true;
	}

	// 单线程写
	bool Push(DATA_& data)
	{
		/*
			在这里，append的时候，头尾是分开的，插入的结点，其数据是作为头的，
			结点的地址是作为尾部。
			这样取的时候，只要取头即可，留下尾部。
		*/
			// 初始化结点
			Node *pNode = new Node;
			pNode->data = data;
			pNode->next = 0;

			// 此时mpTail是原链表的最后一个结点, 将新添加的结点入队
			mpTail->next= pNode;

			// 将mpTail置为最后一个结点
			mpTail		= pNode;

			return true;
	}

	// 多线程读的情况下，读数据
	bool PopEx(DATA_& data)
	{
		bool ret = false;
		if (GetReadPermission())
		{
			ret = Pop(data);

			ReleaseReadPermission();
			return ret;
		}

		return ret;
	}

	bool Pop(DATA_& data)
	{
		if (mpHead->next != NULL)
		{
			Node *pNode = mpHead;
			mpHead = mpHead->next;
			data = mpHead->data;
			delete pNode;

			return true;
		}

		return false;
	}

	bool GetReadPermission()
	{
		int spin = 1;
		while (::_InterlockedCompareExchange(&mReadLock, 1, 0)/* == 1*/)
		{
			++spin;
			if (spin >= SpinCount)
			{
				return false;
			}

			Sleep(1);
		}

		return true;
	}

	void ReleaseReadPermission()
	{
		mReadLock = 0;
	}

	bool GetWritePermission()
	{ 
		//int spin = 1;
		while (::InterlockedCompareExchange(&mWriteLock, 1, 0)/* == 1*/)
		{
// 			++spin;
// 			if (spin >= SpinCount)
// 			{
// 				return false;
// 			}

			Sleep(1);
		}

		return true;
	}

	void ReleaseWritePermission()
	{
		mWriteLock = 0;
	}



	~ProducerConsumerQUeue()
	{
		DATA_ tmp;
		while (Pop(tmp) )
		{
		}

		if (mpHead)
		{
			delete mpHead;

			mpHead = NULL;
			mpTail = NULL;
		}
	}
};

//测试代码
/*
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
volatile long count1 = 1000;
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
queue.PushEx(t);

return 0;
}

DWORD WINAPI ThreadFunPOP(LPVOID lpThreadParameter)
{
Test *t;

set<int> m;
vector<int> ms;
while (queue.PopEx(t))
{
// 		cout << t->i << endl;
// 		count1--;
m.insert(t->i);
ms.push_back(t->i);
}
printf("\nm = %d\n", m.size());
printf("\nms = %d\n", ms.size());
return 0;
}

int main()
{
DWORD dw = 0;
int length  = 10000;
HANDLE *h = new	HANDLE[length];
for (int i = 0; i < length; ++i)
{
h[i] = ::CreateThread(NULL, 0, ThreadFun, NULL, 0, &dw);
}


cout << "  count = " << count1 << endl;



HANDLE h1 = ::CreateThread(NULL, 0, ThreadFunPOP, NULL, 0, &dw);

HANDLE h2 = ::CreateThread(NULL, 0, ThreadFunPOP, NULL, 0, &dw);


WaitForMultipleObjects(length, h, true, INFINITE);

WaitForSingleObject(h1, INFINITE);
WaitForSingleObject(h2, INFINITE);



//  	cout << m.size() << endl;
// 	cout << ms.size() << endl;
cout << "queue.count = " << queue.count << endl;
return 0;
}
*/

_CDH_END