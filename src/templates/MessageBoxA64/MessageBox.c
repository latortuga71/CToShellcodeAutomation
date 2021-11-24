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
    //LPVOID exit_thread = get_func_by_name((HMODULE)base,(LPSTR)"ExitThread");
    // define function definitions we just acquired
    HMODULE(WINAPI * _LoadLibraryA)(LPCSTR lpLibFileName) = (HMODULE(WINAPI*)(LPCSTR))load_lib;
    //void(WINAPI *_ExitThread)(DWORD dwExitCode) = (void(WINAPI*)(DWORD))exit_thread;
    FARPROC(WINAPI * _GetProcAddress)(HMODULE hModule, LPCSTR lpProcName) = (FARPROC(WINAPI*)(HMODULE,LPCSTR))get_proc;
    // Load needed libraries 
    LPVOID u32_dll = _LoadLibraryA("user32.dll");
    // create function definitons
    typedef unsigned long(WINAPI * pMessageBoxA)(IN  HWND, IN LPCSTR, IN LPCSTR, IN UINT);
    pMessageBoxA _MessageBoxA = (pMessageBoxA)_GetProcAddress((HMODULE)u32_dll,"MessageBoxA");
    // start
    if (_MessageBoxA == NULL) return 1;
    _MessageBoxA(0,"Hello World","Demo",MB_OK);
    return 0;
}
