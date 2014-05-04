#pragma once

#include "Common.h"
_CDH_BEGIN

/*
��дȨ�޵������ߡ��������߶��С����ǵ���ɶ˿���д������ʱ�� ���ж���̣߳�����д������Ҫ�߳�ͬ����������������һ���߳̽��У�����û���ڶ��ĵط���ͬ��
���䣺����������չ��������*Ex����������Ex��׺��pop��push���������߳�ʹ�ã�����׺�Ĺ����߳�ʹ�á� �Ѳ��ԡ�
*/
template<typename DATA_>
class ProducerConsumerQUeue
{
	struct Node
	{
		DATA_ data;
		Node *next;
	};

	Node	*mpHead; // ͷ���
	Node	*mpTail; // β���
	long	mWriteLock; // д��
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

	// ���߳�д
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

	// ���߳�д
	bool Push(DATA_& data)
	{
		/*
			�����append��ʱ��ͷβ�Ƿֿ��ģ�����Ľ�㣬����������Ϊͷ�ģ�
			���ĵ�ַ����Ϊβ����
			����ȡ��ʱ��ֻҪȡͷ���ɣ�����β����
		*/
			// ��ʼ�����
			Node *pNode = new Node;
			pNode->data = data;
			pNode->next = 0;

			// ��ʱmpTail��ԭ��������һ�����, ������ӵĽ�����
			mpTail->next= pNode;

			// ��mpTail��Ϊ���һ�����
			mpTail		= pNode;

			return true;
	}

	// ���̶߳�������£�������
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

//���Դ���
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