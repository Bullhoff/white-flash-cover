// WhiteFlashCover.cpp : Defines the entry point for the application.
//
//#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='*' publicKeyToken='6595b64144ccf1df' language='*'\"")
//#pragma comment(lib, "comctl32.lib")
//#include <commctrl.h>

#pragma comment(lib, "comctl32.lib")
#include "framework.h"
#include "WhiteFlashCover.h"
#include "console.h"
#include "devices.h"
#include "parseconfig.h"
#include "elements.h"
#include "utils.h"
#define MAX_LOADSTRING 100

#define TEXTOUTPUT 1
#define INPUT_TIME_OPEN 27
#define INPUT_ACTIVE 28
#define INPUT_ALPHA 4
#define ID_BUTTON 1
#define ID_CLOSE 5
#define ID_EXIT 6
#define ID_ONLY_NOTIFICATION_AREA 7
#define ID_MODE_SHOWHIDE  8
#define ID_MODE_ALPHA  9
#define ID_MODE_CLOAK 10
#define ID_Test1 11
#define ID_Test2 12
#define ID_Test3 13
#define ID_Test4 14
#define ID_Test5 15
#define ID_FULLCOVER 16

#define INPUT_FULLCOVER_ACTIVE 18

#define ID_AUTOSAVE 19
#define ID_SAVE 20
#define ID_INFO_FLASHCOVER 22
#define ID_INFO_FULLCOVER 23
#define ID_START_MINIMIZED 24

#define INPUT_FLASH_COVER_ON_RESTORE 25
#define INPUT_FLASH_COVER_ON_MAXIMIZE 26


#define ID_MODE_SHADE_INACTIVE 48
#define ID_MODE_SHADE_FULL  49
#define ID_MODE_SHADE_INDIVIDUAL 50

#define ID_TRACKBAR 60
#define ID_TRACKBAR_MAX 70



//// Global Variables:
struct ConfigStruct Config;

HINSTANCE hInst;                                // current instance
WCHAR szTitle[MAX_LOADSTRING];                  // The title bar text
WCHAR szWindowClass[MAX_LOADSTRING];            // the main window class name

INITCOMMONCONTROLSEX icce;
HINSTANCE g_hinst;
MENUINFO mi;
HMENU hMenu;

HICON m_hIcon;
NOTIFYICONDATA m_NotifyData_ist;
UINT iNotificationArea;
WCHAR wNotificationArea;
#define NOTIFYCALLBACK 0x4DE

HWND hWindow;
HWND hSave, hAutosave, hOnlyNotificationArea, hStartMinimized;

std::map<std::string, HWINEVENTHOOK> hEventHooks;    // HWINEVENTHOOK

int windowWidth = 350;
int windowHeight = 520;  // 440+40


// FlashCover
HWND hCover;
HHOOK hHook;
BOOL flashCoverCloaked = false;
std::map<std::string, HWND> hwndFlashCover;
std::map<HWND, BOOL> hwndMaxed;

// FullCover
HWND hFullCover;
BOOL fullCoverCloaked = false;
std::map<std::string, HWND> hwndFullCover;
vector<MonitorStruct> monitors;


// Forward declarations of functions included in this code module:
ATOM                MyRegisterClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
LRESULT CALLBACK    WindowProcedure(HWND, UINT, WPARAM, LPARAM);
LRESULT CALLBACK    CoverProcedure(HWND, UINT, WPARAM, LPARAM);
LRESULT CALLBACK    NotifyHandler(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
INT_PTR CALLBACK    About(HWND, UINT, WPARAM, LPARAM);

void CALLBACK       WinEventProcCallback(HWINEVENTHOOK hWinEventHook, DWORD dwEvent, HWND hwnd, LONG idObject, LONG idChild, DWORD dwEventThread, DWORD dwmsEventTime);

BOOL                InitNotificationIcon(HWND hwnd);
int                 InitCover();
int                 initGeneralControls(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
int                 initFlashCover(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
int                 initFullCover(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
int                 adjustFullCover(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
int                 updateInput(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
int                 applyMode();


int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
                     _In_opt_ HINSTANCE hPrevInstance,
                     _In_ LPWSTR    lpCmdLine,
                     _In_ int       nCmdShow)
{

    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);
    //SetProcessDPIAware();
    SetProcessDpiAwarenessContext(DPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE_V2);   // DPI_AWARENESS_CONTEXT_UNAWARE, DPI_AWARENESS_CONTEXT_SYSTEM_AWARE, DPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE, DPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE_V2, DPI_AWARENESS_CONTEXT_UNAWARE_GDISCALED
    WNDCLASSW wcex = { 0 };
    //wcex.style = STATIC;    // https://learn.microsoft.com/en-us/windows/win32/winmsg/window-class-styles
    wcex.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
    wcex.lpszClassName = L"CoverClass";
    wcex.lpfnWndProc = CoverProcedure;
    RegisterClassW(&wcex);
    InitCover();
    LoadConfig(Config);
    if(Config.console) ActivateConsole();
    
    hEventHooks["EVENT_SYSTEM_MINIMIZEEND"] = SetWinEventHook(EVENT_SYSTEM_MINIMIZEEND, EVENT_SYSTEM_MINIMIZEEND, NULL, WinEventProcCallback, 0, 0, WINEVENT_OUTOFCONTEXT);  // | WINEVENT_SKIPOWNPROCESS
    hEventHooks["EVENT_OBJECT_LOCATIONCHANGE"] = SetWinEventHook(EVENT_OBJECT_LOCATIONCHANGE, EVENT_OBJECT_LOCATIONCHANGE, NULL, WinEventProcCallback, 0, 0, WINEVENT_OUTOFCONTEXT);  // | WINEVENT_SKIPOWNPROCESS
    
    LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
    LoadStringW(hInstance, IDC_WHITEFLASHCOVER, szWindowClass, MAX_LOADSTRING);
    MyRegisterClass(hInstance);

    if (!InitInstance (hInstance, nCmdShow)) return FALSE;


    HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_WHITEFLASHCOVER));
    MSG msg;
    
    BOOL bRet;
    while (bRet = GetMessage(&msg, nullptr, 0, 0) != 0)
    {
        if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg)){
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
    }
    Shell_NotifyIconW(NIM_DELETE, &m_NotifyData_ist);
    UnhookWinEvent(hEventHooks["EVENT_SYSTEM_MINIMIZEEND"]);
    UnhookWinEvent(hEventHooks["EVENT_OBJECT_LOCATIONCHANGE"]);
    
    return (int) msg.wParam;
}


ATOM MyRegisterClass(HINSTANCE hInstance)
{
    WNDCLASSEXW wcex;
    wcex.cbSize = sizeof(WNDCLASSEX);
    wcex.style          = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc    = WndProc;
    wcex.cbClsExtra     = 0;
    wcex.cbWndExtra     = 0;
    wcex.hInstance      = hInstance;
    wcex.hIcon          = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_WHITEFLASHCOVER));
    wcex.hCursor        = LoadCursor(nullptr, IDC_ARROW);
    wcex.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);   // (HBRUSH)(COLOR_WINDOW + 1);
    wcex.lpszMenuName   = MAKEINTRESOURCEW(IDC_WHITEFLASHCOVER);
    wcex.lpszClassName  = szWindowClass;
    wcex.hIconSm        = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));
    return RegisterClassExW(&wcex);
}




