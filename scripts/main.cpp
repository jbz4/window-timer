#include <windows.h>
#include <string>

#define MINECRAFT L"javaw.exe"
#define BROWSER L"brave.exe"

int seconds_B = 0; // seconds for browser
int seconds_M = 0; // seconds for minecraft
std::wstring currExe; // current foreground exe

std::wstring getForegroundExe()
{
    HWND foreground = GetForegroundWindow();
    if(!foreground) return L"";

    DWORD pid = 0;
    GetWindowThreadProcessId(foreground, &pid);
    if(pid == 0) return L"";

    HANDLE process = OpenProcess(PROCESS_QUERY_LIMITED_INFORMATION, FALSE, pid);
    if(!process) return L"";

    wchar_t path[MAX_PATH];
    DWORD len = MAX_PATH;
    std::wstring exe;

    if (QueryFullProcessImageNameW(process, 0, path, &len)) // get foreground exe path
    {
        exe.assign(path, len);
 
        // index of last slash
        size_t slash = exe.find_last_of(L'\\');

        if (slash != std::wstring::npos) exe = exe.substr(slash + 1); // npos basically means null
    }

    CloseHandle(process);
    return exe;
}

LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wp, LPARAM lp)
{
    switch (msg)
    {
    case WM_CREATE:
        RegisterHotKey(hwnd, 1, MOD_CONTROL | MOD_ALT, 'Q'); // register hotkey to quit to be alt + q
        SetTimer(hwnd, 1, 1000, nullptr);
        return 0;

    case WM_TIMER:
    {
        currExe = getForegroundExe();
        if(currExe == MINECRAFT) seconds_M++;
        else if(currExe == BROWSER) seconds_B++;
        InvalidateRect(hwnd, nullptr, TRUE);
        return 0;
    }
    case WM_PAINT: // testing
    {
        PAINTSTRUCT ps;
        HDC hdc = BeginPaint(hwnd, &ps);

        wchar_t text[64];

        SetBkMode(hdc, TRANSPARENT);
        SetTextColor(hdc, RGB(255, 255, 255));
        TextOutW(hdc, 10, 10, currExe.c_str(), (int)currExe.size());

        wsprintfW(text, L"A: %d", seconds_B);
        TextOutW(hdc, 10, 30, text, lstrlen(text));
        // TextOutW(hdc, 10, 30, )
        EndPaint(hwnd, &ps);
        return 0;
    }
 
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
 
// entry point, basically win32 api's replacement for int main()
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