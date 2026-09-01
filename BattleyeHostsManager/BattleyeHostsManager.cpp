// BattleyeHostsManager.cpp : Defines the entry point for the application.
//

#include "framework.h"
#include "BattleyeHostsManager.h"
#include <fstream>
#include <string>
#include <shlobj.h>
#include <vector>

#define MAX_LOADSTRING 100
#define IDC_BLOCK_BTN 1001
#define IDC_UNBLOCK_BTN 1002
#define IDC_STATUS_TEXT 1003
#define IDC_DOMAINS_LIST 1004

// Global Variables:
HINSTANCE hInst;
WCHAR szTitle[MAX_LOADSTRING];
WCHAR szWindowClass[MAX_LOADSTRING];
HWND hBlockBtn = nullptr;
HWND hUnblockBtn = nullptr;
HWND hStatusText = nullptr;
HFONT hTitleFont = nullptr;
HFONT hStatusFont = nullptr;
HFONT hSmallFont = nullptr;
HFONT hLargeFont = nullptr;

// Domains to block
const std::vector<std::wstring> g_domains = {
    L"paradise-s1.battleye.com",
    L"test-s1.battleye.com",
    L"paradiseenhanced-s1.battleye.com"
};

// Forward declarations
ATOM MyRegisterClass(HINSTANCE hInstance);
BOOL InitInstance(HINSTANCE, int);
LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

// Helper functions
std::wstring GetHostsPath()
{
    wchar_t systemPath[MAX_PATH];
    GetSystemDirectoryW(systemPath, MAX_PATH);
    return std::wstring(systemPath) + L"\\drivers\\etc\\hosts";
}

bool AddToHosts()
{
    try
    {
        std::wstring hostsPath = GetHostsPath();

        // Read existing hosts file
        std::wifstream readFile(hostsPath);
        std::wstring fileContent;
        std::wstring line;

        if (readFile.is_open())
        {
            while (std::getline(readFile, line))
            {
                fileContent += line + L"\n";
            }
            readFile.close();
        }

        // Check if domains already exist
        std::wofstream writeFile(hostsPath, std::ios::app);
        if (!writeFile.is_open())
            return false;

        for (const auto& domain : g_domains)
        {
            if (fileContent.find(domain) == std::wstring::npos)
            {
                writeFile << L"0.0.0.0 " << domain << L"\n";
            }
        }

        writeFile.close();
        return true;
    }
    catch (...)
    {
        return false;
    }
}

bool RemoveFromHosts()
{
    try
    {
        std::wstring hostsPath = GetHostsPath();
        std::wifstream readFile(hostsPath);
        std::wstring tempPath = hostsPath + L".tmp";
        std::wofstream writeFile(tempPath);

        if (!readFile.is_open() || !writeFile.is_open())
            return false;

        std::wstring line;

        while (std::getline(readFile, line))
        {
            bool shouldRemove = false;

            for (const auto& domain : g_domains)
            {
                if (line.find(domain) != std::wstring::npos)
                {
                    shouldRemove = true;
                    break;
                }
            }

            if (!shouldRemove && !line.empty())
            {
                writeFile << line << L"\n";
            }
        }

        readFile.close();
        writeFile.close();

        DeleteFileW(hostsPath.c_str());
        MoveFileW(tempPath.c_str(), hostsPath.c_str());

        return true;
    }
    catch (...)
    {
        return false;
    }
}

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
    _In_opt_ HINSTANCE hPrevInstance,
    _In_ LPWSTR    lpCmdLine,
    _In_ int       nCmdShow)
{
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);

    LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
    LoadStringW(hInstance, IDC_BATTLEYEHOSTSMANAGER, szWindowClass, MAX_LOADSTRING);
    MyRegisterClass(hInstance);

    if (!InitInstance(hInstance, nCmdShow))
    {
        return FALSE;
    }

    MSG msg;

    while (GetMessage(&msg, nullptr, 0, 0))
    {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    return (int)msg.wParam;
}