int InitCover() {
    hCover = CreateWindowExW(WS_EX_TOPMOST , L"CoverClass", L"",  WS_VISIBLE, 400, 400, 500, 500, NULL, NULL, NULL, NULL);	// | WS_VISIBLE
    SetWindowLongPtr(hCover, GWL_STYLE, GetWindowLong(hCover, GWL_STYLE) & ~(WS_CAPTION | WS_THICKFRAME | WS_MINIMIZEBOX | WS_MAXIMIZEBOX | WS_SYSMENU));
    SetWindowLongPtr(hCover, GWL_EXSTYLE,   WS_EX_TOOLWINDOW | WS_EX_LAYERED | WS_EX_TRANSPARENT);
    SetLayeredWindowAttributes(hCover, 0, BYTE((255 * Config.flashCoverAlphaPct) / 100), LWA_ALPHA);
    if (Config.flashCoverMode == "showhide") ShowWindow(hCover, SW_HIDE);
    if (Config.flashCoverMode == "alpha") SetLayeredWindowAttributes(hCover, 0, BYTE(0 * (255 * Config.flashCoverAlphaPct) / 100), LWA_ALPHA);
    if (Config.flashCoverMode == "cloak") {
        flashCoverCloaked = true;
        DwmSetWindowAttribute(hCover, DWMWA_CLOAK, &flashCoverCloaked, sizeof(flashCoverCloaked));
    }
    return 0;
}




BOOL InitNotificationIcon(HWND hwnd) {
    m_NotifyData_ist.cbSize = sizeof(m_NotifyData_ist);
    m_NotifyData_ist.hWnd = hwnd;
    m_NotifyData_ist.uFlags = NIF_ICON | NIF_TIP | NIF_MESSAGE | NIF_SHOWTIP | NIF_GUID;
    m_NotifyData_ist.uID = 101;
    m_NotifyData_ist.uCallbackMessage = NOTIFYCALLBACK;
    m_NotifyData_ist.hIcon = static_cast<HICON>(::LoadImage(hInst, MAKEINTRESOURCE(IDI_SMALL), IMAGE_ICON, 48, 48, LR_DEFAULTCOLOR));
    //LoadIconMetric(hInst, MAKEINTRESOURCE(IDI_SMALL), LIM_SMALL, &m_NotifyData_ist.hIcon);
    LoadString(hInst, IDS_TOOLTIP, m_NotifyData_ist.szTip, ARRAYSIZE(m_NotifyData_ist.szTip));
    m_NotifyData_ist.uVersion = NOTIFYICON_VERSION_4;
    Shell_NotifyIconW(NIM_ADD, &m_NotifyData_ist);
    return Shell_NotifyIcon(NIM_SETVERSION, &m_NotifyData_ist);
}

int InitMenuBar(HWND hWnd) {
    SetMenu(hWnd, NULL);
    // DestroyMenu(::GetMenu(hWnd));
    return 0;
}

BOOL InitInstance(HINSTANCE hInstance, int nCmdShow){
   hInst = hInstance; 
   hWindow = CreateWindowW(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT, windowWidth, windowHeight, nullptr, nullptr, hInstance, nullptr);
   if (!hWindow) return FALSE;
   BOOL USE_DARK_MODE = true;
   BOOL SET_IMMERSIVE_DARK_MODE_SUCCESS = SUCCEEDED(DwmSetWindowAttribute(hWindow, DWMWINDOWATTRIBUTE::DWMWA_USE_IMMERSIVE_DARK_MODE, &USE_DARK_MODE, sizeof(USE_DARK_MODE)));
   
   ShowWindow(hWindow, nCmdShow);
   UpdateWindow(hWindow);

   if (Config.startMinimized) {
       SendMessage(hStartMinimized, BM_SETCHECK, BST_CHECKED, 1);
       if (Config.minimizeToNotificationArea) ShowWindow(hWindow, SW_HIDE);
       else ShowWindow(hWindow, SW_MINIMIZE);
   }
   return TRUE;
}

