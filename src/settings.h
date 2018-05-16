/*********************************************************************************
MIT License

Copyright (c) 2018 Keith Lewis

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*********************************************************************************/
#ifndef SETTINGS_H
#define SETTINGS_H

#include <string>

struct SETTINGS
{
    std::string osType;
    std::string osArch;
    std::string osName;

    bool plexPassUser;
    std::string authToken;
    unsigned int checkUpdateTime;
    std::string plexPrefFileLocation;
    std::string downloadLocation;
    std::string serverPort;
    bool pushNotifications;
    std::string pushApiToken;
};

class Settings
{  
public:
    Settings();
    static Settings& Instance();

    SETTINGS inline GetSettings() { return settings; }

private:
    SETTINGS settings;

    void GetOsName();
    void ReadAuthToken();
    bool ReadSettings();
    void WriteSettings();
};

#endif // SETTINGS_H 