ATOM MyRegisterClass(HINSTANCE hInstance)
{
    WNDCLASSEXW wcex;

    wcex.cbSize = sizeof(WNDCLASSEX);
    wcex.style = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc = WndProc;
    wcex.cbClsExtra = 0;
    wcex.cbWndExtra = 0;
    wcex.hInstance = hInstance;
    wcex.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_BATTLEYEHOSTSMANAGER));
    wcex.hCursor = LoadCursor(nullptr, IDC_ARROW);
    wcex.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);
    wcex.lpszMenuName = nullptr;
    wcex.lpszClassName = szWindowClass;
    wcex.hIconSm = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

    return RegisterClassExW(&wcex);
}

BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
    hInst = hInstance;

    HWND hWnd = CreateWindowW(szWindowClass, L"BattlEye Hosts Manager",
        WS_OVERLAPPEDWINDOW & ~WS_MAXIMIZEBOX & ~WS_SIZEBOX,
        100, 100, 500, 420, nullptr, nullptr, hInstance, nullptr);

    if (!hWnd)
    {
        return FALSE;
    }

    ShowWindow(hWnd, nCmdShow);
    UpdateWindow(hWnd);

    return TRUE;
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    static HBRUSH hGrayBrush = nullptr;

    switch (message)
    {
    case WM_CREATE:
    {
        hGrayBrush = CreateSolidBrush(RGB(240, 240, 240));

        // Инициализация шрифтов
        hTitleFont = CreateFontW(-20, 0, 0, 0, FW_BOLD, FALSE, FALSE, FALSE,
            DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
            CLEARTYPE_QUALITY, DEFAULT_PITCH | FF_SWISS, L"Segoe UI");

        hStatusFont = CreateFontW(-14, 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE,
            DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
            CLEARTYPE_QUALITY, DEFAULT_PITCH | FF_SWISS, L"Segoe UI");

        // Увеличенный шрифт для администратора
        hLargeFont = CreateFontW(-14, 0, 0, 0, FW_SEMIBOLD, FALSE, FALSE, FALSE,
            DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
            CLEARTYPE_QUALITY, DEFAULT_PITCH | FF_SWISS, L"Segoe UI");

        // Увеличенный шрифт для подписи автора
        hSmallFont = CreateFontW(-12, 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE,
            DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
            CLEARTYPE_QUALITY, DEFAULT_PITCH | FF_SWISS, L"Segoe UI");

        // Title
        HWND hTitleText = CreateWindowExW(0, L"STATIC", L"BattlEye Hosts Manager",
            WS_VISIBLE | WS_CHILD | SS_CENTER,
            10, 15, 465, 30, hWnd, nullptr, hInst, nullptr);
        SendMessageW(hTitleText, WM_SETFONT, (WPARAM)hTitleFont, TRUE);

        // Info text
        HWND hInfoText = CreateWindowExW(0, L"STATIC",
            L"Block/Unblock BattlEye domains:",
            WS_VISIBLE | WS_CHILD | SS_LEFT,
            20, 55, 445, 20, hWnd, nullptr, hInst, nullptr);
        SendMessageW(hInfoText, WM_SETFONT, (WPARAM)hStatusFont, TRUE);

        // Domains list with gray background
        HWND hListText = CreateWindowExW(0, L"STATIC",
            L"  • paradise-s1.battleye.com\r\n  • test-s1.battleye.com\r\n  • paradiseenhanced-s1.battleye.com",
            WS_VISIBLE | WS_CHILD | SS_LEFT,
            20, 80, 445, 70, hWnd, (HMENU)IDC_DOMAINS_LIST, hInst, nullptr);
        SendMessageW(hListText, WM_SETFONT, (WPARAM)hStatusFont, TRUE);

        // Block button
        hBlockBtn = CreateWindowExW(0, L"BUTTON", L"Block Domains",
            WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_PUSHBUTTON,
            35, 165, 180, 40, hWnd, (HMENU)IDC_BLOCK_BTN, hInst, nullptr);
        SendMessageW(hBlockBtn, WM_SETFONT, (WPARAM)hStatusFont, TRUE);

        // Unblock button
        hUnblockBtn = CreateWindowExW(0, L"BUTTON", L"Unblock Domains",
            WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_PUSHBUTTON,
            270, 165, 180, 40, hWnd, (HMENU)IDC_UNBLOCK_BTN, hInst, nullptr);
        SendMessageW(hUnblockBtn, WM_SETFONT, (WPARAM)hStatusFont, TRUE);

        // Status text
        hStatusText = CreateWindowExW(0, L"STATIC", L"Ready",
            WS_VISIBLE | WS_CHILD | SS_CENTER,
            10, 220, 465, 25, hWnd, (HMENU)IDC_STATUS_TEXT, hInst, nullptr);
        SendMessageW(hStatusText, WM_SETFONT, (WPARAM)hStatusFont, TRUE);

        // Footer text (Run as Administrator)
        HWND hFooterText = CreateWindowExW(0, L"STATIC",
            L"Run as Administrator for this to work",
            WS_VISIBLE | WS_CHILD | SS_CENTER,
            10, 275, 465, 25, hWnd, nullptr, hInst, nullptr);
        SendMessageW(hFooterText, WM_SETFONT, (WPARAM)hLargeFont, TRUE);

        // Copyright text (by nixrt)
        HWND hCopyrightText = CreateWindowExW(0, L"STATIC", L"by nixrt",
            WS_VISIBLE | WS_CHILD | SS_CENTER,
            10, 310, 465, 20, hWnd, nullptr, hInst, nullptr);
        SendMessageW(hCopyrightText, WM_SETFONT, (WPARAM)hSmallFont, TRUE);

        return 0;
    }

    case WM_COMMAND:
    {
        int wmId = LOWORD(wParam);
        switch (wmId)
        {
        case IDC_BLOCK_BTN:
        {
            if (AddToHosts())
            {
                SetWindowTextW(hStatusText, L"Success: Domains blocked!");
                MessageBoxW(hWnd, L"Domains added to hosts file.\nRestart Paradise.",
                    L"Success", MB_OK | MB_ICONINFORMATION);
            }
            else
            {
                SetWindowTextW(hStatusText, L"Error: Admin rights needed");
                MessageBoxW(hWnd, L"Failed!\nRun as Administrator.",
                    L"Error", MB_OK | MB_ICONERROR);
            }
        }
        break;

        case IDC_UNBLOCK_BTN:
        {
            if (RemoveFromHosts())
            {
                SetWindowTextW(hStatusText, L"Success: Domains unblocked!");
                MessageBoxW(hWnd, L"Domains removed from hosts file.",
                    L"Success", MB_OK | MB_ICONINFORMATION);
            }
            else
            {
                SetWindowTextW(hStatusText, L"Error: Admin rights needed");
                MessageBoxW(hWnd, L"Failed!\nRun as Administrator.",
                    L"Error", MB_OK | MB_ICONERROR);
            }
        }
        break;

        default:
            return DefWindowProc(hWnd, message, wParam, lParam);
        }
        return 0;
    }

    case WM_CTLCOLORSTATIC:
    {
        HDC hdc = (HDC)wParam;
        HWND hCtrl = (HWND)lParam;

        // Gray background for domains list
        if (hCtrl == GetDlgItem(hWnd, IDC_DOMAINS_LIST))
        {
            SetBkColor(hdc, RGB(240, 240, 240));
            SetTextColor(hdc, RGB(20, 20, 20));
            return (LRESULT)hGrayBrush;
        }

        // White background for other static controls
        SetBkColor(hdc, RGB(255, 255, 255));
        SetTextColor(hdc, RGB(0, 0, 0));
        return (LRESULT)GetStockObject(WHITE_BRUSH);
    }

    case WM_DESTROY:
    {
        if (hTitleFont) DeleteObject(hTitleFont);
        if (hStatusFont) DeleteObject(hStatusFont);
        if (hSmallFont) DeleteObject(hSmallFont);
        if (hLargeFont) DeleteObject(hLargeFont);
        if (hGrayBrush) DeleteObject(hGrayBrush);
        PostQuitMessage(0);
        return 0;
    }

    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }

    return 0;
}