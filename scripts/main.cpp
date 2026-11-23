#include <windows.h>
#include <string>

int seconds_B = 0; // seconds for browser
int seconds_M = 0; // seconds for minecraft
std::wstring currExe; // current foreground exe

const std::wstring MINECRAFT = L"javaw.exe";
const std::wstring BRAVE = L"brave.exe";
const std::wstring FIREFOX = L"firefox.exe";
const std::wstring CHROME = L"chrome.exe";

const DWORD IDLE_THRES_MS = 1 * 60 * 1000;

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
 
        // index of last slash, index will be the of the second slash
        size_t slash = exe.find_last_of(L'\\');

        if (slash != std::wstring::npos) exe = exe.substr(slash + 1); // npos basically means null
    }

    CloseHandle(process);
    return exe;
}

bool IsIdle()
{
    LASTINPUTINFO last = {};
    last.cbSize = sizeof(last); // this tells windows which version of the LASTINPUTINFO struct we're using
    if(!GetLastInputInfo(&last)) return false;

    DWORD idleMs = GetTickCount() - last.dwTime;
    return idleMs > IDLE_THRES_MS;
}

void DrawLine(HDC hdc, int y, const wchar_t* name, int secs, bool active)
{
    SetTextColor(hdc, active ? RGB(120, 255, 120) : RGB(255, 255, 255));
 
    TextOutW(hdc, 10, y, name, lstrlenW(name));
 
    wchar_t time[16]; // buffer for the time text, I opted for this instead of wstring bc it is simpler and more elegant
    wsprintfW(time, L"%02d:%02d:%02d", secs / 3600, (secs / 60) % 60, secs % 60); // converting seconds to hours:minutes:seconds like 00:00:00
 
    TextOutW(hdc, 200, y, time, lstrlenW(time));
}

LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wp, LPARAM lp)
{
    switch (msg)
    {
    case WM_CREATE:
        RegisterHotKey(hwnd, 1, MOD_ALT, 'Q'); // register hotkey to quit to be alt + q
        SetTimer(hwnd, 1, 1000, nullptr);
        return 0;

    case WM_TIMER:
    {
        if(IsIdle()) return 0;
        currExe = getForegroundExe();
        if(!_wcsicmp(currExe.c_str(), MINECRAFT.c_str())) seconds_M++; // _wcsicmp compares wstrings non case sensitive
        else if(!_wcsicmp(currExe.c_str(), BRAVE.c_str()) || !_wcsicmp(currExe.c_str(), FIREFOX.c_str()) || !_wcsicmp(currExe.c_str(), CHROME.c_str())) seconds_B++;
        InvalidateRect(hwnd, nullptr, TRUE); // triggers WM_PAINT event
        return 0;
    }
    case WM_PAINT:
    {
        PAINTSTRUCT ps;
        HDC hdc = BeginPaint(hwnd, &ps);

        SetBkMode(hdc, TRANSPARENT);
        
        DrawLine(hdc, 10, L"Minecraft", seconds_M, !IsIdle() && !_wcsicmp(currExe.c_str(), MINECRAFT.c_str()));
        DrawLine(hdc, 30, L"Homework", seconds_B, !IsIdle() && (!_wcsicmp(currExe.c_str(), BRAVE.c_str()) || !_wcsicmp(currExe.c_str(), FIREFOX.c_str()) || !_wcsicmp(currExe.c_str(), CHROME.c_str())));

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
    while (GetMessageW(&msg, nullptr, 0, 0)) // getting events/messages
    {
        TranslateMessage(&msg);
        DispatchMessageW(&msg);
    }
    return 0;
}