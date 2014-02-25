#include <windows.h>
#include <iostream>
#include <fstream>

using namespace std;




PBITMAPINFO CreateBitmapInfoStruct(HBITMAP hBmp)
{ 
    BITMAP bmp; 
    PBITMAPINFO pbmi; 
    WORD    cClrBits; 

    // Retrieve the bitmap color format, width, and height.  
    GetObject(hBmp, sizeof(BITMAP), (LPSTR)&bmp); 

    // Convert the color format to a count of bits.  
    cClrBits = (WORD)(bmp.bmPlanes * bmp.bmBitsPixel); 
    if (cClrBits == 1) 
        cClrBits = 1; 
    else if (cClrBits <= 4) 
        cClrBits = 4; 
    else if (cClrBits <= 8) 
        cClrBits = 8; 
    else if (cClrBits <= 16) 
        cClrBits = 16; 
    else if (cClrBits <= 24) 
        cClrBits = 24; 
    else cClrBits = 32; 

    // Allocate memory for the BITMAPINFO structure. (This structure  
    // contains a BITMAPINFOHEADER structure and an array of RGBQUAD  
    // data structures.)  

    if (cClrBits < 24) 
        pbmi = (PBITMAPINFO) LocalAlloc(LPTR, 
        sizeof(BITMAPINFOHEADER) + 
        sizeof(RGBQUAD) * (1<< cClrBits)); 

    // There is no RGBQUAD array for these formats: 24-bit-per-pixel or 32-bit-per-pixel 

    else 
        pbmi = (PBITMAPINFO) LocalAlloc(LPTR, 
        sizeof(BITMAPINFOHEADER)); 

    // Initialize the fields in the BITMAPINFO structure.  

    pbmi->bmiHeader.biSize = sizeof(BITMAPINFOHEADER); 
    pbmi->bmiHeader.biWidth = bmp.bmWidth; 
    pbmi->bmiHeader.biHeight = bmp.bmHeight; 
    pbmi->bmiHeader.biPlanes = bmp.bmPlanes; 
    pbmi->bmiHeader.biBitCount = bmp.bmBitsPixel; 
    if (cClrBits < 24) 
        pbmi->bmiHeader.biClrUsed = (1<<cClrBits); 

    // If the bitmap is not compressed, set the BI_RGB flag.  
    pbmi->bmiHeader.biCompression = BI_RGB; 

    // Compute the number of bytes in the array of color  
    // indices and store the result in biSizeImage.  
    // The width must be DWORD aligned unless the bitmap is RLE 
    // compressed. 
    pbmi->bmiHeader.biSizeImage = ((pbmi->bmiHeader.biWidth * cClrBits +31) & ~31) /8
        * pbmi->bmiHeader.biHeight; 
    // Set biClrImportant to 0, indicating that all of the  
    // device colors are important.  
    pbmi->bmiHeader.biClrImportant = 0; 
    return pbmi; 
} 


int CaptureAnImage(HWND);




