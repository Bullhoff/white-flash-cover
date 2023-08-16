#include "framework.h"
#include "utils.h"

HICON CreateGradientColorIcon(COLORREF iconColor, int width, int height) {
    // https://stackoverflow.com/a/41538939
    HICON hIcon = 0;
    ICONINFO ii = { TRUE };
    ULONG n = width * height;
    if (PULONG lpBits = new ULONG[n]) {
        PULONG p = lpBits;
        ULONG x, y = height, t;
        do {
            x = width, t = --y << 8;
            do {
                *p++ = iconColor | ((t * --x) / n << 24);
            } while (x);

        } while (y);

        if (ii.hbmColor = CreateBitmap(width, height, 1, 32, lpBits)) {
            if (ii.hbmMask = CreateBitmap(width, height, 1, 1, 0)) {
                hIcon = CreateIconIndirect(&ii);
                DeleteObject(ii.hbmMask);
            }
            DeleteObject(ii.hbmColor);
        }
        delete[] lpBits;
    }

    return hIcon;
}



BOOL IsMaximized(HWND hwnd) {
    WINDOWPLACEMENT lpwndpl;
    lpwndpl.length = sizeof(WINDOWPLACEMENT);
    GetWindowPlacement(hwnd, &lpwndpl);
    BOOL maxed = lpwndpl.showCmd == SW_SHOWMAXIMIZED;
    //WindowState.maxed = maxed;
    return maxed;
}
BOOL IsOwner(HWND hwnd) {
    HWND ownerhwnd = GetAncestor(hwnd, GA_ROOT);    // GW_OWNER
    BOOL isOwner = (ownerhwnd == hwnd);
    return isOwner;
}
BOOL IsTopLevel(HWND hwnd) {
    LONG style = GetWindowLong(hwnd, GWL_STYLE);
    BOOL isTopLevel = !(style & WS_CHILD);
    return isTopLevel;
}
string getClassName(HWND hwnd) {
    wchar_t win_class[_MAX_PATH];
    int status = GetClassNameW(hwnd, win_class, _MAX_PATH);
    wstring ws(win_class);
    string newstr(ws.begin(), ws.end());
    return newstr;
}
string getWindowText(HWND hwnd) {
    wchar_t win_text[_MAX_PATH];
    int status = GetWindowTextW(hwnd, win_text, _MAX_PATH);
    wstring ws(win_text);
    string newstr(ws.begin(), ws.end());
    return newstr;
}