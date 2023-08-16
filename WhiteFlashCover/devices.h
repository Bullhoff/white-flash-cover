#pragma once

#include "resource.h"


struct MonitorStruct {
    MONITORINFO info;
    HMONITOR hMonitor;
    HDC hdcMonitor;
    LPRECT lprcMonitor;
    LPARAM dwData;
    HWND hwnd;

    HWND hTrackbar;
    int pctAlpha;
};

vector<MonitorStruct> getMonitors();