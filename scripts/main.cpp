#include <windows.h>

int g_secondsB = 0; // seconds for browser
int g_secondsM = 0; // seconds for minecraft

LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wp, LPARAM lp)
{
    switch (msg)
    {
    case WM_CREATE:
        RegisterHotKey(hwnd, 1, MOD_CONTROL | MOD_ALT, 'Q');   // register hotkey to quit to be alt + q
        SetTimer(hwnd, 1, 1000, nullptr);
        return 0;

    case WM_TIMER:
        g_secondsM++;
        return 0;
 
    case WM_HOTKEY: // triggers when previously registered hot key presses
        DestroyWindow(hwnd); // trigger WM_DESTROY event
        return 0;
 
    case WM_DESTROY:
        KillTimer(hwnd, 1);
        PostQuitMessage(0); // quit program
        return 0;
    }
    return DefWindowProcW(hwnd, msg, wp, lp); // other windows procedures I don't handle are handled with this
}
 
// entry point, win32 api's replacement for main
int WINAPI wWinMain(HINSTANCE hInst, HINSTANCE, PWSTR, int ) 
{
    WNDCLASSW wc = {};
    wc.lpfnWndProc = WndProc;
    wc.hInstance = hInst;
    wc.lpszClassName = L"SimpleOverlay";
    wc.hbrBackground = CreateSolidBrush(RGB(20, 20, 20));
    RegisterClassW(&wc);
 
    HWND hwnd = CreateWindowExW(
        WS_EX_TOPMOST | WS_EX_LAYERED | WS_EX_TRANSPARENT | WS_EX_TOOLWINDOW | WS_EX_NOACTIVATE,
        wc.lpszClassName, L"Overlay", WS_POPUP,
        20, 20, 300, 70,
        nullptr, nullptr, hInst, nullptr);
 
    SetLayeredWindowAttributes(hwnd, 0, 200, LWA_ALPHA);
    ShowWindow(hwnd, SW_SHOWNOACTIVATE);
 
    MSG msg;
    while (GetMessageW(&msg, nullptr, 0, 0))
    {
        TranslateMessage(&msg);
        DispatchMessageW(&msg);
    }
    return 0;
}