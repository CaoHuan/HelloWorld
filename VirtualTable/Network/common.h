/*******************************************************************
** �ļ���:	D:\Projects\HelloWorld\VirtualTable\Network\common.h
** ������:	�ܻ�
** ��  ��:	2014/3/13 14:56
** ��  ��:	1.0
** ��  ��:	
** Ӧ  ��:  	
	
**************************** �޸ļ�¼ ******************************
** �޸���: 
** ��  ��: 
** ��  ��: 
********************************************************************/

#pragma once

// ר��Ϊ���Լ��������ռ�
#define _CDH_BEGIN namespace CDH { 
#define _CDH_END   }


typedef unsigned (__stdcall *PTHREAD_START) (void *);

// ��������c���п⺯��_beginthreadex
#define chBEGINTHREADEX(psa, cbStackSize, pfnStartAddr, \
	pvParam, dwCreateFlags, pdwThreadId)                \
	((HANDLE)_beginthreadex(                        \
	(void *)        (psa),                         \
	(unsigned)      (cbStackSize),                 \
	(PTHREAD_START) (pfnStartAddr),                \
	(void *)        (pvParam),                     \
	(unsigned)      (dwCreateFlags),               \
	(unsigned *)    (pdwThreadId)))
