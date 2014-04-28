#pragma once
#include <iostream>

// 专门为了自己的命名空间
#define _CDH_BEGIN namespace CDH { 
#define _CDH_END   }

// 这个宏调用c运行库函数_beginthreadex
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

//旋转次数
#define SpinCount 4000  