/*******************************************************************
** 文件名:	D:\Projects\HelloWorld\VirtualTable\Network\common.h
** 创建人:	曹焕
** 日  期:	2014/3/13 14:56
** 版  本:	1.0
** 描  述:	
** 应  用:  	
	
**************************** 修改记录 ******************************
** 修改人: 
** 日  期: 
** 描  述: 
********************************************************************/

#pragma once

// 专门为了自己的命名空间
#define _CDH_BEGIN namespace CDH { 
#define _CDH_END   }


typedef unsigned (__stdcall *PTHREAD_START) (void *);

// 这个宏调用c运行库函数_beginthreadex
#define chBEGINTHREADEX(psa, cbStackSize, pfnStartAddr, \
	pvParam, dwCreateFlags, pdwThreadId)                \
	((HANDLE)_beginthreadex(                        \
	(void *)        (psa),                         \
	(unsigned)      (cbStackSize),                 \
	(PTHREAD_START) (pfnStartAddr),                \
	(void *)        (pvParam),                     \
	(unsigned)      (dwCreateFlags),               \
	(unsigned *)    (pdwThreadId)))