void CreateBMPFileMine(LPTSTR str)
{
    HDC hDC = GetDC(NULL);
    HDC comHDC = CreateCompatibleDC(hDC);
    int width = GetSystemMetrics(SM_CXSCREEN);
    int height = GetSystemMetrics(SM_CYSCREEN);

    HBITMAP hBitmap = CreateCompatibleBitmap(hDC, width, height);
    SelectObject(comHDC, hBitmap);
    if (!BitBlt(comHDC, 0, 0, width, height, hDC, 0, 0, SRCCOPY))
    {
        cout << "error !!!" << endl;
    }

   
    BITMAPINFOHEADER bmiHeader; 

    bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
    bmiHeader.biWidth = width;
    bmiHeader.biHeight = height;
    bmiHeader.biPlanes = 1;
    bmiHeader.biBitCount = 16;
    bmiHeader.biCompression = BI_RGB;
    bmiHeader.biSizeImage = 0;
    bmiHeader.biXPelsPerMeter = 0;
    bmiHeader.biYPelsPerMeter = 0;
    bmiHeader.biClrUsed = 0;
    bmiHeader.biClrImportant = 0;

    DWORD dwBmpSize = ((width * bmiHeader.biBitCount + 31) / 32) * 4 * height;

    DWORD dwSizeofDIB = dwBmpSize + sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER);

    BITMAPFILEHEADER bmfHeader;

    // bitmap数据开始写入的偏移
    bmfHeader.bfOffBits = (DWORD)sizeof(BITMAPFILEHEADER) + (DWORD)sizeof(BITMAPINFOHEADER);

    // 文件的大小
    bmfHeader.bfSize = dwSizeofDIB;

    bmfHeader.bfType = 0x4D42;  // 标识这是个BMP文件

    char * lpbitmap = new char[dwBmpSize];
    ZeroMemory(lpbitmap, dwBmpSize);

    GetDIBits(comHDC, hBitmap, 0, (UINT)height, lpbitmap, (BITMAPINFO*)&bmiHeader, DIB_RGB_COLORS);

    DWORD dwBytesWritten = 0;
    int totallSize = dwBmpSize + sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER);

    // 写入文件头信息
    ofstream of("tt.bmp", ios_base::binary);

    char* allData = new char[dwBmpSize + sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER)];
    ZeroMemory(allData, dwBmpSize + sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER));
    memcpy(allData, (void*)&bmfHeader, sizeof(BITMAPFILEHEADER));
    memcpy(allData + sizeof(BITMAPFILEHEADER), (void*)&bmiHeader, sizeof(BITMAPINFOHEADER));
    memcpy(allData + sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER), lpbitmap, dwBmpSize);

    of.write(allData, dwBmpSize + sizeof(BITMAPINFOHEADER) + sizeof(BITMAPFILEHEADER));
    
    of.write((char *)&bmfHeader, sizeof(BITMAPFILEHEADER) );
    of.write((char *)&bmiHeader, sizeof(BITMAPINFOHEADER) );
    of.write(lpbitmap, dwBmpSize);
    of.close();
 
    // 创建文件
    HANDLE hFile = CreateFile(str, GENERIC_WRITE, 0, NULL,
        CREATE_NEW, FILE_ATTRIBUTE_NORMAL, NULL);

    // 	WriteFile(hFile, (LPSTR)&bmfHeader, sizeof(BITMAPFILEHEADER), &dwBytesWritten, NULL);
    // 	WriteFile(hFile, (LPSTR)&bmiHeader, sizeof(BITMAPINFOHEADER), &dwBytesWritten, NULL);
    // 	WriteFile(hFile, (LPSTR)lpbitmap, dwBmpSize, &dwBytesWritten, NULL);
    WriteFile(hFile, allData, dwBmpSize + sizeof(BITMAPINFOHEADER) + sizeof(BITMAPFILEHEADER), &dwBytesWritten, NULL);

    //
    // 	GlobalUnlock(hDIB);
    // 	GlobalFree(hDIB);
    CloseHandle(hFile);

    DeleteObject(comHDC);

}

int main()
{
    int width = GetSystemMetrics(SM_CXSCREEN);
    int height = GetSystemMetrics(SM_CYSCREEN);

    //HDC hdc = GetDC(NULL);
    //   	HDC myHDC = CreateCompatibleDC(hdc);
    //   	BitBlt(hdc, 0, 0, width, height, myHDC, 0, 0, SRCCOPY);
    //  	HBITMAP bitmap = CreateCompatibleBitmap(myHDC, width, height);
    //  	PBITMAPINFO mapInfo = CreateBitmapInfoStruct(bitmap);
    // 	//CreateBMPFile(L"ttttt.bmp", mapInfo, bitmap, myHDC);
    // 	//CaptureAnImage(GetDesktopWindow());
    CreateBMPFileMine(L"test.bmp");
}

