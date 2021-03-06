/**************************************************************
Desc:一些共同的宏
12/25/2013 CDH]

***************************************************************/
#pragma once

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


