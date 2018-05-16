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
#include <fstream>
#include <iostream>
#include <string>
#include "utilities.h"
#include "settings.h"

Settings::Settings()
{ 
    if (!ReadSettings())
      WriteSettings();
    
    int cpuArch = sizeof(void*);
    
    if (cpuArch == 8)
      settings.osArch = "x86_64";
    else
      settings.osArch = "i686";
    
    settings.osType = "ubuntu";

    GetOsName();

    if (settings.plexPassUser)
      ReadAuthToken();
}

Settings& Settings::Instance()
{
    static Settings object;
    return object;
}
 
void Settings::GetOsName()
{
    std::fstream version("/proc/version");
    
    if (version.is_open())
    {
        std::string line;
    
        while (getline(version, line))
        {           
            if(line.find("Ubuntu") != std::string::npos)
            {
              settings.osName = "ubuntu";
              break;
            }
            else if (line.find("redhat") != std::string::npos || line.find("fedora") != std::string::npos)
            {
              settings.osName = "redhat";
              break;
            }
            else if (line.find("suse") != std::string::npos)
            {
              settings.osName = "suse";
              break;
            }
            else
            {
              settings.osName = "ubuntu";
            }
        }
        
        version.close();   
    }
}

bool Settings::ReadSettings()
{
    std::fstream file("plexauto.conf");
    
    if (file.is_open())
    {
       std::string line;
    
        while (getline(file, line))
        {          
            if (line.find("PLEX_PASS_USER") != std::string::npos)
              settings.plexPassUser = (ParseLine(line) == "1") ? true : false;
            if (line.find("CHECK_FOR_UPDATE_TIME") != std::string::npos)
              settings.checkUpdateTime = StringToInt(ParseLine(line));
            if (line.find("PLEX_PREF_LOCATION") != std::string::npos)
              settings.plexPrefFileLocation = ParseLine(line);
            if (line.find("DOWNLOAD_LOCATION") != std::string::npos)
              settings.downloadLocation = ParseLine(line);
            if (line.find("SERVER_PORT") != std::string::npos)
              settings.serverPort = ParseLine(line);
            if (line.find("PUSH_NOTIFICATIONS") != std::string::npos)
              settings.pushNotifications = (ParseLine(line) == "1") ? true : false;
            if (line.find("PUSHBULLET_API_TOKEN") != std::string::npos)
              settings.pushApiToken = ParseLine(line);
        }

        file.close();

        return true;
    }
    else
    {
        return false;
    }
}

void Settings::WriteSettings()
{
    std::ofstream file("plexauto.conf");

    if (file.is_open())
    {
        file << "###########################################################" << "\n";
        file << "# plexauto.conf" << "\n";
        file << "# Configuraion file for the PlexAuto application" << "\n";
        file << "###########################################################" << "\n\n";

        file << "# Change to 0 if you are not a plex pass user" << "\n";
        file << "# valid values are 0 and 1" << "\n";
        file << "PLEX_PASS_USER=1" << "\n\n";

        file << "# Check for updates. Time is in hours." << "\n";
        file << "CHECK_FOR_UPDATE_TIME=24" << "\n\n";

        file << "# Location of the plex preferences file" << "\n";
        file << "PLEX_PREF_LOCATION=/var/lib/plexmediaserver/Library/Application Support/Plex Media Server" << "\n\n";

        file << "# Location were plexauto will download the update files" << "\n";
        file << "DOWNLOAD_LOCATION=/tmp" << "\n\n";

        file << "# Plex sever port number" << "\n";
        file << "SERVER_PORT=32400" << "\n\n";

        file << "# Change to 0 to disable push notifications" << "\n";
        file << "# valid values are 0 and 1" << "\n";
        file << "PUSH_NOTIFICATIONS=1" << "\n\n";

        file << "# Enter your pushbullet api key here if push notifications are enabled" << "\n";
        file << "PUSHBULLET_API_TOKEN=12345" << "\n";

        file.close();
    }
}

void Settings::ReadAuthToken(void)
{
    std::string fileNameAndPath = settings.plexPrefFileLocation + "/Preferences.xml";
    std::fstream preferences(fileNameAndPath);

    if (preferences.is_open())
    {
        std::string line;

        while (getline(preferences, line))
        {
            size_t pos = line.find("PlexOnlineToken");

            if (pos != std::string::npos)
            {
                std::string remainder = line.substr(pos, -1);
                pos = remainder.find(' ');
                std::string tokenData = remainder.substr(0, pos);

                settings.authToken = ParseLine(tokenData);
            }
        }

        preferences.close();
    }
}
