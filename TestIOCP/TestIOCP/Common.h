#pragma once
#include <iostream>

// ר��Ϊ���Լ��������ռ�
#define _CDH_BEGIN namespace CDH { 
#define _CDH_END   }

// ��������c���п⺯��_beginthreadex
typedef unsigned (__stdcall *PTHREAD_START) (void *);

#define chBEGINTHREADEX(psa, cbStackSize, pfnStartAddr, \
	pvParam, dwCreateFlags, pdwThreadId)                \
	((HANDLE)_beginthreadex(                        \
	(void *)        (psa),                         \
	(unsigned)      (cbStackSize),                 \
	(PTHREAD_START) (pfnStartAddr),                \
	(void *)        (pvParam),                     \
	(unsigned)      (dwCreateFlags),               \
	(unsigned *)    (pdwThreadId)))


#define OutPut(str) std::cout << str << std::endl;

//��ת����
#define SpinCount 4000  