int ActivateFlashCover(HWND hwnd) {
    if (!Config.flashCoverActive) return 0;
    if (hCover == NULL) InitCover();
    if (hCover == NULL) return 1;
    RECT rect;
    if (GetWindowRect(hwnd, &rect)) {
        if (Config.flashCoverMode == "showhide") SetWindowPos(hCover, NULL, rect.left, rect.top, rect.right - rect.left, rect.bottom - rect.top, SWP_NOACTIVATE | SWP_SHOWWINDOW);
        else if (Config.flashCoverMode == "alpha") {
            SetWindowPos(hCover, NULL, rect.left, rect.top, rect.right - rect.left, rect.bottom - rect.top, SWP_NOACTIVATE);
            SetLayeredWindowAttributes(hCover, 0, BYTE(1 * (255 * Config.flashCoverAlphaPct) / 100), LWA_ALPHA);
        }
        else if (Config.flashCoverMode == "cloak") {
            SetWindowPos(hCover, NULL, rect.left, rect.top, rect.right - rect.left, rect.bottom - rect.top, SWP_NOACTIVATE);
            flashCoverCloaked = false;
            DwmSetWindowAttribute(hCover, DWMWA_CLOAK, &flashCoverCloaked, sizeof(flashCoverCloaked));
        }
        Sleep(Config.flashCoverTimeMs);
        if (Config.flashCoverMode == "showhide") ShowWindow(hCover, SW_HIDE);
        else if (Config.flashCoverMode == "alpha") SetLayeredWindowAttributes(hCover, 0, BYTE(0 * (255 * Config.flashCoverAlphaPct) / 100), LWA_ALPHA);
        else if (Config.flashCoverMode == "cloak") {
            flashCoverCloaked = true;
            DwmSetWindowAttribute(hCover, DWMWA_CLOAK, &flashCoverCloaked, sizeof(flashCoverCloaked));
        }
    }

    return 0;
}

void CALLBACK WinEventProcCallback(HWINEVENTHOOK hWinEventHook, DWORD dwEvent, HWND hwnd, LONG idObject, LONG idChild, DWORD dwEventThread, DWORD dwmsEventTime) {
    if (!Config.flashCoverActive) return; 
    BOOL visible = IsWindowVisible(hwnd);
    BOOL window = IsWindow(hwnd);
    LONG style = GetWindowLong(hwnd, GWL_STYLE);
    BOOL isTopLevel = !(style & WS_CHILD);

    if (dwEvent == EVENT_OBJECT_LOCATIONCHANGE && isTopLevel && window && Config.flashCoverOnMaximize ) {
        HWND fg = GetForegroundWindow();
        if (fg == hwnd ) {
            WINDOWPLACEMENT lpwndpl;
            lpwndpl.length = sizeof(WINDOWPLACEMENT);
            GetWindowPlacement(hwnd, &lpwndpl);
            BOOL maxed = lpwndpl.showCmd == SW_SHOWMAXIMIZED;
            if (maxed && hwndMaxed.count(hwnd) > 0 && hwndMaxed[hwnd] == FALSE) {
                std::thread th(ActivateFlashCover, hwnd);                th.detach();
            }
            hwndMaxed[hwnd] = maxed;
        }
    }
    else if (dwEvent == EVENT_SYSTEM_MINIMIZEEND && visible && window && Config.flashCoverOnRestore) {
        std::thread th(ActivateFlashCover, hwnd);        th.detach();
    }
}

LRESULT CALLBACK CoverProcedure(HWND hWnd, UINT msg, WPARAM wp, LPARAM lp){
    switch (msg){
        //case WM_GETMINMAXINFO: break;
        //case HC_ACTION: break;
        //case WM_SHOWWINDOW: break;
        //case WM_MOVE: break;
        //case WM_WINDOWPOSCHANGED: break;
        default: return DefWindowProcW(hWnd, msg, wp, lp);
    }
    return 0;
}

