// dllmain.cpp : Defines the entry point for the DLL application.
#include "pch.h"
#include <windows.h>


BOOL APIENTRY DllMain( HMODULE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
                     )
{
    switch (ul_reason_for_call)
    {
    case DLL_PROCESS_ATTACH:
    case DLL_THREAD_ATTACH:
    case DLL_THREAD_DETACH:
    case DLL_PROCESS_DETACH:
        break;
    }
    return TRUE;
}

extern "C" __declspec(dllexport) void StartKeyboardHook();
extern "C" __declspec(dllexport) void StopKeyboardHook();
extern "C" __declspec(dllexport) char GetLastKey();


#define KEYDOWN 0x8000
#define WASPRESSED 0x0001

char VkCodeToChar(UINT vkCode) {
    if (vkCode >= 0x30 && vkCode <= 0x39) {
        // Handle digits 0-9
        return '0' + (vkCode - 0x30);
    }
    else if (vkCode >= 0x41 && vkCode <= 0x5A) {
        // Handle uppercase letters A-Z
        return 'a' + (vkCode - 0x41);
    }
    else if (vkCode >= 0x61 && vkCode <= 0x7A) {
        // Handle lowercase letters a-z
        return 'a' + (vkCode - 0x61);
    }
    else {
        return '\0'; // Invalid or non-alphanumeric key
    }
}

//params
static volatile char g_lastKey = '\0';
static HHOOK g_hook = NULL;

LRESULT CALLBACK one_to_nine_keyboard_hook(int nCode, WPARAM wParam, LPARAM lParam)
{
    if (nCode == HC_ACTION)
    {
        KBDLLHOOKSTRUCT* hook_event = (KBDLLHOOKSTRUCT*)lParam;
        char pressed_char = VkCodeToChar(hook_event->vkCode);

        if (pressed_char != '\0')
        {
            if (wParam == WM_KEYUP)
            {
                g_lastKey = pressed_char;   // store key for C# to read
            }
        }
    }

    return CallNextHookEx(NULL, nCode, wParam, lParam);
}

//exposed functions

extern "C" __declspec(dllexport) void StartKeyboardHook()
{
    g_hook = SetWindowsHookEx(WH_KEYBOARD_LL, one_to_nine_keyboard_hook, GetModuleHandle(NULL), 0);
}

extern "C" __declspec(dllexport) void StopKeyboardHook()
{
    if (g_hook)
    {
        UnhookWindowsHookEx(g_hook);
        g_hook = NULL;
    }
}

extern "C" __declspec(dllexport) char GetLastKey()
{
    char key = g_lastKey;
    g_lastKey = '\0';
    return key;
}
