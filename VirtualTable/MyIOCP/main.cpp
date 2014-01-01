#include <iostream>
#include <fstream>

#include "MyIOCP.h"
using namespace std;
using namespace CDH;

static int i = 0;
FILE* file;
void ProcessFun(int socket, char* data, unsigned int length)
{
   
    fwrite(data, length, length, file);
    
    IOCPMgr->Send(socket, data, length);
}
int main()
{
    file = fopen("test.txt", "wt");
    IOCPMgr->SetInit(30);
    WrapSocket wrapSocket(6000);
    IOCPMgr->AddListenerSocket(&wrapSocket, ProcessFun);
    while (true)
    {

        IOCPMgr->Update();
        Sleep(0);
    }
    return 0;
}