LRESULT CALLBACK NotifyHandler(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) {
    switch (LOWORD(lParam)){
        case NIN_SELECT: {   // if (lParam == 514)
            if(Config.console) cout << " NIN_SELECT:" << NIN_SELECT << endl;
            //BOOL visible = IsWindowVisible(hWnd);
            //HWND ForegroundWindow = GetForegroundWindow();
            //HWND ActiveWindow = GetActiveWindow();
            BOOL iconic = IsIconic(hWnd);
            auto style = GetWindowLong(hWindow, GWL_STYLE);
            BOOL isShow = (style & WS_VISIBLE);
            if (isShow && Config.minimizeToNotificationArea) ShowWindow(hWindow, SW_HIDE);
            else if (!isShow && Config.minimizeToNotificationArea) ShowWindow(hWindow, SW_SHOW);
            else if (!iconic && !Config.minimizeToNotificationArea) ShowWindow(hWindow, SW_MINIMIZE);
            else ShowWindow(hWindow, SW_RESTORE);
            SetForegroundWindow(hWindow);
            break;
        }
        case WM_CONTEXTMENU : { // if (lParam == 517)
            if (Config.console) cout << " WM_CONTEXTMENU:" << WM_CONTEXTMENU << endl;
            POINT pt;
            GetCursorPos(&pt);
            HMENU hPopupMenu = CreatePopupMenu();
            InsertMenuW(hPopupMenu, 0, MF_BYPOSITION | MF_STRING, IDM_EXIT, L"Exit");
            TrackPopupMenu(hPopupMenu, TPM_BOTTOMALIGN | TPM_LEFTALIGN, pt.x, pt.y, 0, hWindow, NULL);
            break;
        }
        default:
            return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return DefWindowProc(hWnd, message, wParam, lParam);
}
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam){
    switch (message){
        case WM_CONTEXTMENU:
            return DefWindowProc(hWnd, message, wParam, lParam);
            break;
        case NOTIFYCALLBACK: {
            NotifyHandler(hWnd, message, wParam, lParam);
            break;
        }
        case WM_CREATE:
            //icce = { sizeof(INITCOMMONCONTROLSEX), ICC_BAR_CLASSES };
            //icce.dwSize = sizeof(INITCOMMONCONTROLSEX);
            //icce.dwICC = ICC_STANDARD_CLASSES | ICC_TAB_CLASSES | ICC_BAR_CLASSES | ICC_WIN95_CLASSES;
            //InitCommonControlsEx(&icce);
            InitNotificationIcon(hWnd);
            InitMenuBar(hWnd);
            initGeneralControls( hWnd,  message,  wParam,  lParam);
            initFlashCover(hWnd, message, wParam, lParam);
            initFullCover(hWnd, message, wParam, lParam);
            break;
        case WM_COMMAND:{
            int wmId = LOWORD(wParam);
            switch (wmId){
                case IDM_ABOUT:
                    DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
                    break;
                case IDM_EXIT:
                    Shell_NotifyIconW(NIM_DELETE, &m_NotifyData_ist);
                    DestroyWindow(hWnd);
                    break;
                case ID_SAVE: 
                    SaveConfig(Config);
                    break;
                case ID_AUTOSAVE:
                    Config.autosave = !Config.autosave;
                    if (Config.autosave) SaveConfig(Config);
                    break;
                case ID_START_MINIMIZED: 
                    Config.startMinimized = !Config.startMinimized;
                    if (Config.autosave) SaveConfig(Config);
                    break;
                case ID_ONLY_NOTIFICATION_AREA:
                    Config.minimizeToNotificationArea = !Config.minimizeToNotificationArea;
                    if (Config.autosave) SaveConfig(Config);
                    break;
                case INPUT_ACTIVE:
                    Config.flashCoverActive = !Config.flashCoverActive;
                    if (Config.autosave) SaveConfig(Config);
                    break;
                case INPUT_FLASH_COVER_ON_RESTORE:
                    Config.flashCoverOnRestore = !Config.flashCoverOnRestore;
                    if (Config.autosave) SaveConfig(Config);
                    break;
                case INPUT_FLASH_COVER_ON_MAXIMIZE:
                    Config.flashCoverOnMaximize = !Config.flashCoverOnMaximize;
                    if (Config.autosave) SaveConfig(Config);
                    break;
                case INPUT_FULLCOVER_ACTIVE: 
                    Config.fullCoverActive = !Config.fullCoverActive;
                    if (Config.fullCoverActive) adjustFullCover( hWnd,  message,  wParam,  lParam);
                    if (Config.autosave) SaveConfig(Config);
                    break;
                case ID_MODE_SHADE_FULL: 
                    Config.fullCoverMode = "per_all";
                    EnableWindow(hwndFullCover["per_all_trackbar"], true);
                    for (size_t i = 0; i < monitors.size(); i++) EnableWindow(monitors[i].hTrackbar, false);
                    if (Config.autosave) SaveConfig(Config);
                    break;
                case ID_MODE_SHADE_INDIVIDUAL:
                    Config.fullCoverMode = "per_monitor";
                    EnableWindow(hwndFullCover["per_all_trackbar"], false);
                    for (size_t i = 0; i < monitors.size(); i++) EnableWindow(monitors[i].hTrackbar, true);
                    if (Config.autosave) SaveConfig(Config);
                    break;
                case ID_MODE_CLOAK:
                case ID_MODE_SHOWHIDE:
                case ID_MODE_ALPHA:
                case INPUT_ALPHA:
                case INPUT_TIME_OPEN:
                    updateInput(hWnd, message, wParam, lParam);
                    applyMode();
                    if (Config.autosave) SaveConfig(Config);
                    break;
                default:
                    return DefWindowProc(hWnd, message, wParam, lParam);
            }
            break;
        }
        case WM_HSCROLL:
            adjustFullCover(hWnd, message, wParam, lParam);
            if (Config.autosave) SaveConfig(Config);
            break;
        case WM_PAINT:{
                PAINTSTRUCT ps;
                HDC hdc = BeginPaint(hWnd, &ps);
                EndPaint(hWnd, &ps);
                return DefWindowProc(hWnd, message, wParam, lParam);
        }
        //case WM_DRAWITEM:
        //case WM_CTLCOLORBTN:
        case WM_CTLCOLOREDIT:
        case WM_CTLCOLORSTATIC:{
            HWND hctrl = reinterpret_cast<HWND>(lParam);
            int id = GetDlgCtrlID(hctrl);
            if(id >= ID_TRACKBAR && id <= ID_TRACKBAR_MAX) 
                return (INT_PTR)(HBRUSH)GetStockObject(BLACK_BRUSH);  // Paints over the trackbar thumb skid marks 
            
            HDC hdcStatic = (HDC)wParam;
            if (id == INPUT_TIME_OPEN || id == INPUT_ALPHA) {
                SetBkColor(hdcStatic, RGB(150, 150, 150));
                SetDCBrushColor(hdcStatic, RGB(150, 150, 150));
                return (INT_PTR)(HBRUSH)GetStockObject(DC_BRUSH);
            }
            SetBkColor(hdcStatic, TRANSPARENT);
            SetTextColor(hdcStatic, 0x00FFFFFF);
            return (INT_PTR)(HBRUSH)GetStockObject(NULL_BRUSH);     // GetStockObject(DC_BRUSH) DKGRAY_BRUSH BLACK_BRUSH
        }
        case WM_SYSCOMMAND:{
            int command = wParam & 0xfff0;
            if (command == SC_MINIMIZE) {
                if (Config.minimizeToNotificationArea) {
                    ShowWindow(hWnd, SW_HIDE);
                    break;
                }
            }
            //if (command == SC_RESTORE) {}
            //if (command == SC_MAXIMIZE) {}
            //if (command == SC_CLOSE) {}
            //if (command == SC_MAXIMIZE) {}
            //if (command == SC_RESTORE) {}
            //if (command == SC_CLOSE) {}
            return DefWindowProc(hWnd, message, wParam, lParam);
        }
        //case WM_PRINTCLIENT: return DefWindowProc(hWnd, message, wParam, lParam);
        //case WM_ACTIVATEAPP: return DefWindowProc(hWnd, message, wParam, lParam);
        //case WM_CHILDACTIVATE: return DefWindowProc(hWnd, message, wParam, lParam);
        //case WM_SHOWWINDOW: return DefWindowProc(hWnd, message, wParam, lParam);
        //case WM_MOVE: return DefWindowProc(hWnd, message, wParam, lParam);
        //case WM_WINDOWPOSCHANGED: return DefWindowProc(hWnd, message, wParam, lParam);
        case WM_DESTROY:
            Shell_NotifyIcon(NIM_DELETE, &m_NotifyData_ist);
            PostQuitMessage(0);
            break;
        default:
            return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
}

INT_PTR CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam) {
    UNREFERENCED_PARAMETER(lParam);
    switch (message){
        case WM_INITDIALOG:
            SetDlgItemText(hDlg, IDGHLINK, L"https://github.com/Bullhoff/WhiteFlashCover");
            return (INT_PTR)TRUE;

        case WM_COMMAND:
            if (LOWORD(wParam) == IDCOPY) {}
            if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL){
                EndDialog(hDlg, LOWORD(wParam));
                return (INT_PTR)TRUE;
            }
            break;
    }
    return (INT_PTR)FALSE;
}
int initGeneralControls(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) {
    
    CreateWindowExW(0, L"BUTTON", L"?", WS_CHILD | WS_VISIBLE , 270, 10, 30, 16, hWnd, (HMENU)IDM_ABOUT, GetModuleHandle(NULL), NULL);  // | BS_OWNERDRAW

    hSave = CreateWindowExW(0, L"BUTTON", L"Save", WS_CHILD | WS_VISIBLE | BS_PUSHLIKE, 10, 10, 100, 16, hWnd, (HMENU)ID_SAVE, GetModuleHandle(NULL), NULL);
    CreateToolTip(ID_SAVE, hWnd, L"Save to config file.");

    hAutosave = CreateWindowExW(0, L"BUTTON", L"Autosave", WS_CHILD | WS_VISIBLE | BS_AUTOCHECKBOX, 120, 10, 100, 16, hWnd, (HMENU)ID_AUTOSAVE, GetModuleHandle(NULL), NULL);
    CreateToolTip(ID_AUTOSAVE, hWnd, L"Autosave to config file on changes.");
    if (Config.autosave) SendMessage(hAutosave, BM_SETCHECK, BST_CHECKED, 1);

    hOnlyNotificationArea = CreateWindowExW(0, L"BUTTON", L"Minimize to notification area", WS_CHILD | WS_VISIBLE | BS_AUTOCHECKBOX, 10, 30, 250, 16, hWnd, (HMENU)ID_ONLY_NOTIFICATION_AREA, GetModuleHandle(NULL), NULL);
    CreateToolTip(ID_ONLY_NOTIFICATION_AREA, hWnd, L"Minimize to notification area. ");
    if (Config.minimizeToNotificationArea) SendMessage(hOnlyNotificationArea, BM_SETCHECK, BST_CHECKED, 1);

    hStartMinimized = CreateWindowExW(0, L"BUTTON", L"Start minimized", WS_CHILD | WS_VISIBLE | BS_AUTOCHECKBOX, 10, 50, 220, 16, hWnd, (HMENU)ID_START_MINIMIZED, GetModuleHandle(NULL), NULL);
    CreateToolTip(ID_START_MINIMIZED, hWnd, L"Start minimized. ");
    if (Config.startMinimized) SendMessage(hStartMinimized, BM_SETCHECK, BST_CHECKED, 1);
    
    return 0;
}
int initFlashCover(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) {
    if (Config.flashCoverActive) InitCover();

    //  Group controls visually
    CreateWindowEx(WS_EX_WINDOWEDGE, L"BUTTON", L"Flash cover:", WS_VISIBLE | WS_CHILD | BS_GROUPBOX,
        5, OFFSET_FLASHCOVER + 10 + FLASH_SPACING_Y * -1,        310, (OFFSET_FLASHCOVER + 10 + FLASH_SPACING_Y * 8) - (OFFSET_FLASHCOVER + 10 + FLASH_SPACING_Y * -1)+5,
        hWnd, NULL, hInst, NULL);

    //CreateWindowExW(0, L"BUTTON", L"?", WS_CHILD | WS_VISIBLE, 210, OFFSET_FLASHCOVER + 10 + 20 * 0, 16, 16, hWnd, (HMENU)ID_INFO_FLASHCOVER, GetModuleHandle(NULL), NULL);
    //CreateToolTip(ID_INFO_FLASHCOVER, hWnd, L"Puts a black window in foreground for a brief period of time when restoring and/or maximizing a window. ");

    hwndFlashCover["Active"] = CreateWindowExW(0, L"BUTTON", L"Active", WS_CHILD | WS_VISIBLE | BS_AUTOCHECKBOX, 10, OFFSET_FLASHCOVER + 10 + FLASH_SPACING_Y * 0, 200, 16, hWnd, (HMENU)INPUT_ACTIVE, GetModuleHandle(NULL), NULL);
    CreateToolTip(INPUT_ACTIVE, hWnd, L"Puts a black window in foreground for a brief period of time when restoring and/or maximizing a window. ");
    if (Config.flashCoverActive) SendMessage(hwndFlashCover["Active"], BM_SETCHECK, BST_CHECKED, 1);
    
    hwndFlashCover["flashCoverOnRestore"] = CreateWindowExW(0, L"BUTTON", L"On unminimizing", WS_CHILD | WS_VISIBLE | BS_AUTOCHECKBOX, 10, OFFSET_FLASHCOVER + 10 + FLASH_SPACING_Y * 1, 200, 16, hWnd, (HMENU)INPUT_FLASH_COVER_ON_RESTORE, GetModuleHandle(NULL), NULL);
    CreateToolTip(INPUT_FLASH_COVER_ON_RESTORE, hWnd, L"On unminimizing.");
    if (Config.flashCoverOnRestore) SendMessage(hwndFlashCover["flashCoverOnRestore"], BM_SETCHECK, BST_CHECKED, 1);
    
    hwndFlashCover["flashCoverOnMaximize"] = CreateWindowExW(0, L"BUTTON", L"On maximizing", WS_CHILD | WS_VISIBLE | BS_AUTOCHECKBOX, 10, OFFSET_FLASHCOVER + 10 + FLASH_SPACING_Y * 2, 200, 16, hWnd, (HMENU)INPUT_FLASH_COVER_ON_MAXIMIZE, GetModuleHandle(NULL), NULL);
    CreateToolTip(INPUT_FLASH_COVER_ON_MAXIMIZE, hWnd, L"On maximizing.");
    if (Config.flashCoverOnMaximize) SendMessage(hwndFlashCover["flashCoverOnMaximize"], BM_SETCHECK, BST_CHECKED, 1);
    

    CreateWindowW(L"STATIC", L"Time(ms)", WS_CHILD | WS_VISIBLE, 10, OFFSET_FLASHCOVER + 10 + FLASH_SPACING_Y * 3, 170, FLASH_SPACING_Y, hWnd, NULL, NULL, NULL);
    hwndFlashCover["vDuration"] = CreateWindowW(L"EDIT", L"100", WS_CHILD | WS_VISIBLE | ES_WANTRETURN | ES_NUMBER, 170, OFFSET_FLASHCOVER + 10 + FLASH_SPACING_Y * 3, 70, 16, hWnd, (HMENU)INPUT_TIME_OPEN, NULL, NULL);
    CreateToolTip(INPUT_TIME_OPEN, hWnd, L"Time for flash cover to stay open (ms)");
    SetDlgItemInt(hWnd, INPUT_TIME_OPEN, Config.flashCoverTimeMs, false);
    
    
    CreateWindowW(L"STATIC", L"Transparency(%)", WS_CHILD | WS_VISIBLE, 10, OFFSET_FLASHCOVER + 10 + FLASH_SPACING_Y * 4, 170, FLASH_SPACING_Y, hWnd, NULL, NULL, NULL);
    hwndFlashCover["vAlpha"] = CreateWindow(L"EDIT", L"70", WS_CHILD | WS_VISIBLE | ES_WANTRETURN | ES_NUMBER, 170, OFFSET_FLASHCOVER + 10 + FLASH_SPACING_Y * 4, 70, 16, hWnd, (HMENU)INPUT_ALPHA, NULL, NULL);
    CreateToolTip(INPUT_ALPHA, hWnd, L"Flash cover transparency (%)");
    SetDlgItemInt(hWnd, INPUT_ALPHA, Config.flashCoverAlphaPct, false);

    hwndFlashCover["ShowHide"] = CreateWindowEx(WS_EX_WINDOWEDGE,L"BUTTON", L"ShowHide",WS_VISIBLE | WS_CHILD | BS_AUTORADIOBUTTON | WS_GROUP,   
        10, OFFSET_FLASHCOVER + 10 + FLASH_SPACING_Y * 5,        200, 20,
        hWnd, (HMENU)ID_MODE_SHOWHIDE, NULL, NULL);
    CreateToolTip(ID_MODE_SHOWHIDE, hWnd, L"Apply flash cover by show/hide");

    hwndFlashCover["Alpha"] = CreateWindowEx(WS_EX_WINDOWEDGE, L"BUTTON", L"Alpha", WS_VISIBLE | WS_CHILD | BS_AUTORADIOBUTTON,
        10, OFFSET_FLASHCOVER + 10 + FLASH_SPACING_Y * 6,        200, 20,
        hWnd, (HMENU)ID_MODE_ALPHA, NULL, NULL);
    CreateToolTip(ID_MODE_ALPHA, hWnd, L"Apply flash cover by changing transparency from 0% to the transparency value");

    hwndFlashCover["Cloak"] = CreateWindowEx(WS_EX_WINDOWEDGE, L"BUTTON", L"Cloak", WS_VISIBLE | WS_CHILD | BS_AUTORADIOBUTTON,
        10, OFFSET_FLASHCOVER + 10 + FLASH_SPACING_Y * 7,        200, 20,
        hWnd, (HMENU)ID_MODE_CLOAK, NULL, NULL);
    CreateToolTip(ID_MODE_CLOAK, hWnd, L"Apply flash cover by cloak/uncloak");

    
    if (Config.flashCoverMode == "showhide") SendMessage(hwndFlashCover["ShowHide"], BM_SETCHECK, BST_CHECKED, 1);
    else if (Config.flashCoverMode == "alpha") SendMessage(hwndFlashCover["Alpha"], BM_SETCHECK, BST_CHECKED, 1);
    else if (Config.flashCoverMode == "cloak") SendMessage(hwndFlashCover["Cloak"], BM_SETCHECK, BST_CHECKED, 1);

    return 0;
}
int initFullCover(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) {
    // Get monitors
    monitors = getMonitors();

    //  Group controls visually
    CreateWindowEx(WS_EX_WINDOWEDGE,
        L"BUTTON", L"Full cover:",
        WS_VISIBLE | WS_CHILD | BS_GROUPBOX,
        5, TB_OFFSET - 20 * 2,
        310, (TB_OFFSET + TB_SPACING_Y * ((int)monitors.size() + 3)) - (TB_OFFSET - 20 * 2) + 10,
        hWnd, NULL, hInst, NULL);

    //CreateWindowExW(0, L"BUTTON", L"?", WS_CHILD | WS_VISIBLE, 210, TB_OFFSET - 20 * 1, 16, 16, hWnd, (HMENU)ID_INFO_FULLCOVER, GetModuleHandle(NULL), NULL);
    //CreateToolTip(ID_INFO_FULLCOVER, hWnd, L"Adds a transparent layer to each monitor. \nThe sliders below control the transparency. The top slider controls all monitors and the rest controls the monitors individually. ");
    
    hwndFullCover["Active"]  = CreateWindowExW(0, L"BUTTON", L"Active", WS_CHILD | WS_VISIBLE | BS_AUTOCHECKBOX, 10, TB_OFFSET - 20 * 1, 200, 16, hWnd, (HMENU)INPUT_FULLCOVER_ACTIVE, GetModuleHandle(NULL), NULL);
    CreateToolTip(INPUT_FULLCOVER_ACTIVE, hWnd, L"Adds a transparent layer to each monitor. \nThe sliders below control the transparency. The top slider controls all monitors and the rest controls the monitors individually. ");
    if (Config.fullCoverActive) SendMessage(hwndFullCover["Active"], BM_SETCHECK, BST_CHECKED, 1);

    // Select slider mode
    hwndFullCover["per_all"] = CreateWindowEx(WS_EX_WINDOWEDGE,  L"BUTTON", L"Shade all",WS_VISIBLE | WS_CHILD | BS_AUTORADIOBUTTON | WS_GROUP,
        10, TB_OFFSET + 20 * 0,        200, 20,
        hWnd, (HMENU)ID_MODE_SHADE_FULL, NULL, NULL);
    CreateToolTip(ID_MODE_SHADE_FULL, hWnd, L"Use the top slider to adjust the cover transparency(%) on all monitors");

    hwndFullCover["per_monitor"] = CreateWindowEx(WS_EX_WINDOWEDGE, L"BUTTON", L"Shade individual", WS_VISIBLE | WS_CHILD | BS_AUTORADIOBUTTON,
        10, TB_OFFSET + 20 * 1,        200, 20,
        hWnd, (HMENU)ID_MODE_SHADE_INDIVIDUAL, NULL, NULL);
    CreateToolTip(ID_MODE_SHADE_INDIVIDUAL, hWnd, L"Use the sliders to adjust the cover transparency(%) individually for the monitors");
    
    // setup per_all controls
    hwndFullCover["per_all_trackbar"] = CreateTrackbar(hWnd, ID_TRACKBAR, 0, 100, 0, 100, 4, 10, int(TB_OFFSET + TB_SPACING_Y * 1.5), 200, 30, Config.fullCoverAlphaPct);
    CreateToolTip(ID_TRACKBAR, hWnd, L"All monitors transparency(%)");

    
    // setup per_monitor controls
    for (int i = 0; i < monitors.size(); i++) {
        if (i > 0) {
            RECT rect;
            GetWindowRect(hWnd, &rect);
            SetWindowPos(hWnd, NULL, rect.left, rect.top, windowWidth, windowHeight + i * TB_SPACING_Y, SWP_SHOWWINDOW);
        }

        if (Config.fullCoverAlphaPct_i.size() > i) monitors[i].pctAlpha = Config.fullCoverAlphaPct_i[i];
        else Config.fullCoverAlphaPct_i.push_back(0);

        monitors[i].hTrackbar = CreateTrackbar(hWnd, ID_TRACKBAR + i + 1, 0, 100, 0, 100, 4, 10, TB_OFFSET + TB_SPACING_Y * (i + 3), 200, 30, Config.fullCoverAlphaPct_i[i]);
        CreateToolTip(ID_TRACKBAR + i + 1, hWnd, L"transparency(%)");

        
        //  int "x" int --> "int x int"
        const long w = monitors[i].info.rcMonitor.right - monitors[i].info.rcMonitor.left;
        const long h = monitors[i].info.rcMonitor.bottom - monitors[i].info.rcMonitor.top;
        std::ostringstream oss;
        oss << w << " x " << h;
        std::string monitortext = oss.str();
        LPCSTR  orig = monitortext.c_str();
        size_t origsize = strlen(orig) + 1;
        const size_t newsize = 100;
        size_t convertedChars = 0;
        wchar_t wcstring[newsize];
        mbstowcs_s(&convertedChars, wcstring, origsize, orig, _TRUNCATE);
        int y = TB_OFFSET + TB_SPACING_Y * (i + 3) + TB_SPACING_Y / 4;
        CreateWindow(L"STATIC", wcstring, WS_CHILD | WS_VISIBLE, 216, y, 185, TB_SPACING_Y / 2, hWnd, NULL, NULL, NULL);
        if (monitors[i].info.dwFlags == MONITORINFOF_PRIMARY) 
            CreateWindowEx(WS_EX_TRANSPARENT, L"STATIC", L"*", WS_CHILD | WS_VISIBLE | SS_CENTER, 200, y, 16, TB_SPACING_Y / 2, hWnd, NULL, NULL, NULL);
        
    }
    if (Config.fullCoverMode == "per_all") {
        SendMessage(hwndFullCover["per_all"], BM_SETCHECK, BST_CHECKED, 1);
        for (int i = 0; i < monitors.size(); i++) EnableWindow(monitors[i].hTrackbar, false);
    }
    else if (Config.fullCoverMode == "per_monitor") {
        SendMessage(hwndFullCover["per_monitor"], BM_SETCHECK, BST_CHECKED, 1);
        EnableWindow(hwndFullCover["per_all_trackbar"], false);
    }
    if(Config.fullCoverActive) adjustFullCover(hWnd, message, wParam, lParam);
    return 0;
}