int CaptureAnImage(HWND hWnd)
{
    HDC hdcScreen;
    HDC hdcWindow;
    HDC hdcMemDC = NULL;
    HBITMAP hbmScreen = NULL;
    BITMAP bmpScreen;

    // Retrieve the handle to a display device context for the client  
    // area of the window. 
    hdcScreen = GetDC(NULL);  // 找到整个屏幕的DC
    hdcWindow = GetDC(hWnd);	

    // Create a compatible DC which is used in a BitBlt from the window DC
    hdcMemDC = CreateCompatibleDC(hdcWindow); 

    if(!hdcMemDC)
    {
        MessageBox(hWnd, L"CreateCompatibleDC has failed", L"Failed", MB_OK);
        goto done;
    }

    // Get the client area for size calculation
    RECT rcClient;
    GetClientRect(hWnd, &rcClient);

    //This is the best stretch mode
    SetStretchBltMode(hdcWindow,HALFTONE);

    //The source DC is the entire screen and the destination DC is the current window (HWND)
    if(!StretchBlt(hdcWindow, 
        0,0, 
        rcClient.right, rcClient.bottom, 
        hdcScreen, 
        0,0,
        GetSystemMetrics (SM_CXSCREEN),
        GetSystemMetrics (SM_CYSCREEN),
        SRCCOPY))
    {
        MessageBox(hWnd, L"StretchBlt has failed",L"Failed", MB_OK);
        goto done;
    }

    // Create a compatible bitmap from the Window DC
    hbmScreen = CreateCompatibleBitmap(hdcWindow, rcClient.right-rcClient.left, rcClient.bottom-rcClient.top);

    if(!hbmScreen)
    {
        MessageBox(hWnd, L"CreateCompatibleBitmap Failed",L"Failed", MB_OK);
        goto done;
    }

    // Select the compatible bitmap into the compatible memory DC.
    SelectObject(hdcMemDC,hbmScreen);

    // Bit block transfer into our compatible memory DC.
    if(!BitBlt(hdcMemDC, 
        0,0, 
        rcClient.right-rcClient.left, rcClient.bottom-rcClient.top, 
        hdcWindow, 
        0,0,
        SRCCOPY))
    {
        MessageBox(hWnd, L"BitBlt has failed", L"Failed", MB_OK);
        goto done;
    }

    // Get the BITMAP from the HBITMAP
    GetObject(hbmScreen,sizeof(BITMAP),&bmpScreen);

    BITMAPFILEHEADER   bmfHeader;    
    BITMAPINFOHEADER   bi;

    bi.biSize = sizeof(BITMAPINFOHEADER);    
    bi.biWidth = bmpScreen.bmWidth;    
    bi.biHeight = bmpScreen.bmHeight;  
    bi.biPlanes = 1;    
    bi.biBitCount = 32;    
    bi.biCompression = BI_RGB;    
    bi.biSizeImage = 0;  
    bi.biXPelsPerMeter = 0;    
    bi.biYPelsPerMeter = 0;    
    bi.biClrUsed = 0;    
    bi.biClrImportant = 0;

    DWORD dwBmpSize = ((bmpScreen.bmWidth * bi.biBitCount + 31) / 32) * 4 * bmpScreen.bmHeight;

    // Starting with 32-bit Windows, GlobalAlloc and LocalAlloc are implemented as wrapper functions that 
    // call HeapAlloc using a handle to the process's default heap. Therefore, GlobalAlloc and LocalAlloc 
    // have greater overhead than HeapAlloc.
    HANDLE hDIB = GlobalAlloc(GHND,dwBmpSize); 
    char *lpbitmap = (char *)GlobalLock(hDIB);    

    // Gets the "bits" from the bitmap and copies them into a buffer 
    // which is pointed to by lpbitmap.
    GetDIBits(hdcWindow, hbmScreen, 0,
        (UINT)bmpScreen.bmHeight,
        lpbitmap,
        (BITMAPINFO *)&bi, DIB_RGB_COLORS);

    // A file is created, this is where we will save the screen capture.
    HANDLE hFile = CreateFile(L"captureqwsx.bmp",
        GENERIC_WRITE,
        0,
        NULL,
        CREATE_ALWAYS,
        FILE_ATTRIBUTE_NORMAL, NULL);   

    // Add the size of the headers to the size of the bitmap to get the total file size
    DWORD dwSizeofDIB = dwBmpSize + sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER);

    //Offset to where the actual bitmap bits start.
    bmfHeader.bfOffBits = (DWORD)sizeof(BITMAPFILEHEADER) + (DWORD)sizeof(BITMAPINFOHEADER); 

    //Size of the file
    bmfHeader.bfSize = dwSizeofDIB; 

    //bfType must always be BM for Bitmaps
    bmfHeader.bfType = 0x4D42; //BM   

    DWORD dwBytesWritten = 0;
    WriteFile(hFile, (LPSTR)&bmfHeader, sizeof(BITMAPFILEHEADER), &dwBytesWritten, NULL);
    WriteFile(hFile, (LPSTR)&bi, sizeof(BITMAPINFOHEADER), &dwBytesWritten, NULL);
    WriteFile(hFile, (LPSTR)lpbitmap, dwBmpSize, &dwBytesWritten, NULL);

    //Unlock and Free the DIB from the heap
    GlobalUnlock(hDIB);    
    GlobalFree(hDIB);

    //Close the handle for the file that was created
    CloseHandle(hFile);

    //Clean up
done:
    DeleteObject(hbmScreen);
    DeleteObject(hdcMemDC);
    ReleaseDC(NULL,hdcScreen);
    ReleaseDC(hWnd,hdcWindow);

    return 0;
}
