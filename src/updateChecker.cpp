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
#include <iostream>
#include <unistd.h>
#include <jsoncpp/json/json.h>
#include "utilities.h"
#include "downloader.h"
#include "sender.h"
#include "updateChecker.h"

UpdateChecker::UpdateChecker()
{
    CheckForUpdate();
}

void UpdateChecker::CheckForUpdate()
{
    std::string url;
    Downloader dl;
    currentSettings = Settings::Instance().GetSettings();
    
    if (currentSettings.plexPassUser)
    {
        std::string authHeader = "X-Plex-Token:" + currentSettings.authToken;
        dl.setCustomHeader(authHeader);
        url = "https://plex.tv/api/downloads/1.json?channel=plexpass";
    }
    else
    { 
        url = "https://plex.tv/api/downloads/1.json";
    }
    
    std::string result = dl.download(url);

    Json::CharReaderBuilder readerBuilder;
    Json::CharReader *reader = readerBuilder.newCharReader();
    
    Json::Value rootObj;
    std::string err;
    
    bool sucess = reader->parse(result.c_str(), result.c_str() + result.size(), &rootObj, &err);
    
    if(!sucess)
      std::cout << "failed to parse json! here is the error: " << err << std::endl;

    delete reader;

    Json::Value linuxObj = rootObj["computer"]["Linux"];
    updateVer = linuxObj["version"].asString();
    Json::Value releasesArray = linuxObj["releases"];

    if (releasesArray.size() != 0)
    {
        for( Json::Value::ArrayIndex i = 0; i < releasesArray.size() ; i++)
        {
            std::string distro = releasesArray[i]["distro"].asString();
            std::string build = releasesArray[i]["build"].asString();

            if (distro == currentSettings.osType &&
                build.find(currentSettings.osArch) != std::string::npos)
            {
                downloadUrl = releasesArray[i]["url"].asString();

                std::size_t pos = downloadUrl.find_last_of('/') + 1;
                downloadFileName = downloadUrl.substr(pos, -1);
                GetCurrentVersion();
                break;
            }
        }
    }
}

void UpdateChecker::GetCurrentVersion()
{
    Downloader dl;
    std::string authHeader = "X-Plex-Token:" + currentSettings.authToken;
    dl.setCustomHeader(authHeader);
    std::string url = "localhost:" + currentSettings.serverPort;
    std::string result = dl.download(url);

    size_t pos = result.find("updater");

    if (pos != std::string::npos)
    {
        std::string parse1 = result.substr(pos, -1);
        pos = parse1.find("version");
        std::string parse2 = parse1.substr(pos, -1);
        pos = parse2.find(' ');
        std::string tokenData = parse2.substr(0, pos);

        currentVer = ParseLine(tokenData);
    }

    // TODO: Make a better version check
    if (updateVer != currentVer)
      CheckForMediaPlayback();
}

void UpdateChecker::CheckForMediaPlayback()
{
    Downloader dl;
    std::string authHeader = "X-Plex-Token:" + currentSettings.authToken;
    dl.setCustomHeader(authHeader);
    std::string url = "localhost:" + currentSettings.serverPort + "/status/sessions";
    std::string result = dl.download(url);

    size_t pos = result.find("MediaContainer size");

    if (pos != std::string::npos)
    {
        std::string parse = result.substr(pos, -1);
        pos = parse.find('>');
        std::string tokenData = parse.substr(0, pos);
        std::string tokenValue = ParseLine(tokenData);

        unsigned int activeNum = StringToInt(tokenValue);

        if (activeNum <= 0)
          StartDownload();
    }
}

void UpdateChecker::SendMsg()
{
    Sender snd;
    Json::Value root;

    std::string pushUrl = "https://maker.ifttt.com/trigger/" + currentSettings.pushApiEventName + "/with/key/" + currentSettings.pushApiToken;
    
    root["value1"] = "PlexAuto\n";
    root["value2"] = "Your Plex server has been updated from " + currentVer + " to " + updateVer + "\n";

    Json::StyledWriter writer;
    std::string jsonReq = writer.write(root);

    std::string contentType = "Content-Type: application/json";
    snd.setCustomHeader(contentType);
    snd.send(pushUrl, jsonReq); 
}

void UpdateChecker::StartDownload()
{
    std::string name = currentSettings.downloadLocation + "/" + downloadFileName;
    FILE *file = fopen(name.c_str(), "wb");

    Downloader dl;
    dl.download(downloadUrl, file);
    fclose(file);

    ApplyUpdate();
}

void UpdateChecker::ApplyUpdate()
{
    std::string cmd;

    if (currentSettings.osType == "ubuntu")
        cmd = "dpkg -i " + currentSettings.downloadLocation + "/" + downloadFileName;

    int complete = std::system(cmd.c_str());

    if ((complete > -1) && currentSettings.pushNotifications)
       SendMsg();
}
