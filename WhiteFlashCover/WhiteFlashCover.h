#pragma once

#include "resource.h"


struct ConfigStruct {
    BOOL console = false;
    BOOL minimizeToNotificationArea = true;
    BOOL startMinimized = false;
    BOOL autosave = false;

    BOOL flashCoverActive = true;
    string flashCoverMode = "cloak"; // showhide, alpha, cloak
    BOOL flashCoverOnRestore = true;
    BOOL flashCoverOnMaximize = true;
    int flashCoverAlphaPct = 80;
    int flashCoverTimeMs = 100;

    BOOL fullCoverActive = true;
    string fullCoverMode = "per_monitor"; // per_monitor, per_all    
    int fullCoverAlphaPct = 0;
    std::vector<int> fullCoverAlphaPct_i = {};
};