int adjustFullCover(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) {
    for (auto it = monitors.begin(); it != monitors.end(); ++it) {
        auto& item = *it;
        fullCoverCloaked = !Config.fullCoverActive;
        DwmSetWindowAttribute(item.hwnd, DWMWA_CLOAK, &fullCoverCloaked, sizeof(fullCoverCloaked));
        //SetLayeredWindowAttributes(item.hwnd, 0, BYTE(0*(255 * item.pctAlpha) / 100), LWA_ALPHA);
    }
    int count = 0;
    if (Config.fullCoverMode == "per_monitor")
        for (int i = 0; i < monitors.size(); i++) {
            int checkVal = (int)SendMessage(monitors[i].hTrackbar, TBM_GETPOS, 0, 0);
            if (checkVal != monitors[i].pctAlpha) {
                monitors[i].pctAlpha = (int)checkVal;
                //Config.fullCoverAlphaPct_i[i] = (int)checkVal;
                break;
            }
        }
    else if (Config.fullCoverMode == "per_all") {
        int checkVal = (int)SendMessage(hwndFullCover["per_all_trackbar"], TBM_GETPOS, 0, 0);
        Config.fullCoverAlphaPct = checkVal;
        for (auto it = monitors.begin(); it != monitors.end(); ++it) {
            auto& item = *it;
            item.pctAlpha = (int)checkVal;
            SendMessage(item.hTrackbar, TBM_SETPOS, TRUE, checkVal);
        }
    }

    count = 0;
    for (auto it = monitors.begin(); it != monitors.end(); ++it) {
        auto& item = *it;
        if (item.hwnd == NULL) {
            int add = 0;
            item.hwnd = CreateWindowExW(WS_EX_TOPMOST | WS_EX_TOOLWINDOW | WS_EX_LAYERED | WS_EX_TRANSPARENT, L"CoverClass", L"", WS_DLGFRAME | WS_VISIBLE, item.info.rcMonitor.left - add, item.info.rcMonitor.top - add, std::abs(item.info.rcMonitor.left - item.info.rcMonitor.right) + add * 2, std::abs(item.info.rcMonitor.top - item.info.rcMonitor.bottom) + add * 2, NULL, NULL, NULL, NULL);	// | WS_VISIBLE
            if (item.hwnd == NULL) continue;
            SetWindowLongPtr(item.hwnd, GWL_EXSTYLE, GetWindowLong(item.hwnd, GWL_EXSTYLE) | WS_EX_TOOLWINDOW | WS_EX_LAYERED | WS_EX_TRANSPARENT);
            SetLayeredWindowAttributes(item.hwnd, 0, BYTE((255 * item.pctAlpha) / 100), LWA_ALPHA);
            SetWindowLongPtr(item.hwnd, GWL_STYLE, GetWindowLong(item.hwnd, GWL_STYLE) & ~(WS_CAPTION | WS_THICKFRAME | WS_MINIMIZEBOX | WS_MAXIMIZEBOX | WS_SYSMENU));
            if (count > Config.fullCoverAlphaPct_i.size()) Config.fullCoverAlphaPct_i.push_back(item.pctAlpha);
        }
        Config.fullCoverAlphaPct_i[count] = item.pctAlpha;
        SetLayeredWindowAttributes(item.hwnd, 0, (255 * item.pctAlpha) / 100, LWA_ALPHA);
        count++;
    }
    return 0;
}


