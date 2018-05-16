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
#include <sstream>
#include "downloader.h" 

size_t onDataRecieved(char *ptr, size_t size, size_t nmemb, void *stream)
{  
    std::string data((const char*) ptr, (size_t) size * nmemb);
    *((std::stringstream*) stream) << data << std::endl;
    return size * nmemb;
}

size_t onFileDataRecieved(void *ptr, size_t size, size_t nmemb, FILE *file)
{   
    size_t bytesWriten = fwrite(ptr, size, nmemb, file);
    
    return bytesWriten;
}

Downloader::Downloader()
{
    headerFlag = false;
    curl = curl_easy_init();
    headerList = NULL;
}

Downloader::~Downloader()
{
    if (headerList != NULL)
        curl_slist_free_all(headerList);
    
    curl_easy_cleanup(curl);
}

void Downloader::setCustomHeader(std::string header)
{
    headerList = curl_slist_append(headerList, header.c_str());
}

std::string Downloader::download(std::string url, FILE *file)
{
    CURLcode result;
    
    if (curl)
    {
        std::stringstream downloadedData;
        curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
        
        if (headerList != NULL)
            curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headerList);
        
        if (file != NULL)
        {
            curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, onFileDataRecieved);
            curl_easy_setopt(curl, CURLOPT_WRITEDATA, file); 
        }
        else
        {
            curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, onDataRecieved);
            curl_easy_setopt(curl, CURLOPT_WRITEDATA, &downloadedData); 
        }
        
        result = curl_easy_perform(curl);  
        
        if (result != CURLE_OK)
           std::cout << "Download error: " << curl_easy_strerror(result) << std::endl;
        
        return downloadedData.str();
    }
    
    return "curl object not properly initilized";
}
