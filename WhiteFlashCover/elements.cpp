#include "framework.h"
#include "WhiteFlashCover.h"
#include "elements.h"

HWND CreateToolTip(int toolID, HWND hDlg, LPCWSTR pszText) {
    HWND hwndTool = GetDlgItem(hDlg, toolID);
    HWND hwndTip = CreateWindowEx(NULL, TOOLTIPS_CLASS, NULL,
        WS_POPUP | TTS_ALWAYSTIP | TTS_BALLOON,
        CW_USEDEFAULT, CW_USEDEFAULT,
        CW_USEDEFAULT, CW_USEDEFAULT,
        hDlg, NULL,
        NULL, NULL);

    if (hwndTip == NULL) return NULL;
    TOOLINFOW toolInfo;
    toolInfo.cbSize = TTTOOLINFO_V1_SIZE;//sizeof(toolInfo);//TTTOOLINFO_V1_SIZE; //sizeof(toolInfo);
    toolInfo.hwnd = GetParent(hDlg);
    toolInfo.uFlags = TTF_IDISHWND | TTF_SUBCLASS  | TTF_CENTERTIP | TTF_TRANSPARENT ;  // | TTF_TRACK
    toolInfo.uId = (UINT_PTR)hwndTool;
    toolInfo.lpszText = (LPWSTR)pszText;
    //GetClientRect(toolInfo.hwnd, &toolInfo.rect);
    if (!SendMessage(hwndTip, TTM_ACTIVATE, TRUE, 0)) std::cout << "err1.. " << to_string(GetLastError()).c_str() << std::endl;
    
    if (!SendMessage(hwndTip, TTM_ADDTOOL, 0, (LPARAM)&toolInfo)){
        std::cout << "err2.. " << to_string(GetLastError()).c_str() << std::endl;
        DestroyWindow(hwndTip);
        return NULL;
    }
    SendMessage(hwndTip, TTM_SETMAXTIPWIDTH, 0, 500);
    return hwndTip;
}


HWND WINAPI CreateTrackbar(
    HWND hwndParent,
    long long id,
    UINT iMin,
    UINT iMax,
    UINT iSelMin,
    UINT iSelMax,
    UINT iPageSize,
    UINT x,
    UINT y,
    UINT w,
    UINT h,
    UINT value
)
{
    HWND hwndTrack = CreateWindowEx( 0, TRACKBAR_CLASS, L"Trackbar Control",              
        WS_CHILD 
        | WS_VISIBLE
        | TBS_AUTOTICKS 
        | TBS_TOOLTIPS
        //| TBS_FIXEDLENGTH
        //| TBS_TRANSPARENTBKGND 
        //| TBS_NOTICKS 
        //| TBS_ENABLESELRANGE 
        , x, y, w, h, hwndParent, (HMENU)id, GetModuleHandle(NULL), NULL                  
    );

    SendMessage(hwndTrack, TBM_SETRANGE, (WPARAM)TRUE, (LPARAM)MAKELONG(iMin, iMax));  // min. & max. positions
    //SendMessage(hwndTrack, TBM_SETPAGESIZE,        0, (LPARAM)4);                  // new page size 
    SendMessage(hwndTrack, TBM_SETSEL, (WPARAM)TRUE, (LPARAM)MAKELONG(iSelMin, iSelMax));
    SendMessage(hwndTrack, TBM_SETPOS, (WPARAM)TRUE, (LPARAM)value);
    SendMessage( hwndTrack, WM_SETREDRAW, (WPARAM)TRUE, (LPARAM)NULL );
    SetFocus(hwndTrack);
    return hwndTrack;
}