int applyMode() {
    if (!Config.flashCoverActive) return 0;
    if (hCover == NULL) InitCover();
    if (hCover == NULL) return 1;
    if (Config.flashCoverMode == "showhide") {
        SetLayeredWindowAttributes(hCover, 0, BYTE(1 * (255 * Config.flashCoverAlphaPct) / 100), LWA_ALPHA);
        ShowWindow(hCover, SW_HIDE);
        flashCoverCloaked = false;
        DwmSetWindowAttribute(hCover, DWMWA_CLOAK, &flashCoverCloaked, sizeof(flashCoverCloaked));
    }
    else if (Config.flashCoverMode == "alpha") {
        SetLayeredWindowAttributes(hCover, 0, BYTE(0 * (255 * Config.flashCoverAlphaPct) / 100), LWA_ALPHA);
        SetWindowPos(hCover, NULL, 0, 0, 0, 0, SWP_NOACTIVATE | SWP_SHOWWINDOW);
        flashCoverCloaked = false;
        DwmSetWindowAttribute(hCover, DWMWA_CLOAK, &flashCoverCloaked, sizeof(flashCoverCloaked));
    }
    else if (Config.flashCoverMode == "cloak") {
        SetLayeredWindowAttributes(hCover, 0, BYTE(1 * (255 * Config.flashCoverAlphaPct) / 100), LWA_ALPHA);
        SetWindowPos(hCover, NULL, 0, 0, 0, 0, SWP_NOACTIVATE | SWP_SHOWWINDOW);
        flashCoverCloaked = true;
        DwmSetWindowAttribute(hCover, DWMWA_CLOAK, &flashCoverCloaked, sizeof(flashCoverCloaked));
    }
    return 0;
}
int updateInput(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) {
    int wmId = LOWORD(wParam);
    if (wmId == ID_MODE_SHOWHIDE) Config.flashCoverMode = "showhide";
    else if (wmId == ID_MODE_ALPHA) Config.flashCoverMode = "alpha";
    else if (wmId == ID_MODE_CLOAK) Config.flashCoverMode = "cloak";
    else if (wmId == INPUT_ACTIVE) Config.flashCoverActive = !Config.flashCoverActive;
    else if (wmId == INPUT_FLASH_COVER_ON_RESTORE) Config.flashCoverOnRestore = !Config.flashCoverOnRestore;
    else if (wmId == INPUT_FLASH_COVER_ON_MAXIMIZE) Config.flashCoverOnMaximize = !Config.flashCoverOnMaximize;
    else if (wmId == INPUT_FULLCOVER_ACTIVE) Config.fullCoverActive = !Config.fullCoverActive;
    else if (wmId == INPUT_TIME_OPEN) {
        int len = GetWindowTextLength(hwndFlashCover["vDuration"]) + 1;
        if (len > 0) {
            TCHAR* buff = new TCHAR[len];
            if (GetWindowText(hwndFlashCover["vDuration"], buff, len)) Config.flashCoverTimeMs = std::stoi(buff);    // Config.flashCoverTimeMs
            delete[] buff;
        }
    }
    else if (wmId == INPUT_ALPHA) {
        int len = GetWindowTextLength(hwndFlashCover["vAlpha"]) + 1;
        if (len > 0 && LOWORD(wParam) == INPUT_ALPHA) {
            TCHAR* buff = new TCHAR[len];
            if (GetWindowText(hwndFlashCover["vAlpha"], buff, len))  Config.flashCoverAlphaPct = std::stoi(buff);
            delete[] buff;
        }
    }
    return 0;
}

