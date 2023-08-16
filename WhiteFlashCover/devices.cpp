#include "framework.h"
#include "devices.h"



std::map<std::string, MonitorStruct> monitors;
vector<MonitorStruct> vMonitors;


static BOOL CALLBACK MonitorEnumProc(HMONITOR hMonitor, HDC hdcMonitor, LPRECT lprcMonitor, LPARAM dwData) {
    MONITORINFO info;
    info.cbSize = sizeof(info);
    if (GetMonitorInfo(hMonitor, &info)) {
        vMonitors.push_back({ info, hMonitor, hdcMonitor, lprcMonitor, dwData });
    }
    return true;
}
vector<MonitorStruct> getMonitors() {
    EnumDisplayMonitors(NULL, NULL, MonitorEnumProc, 0);
    return vMonitors;
}
