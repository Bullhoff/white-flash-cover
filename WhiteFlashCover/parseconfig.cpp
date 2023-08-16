
#include "framework.h"
#include "WhiteFlashCover.h"
#include <fstream>


int SaveConfig(ConfigStruct Config) {
    if (Config.console) cout << "\n!!!!!!!!!!!!!! SaveConfig !!!!!!!!!!!!!!!!" << " " << endl;

    std::ostringstream oss; //std::basic_stringstream oss; 
    oss
        << "startMinimized=" << Config.startMinimized
        << "\nminimizeToNotificationArea=" << Config.minimizeToNotificationArea
        << "\nautosave=" << Config.autosave
        << "\nflashCoverActive=" << Config.flashCoverActive
        << "\nflashCoverMode=" << Config.flashCoverMode
        << "\nflashCoverOnRestore=" << Config.flashCoverOnRestore
        << "\nflashCoverOnMaximize=" << Config.flashCoverOnMaximize
        << "\nflashCoverAlphaPct=" << Config.flashCoverAlphaPct
        << "\nflashCoverTimeMs=" << Config.flashCoverTimeMs
        << "\nfullCoverActive=" << Config.fullCoverActive
        << "\nfullCoverMode=" << Config.fullCoverMode
        << "\nfullCoverAlphaPct=" << Config.fullCoverAlphaPct
        << "\nfullCoverAlphaPct_i=" << "";
    for (size_t i = 0; i < Config.fullCoverAlphaPct_i.size(); ++i)
        oss << std::to_string(Config.fullCoverAlphaPct_i[i]) + ",";
    if (Config.console != 0) oss << "\nconsole=" << Config.console;
    std::string str = oss.str();

    ofstream myfile;
    myfile.open("config.ini");
    myfile << str;
    myfile.close();
    if (Config.console) cout << "str: \n" << str << "\n" << endl;
    return 0;
}


std::map<std::string, std::string> readConfigFile() {
    std::map<std::string, std::string> tempSettings;
    std::ifstream inputFile("config.ini");

    if (inputFile.is_open()) {
        std::string line;
        while (std::getline(inputFile, line)) {
            std::string delimiter = "=";
            std::string key = line.substr(0, line.find(delimiter)); 
            std::string value = line.erase(0, line.find(delimiter) + delimiter.length());
            tempSettings[key] = value;
        }
        inputFile.close();
    }
    return tempSettings;
}

int LoadConfig(ConfigStruct  &conf) {
    std::map<std::string, std::string> file = readConfigFile();
   
    for (auto it = file.begin(); it != file.end(); it++){
        if (it->first == "console") conf.console = atoi(it->second.c_str());
        if (it->first == "startMinimized") conf.startMinimized = atoi(it->second.c_str());
        if (it->first == "minimizeToNotificationArea") conf.minimizeToNotificationArea = atoi(it->second.c_str());
        if (it->first == "autosave") conf.autosave = atoi(it->second.c_str());
        if (it->first == "flashCoverActive") conf.flashCoverActive = atoi(it->second.c_str());
        if (it->first == "flashCoverMode") conf.flashCoverMode = it->second;
        if (it->first == "flashCoverOnRestore") conf.flashCoverOnRestore = atoi(it->second.c_str());
        if (it->first == "flashCoverOnMaximize") conf.flashCoverOnMaximize = atoi(it->second.c_str());
        if (it->first == "flashCoverAlphaPct") conf.flashCoverAlphaPct = atoi(it->second.c_str());
        if (it->first == "flashCoverTimeMs") conf.flashCoverTimeMs = atoi(it->second.c_str());
        if (it->first == "fullCoverActive") conf.fullCoverActive = atoi(it->second.c_str());
        if (it->first == "fullCoverMode") conf.fullCoverMode = it->second;
        if (it->first == "fullCoverAlphaPct") conf.fullCoverAlphaPct = atoi(it->second.c_str());
        if (it->first == "fullCoverAlphaPct_i") {
                string s;
                stringstream ss(it->second.c_str());
                while (getline(ss, s, ',')) {
                    stringstream convert(atoi(s.c_str()));
                    int val = atoi(s.c_str());
                    conf.fullCoverAlphaPct_i.push_back(val);
                }
        }
    }
    return 0;
}


