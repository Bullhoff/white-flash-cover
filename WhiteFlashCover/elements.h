#pragma once
#include "resource.h"

HWND CreateToolTip(int toolID, HWND hDlg, LPCWSTR pszText);

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
);