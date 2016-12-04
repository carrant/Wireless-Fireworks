/*
    Copyright 2016 Chris Arrant
    
    This file is part of FireworksGuiAndroid.

    FireworksGuiAndroid is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    FireworksGuiAndroid is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with FireworksGuiAndroid.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "utils.h"

//problem! you have to iunclude qglobal for this
#define QT_VERSION

#ifdef QT_VERSION
extern void g_LogText(const char *s);
#endif

void dbg_printf(char *fmt, ... )
{
#ifdef ARDUINO
    char buf[128]; // resulting string limited to 128 chars
#else
  char buf[1024]; // resulting string limited to 128 chars
#endif
  va_list args;
  va_start (args, fmt );
#ifdef ARDUINO
  vsnprintf(buf, 128, fmt, args);
#else
  vsnprintf(buf, 1024, fmt, args);
#endif
  va_end (args);
#ifdef ARDUINO
  Serial.print(buf);
#elif defined(QT_VERSION)
  g_LogText(buf);
#else
  printf("%s", buf);
#endif
}

#ifdef ARDUINO
void dbg_printf(const __FlashStringHelper *fmt, ... )
{
  char buf[128]; // resulting string limited to 128 chars
  va_list args;
  va_start (args, fmt);
#ifdef __AVR__
  vsnprintf_P(buf, sizeof(buf), (const char *)fmt, args); // progmem for AVR
#else
  vsnprintf(buf, sizeof(buf), (const char *)fmt, args); // for the rest of the world
#endif
  va_end(args);
  Serial.print(buf);
}
#endif

//------------------------------------------------------------------------------
//  GetCSV
//! Return strValue from strCSV
//------------------------------------------------------------------------------
std::string GetCSV(const std::string& strCSV, uint32_t u32Index, const char* pstrComma)
{
    if (!pstrComma || *pstrComma == '\0')
        pstrComma = ",";

    std::string strValue;
    size_t pos0 = 0;
    size_t pos1 = 0;

    uint32_t i;
    for ( i = 0; i < u32Index; i++ )
    {
        pos1 = strCSV.find_first_of(pstrComma, pos0);
        if (pos1 == std::string::npos)
            break;
        pos0 = pos1 + 1;
    }

    if (i == u32Index)
    {
        pos1 = strCSV.find_first_of(pstrComma, pos0);
        if (pos1 != std::string::npos)
            strValue = strCSV.substr(pos0, pos1-pos0);
        else
            strValue = strCSV.substr(pos0);
    }
    return Trim(strValue);
}

std::string Trim(const std::string& strIn)
{
    std::string strOut = strIn;
    const char* pstrWhiteSpace = " \r\n\t";
    strOut.erase(0, strOut.find_first_not_of(pstrWhiteSpace));
    strOut.erase(strOut.find_last_not_of(pstrWhiteSpace)+1);
    return strOut;
}

#ifndef ARDUINO
#include <sys/time.h>

long long millis() 
{
    static long long llStart = 0;
    
    struct timeval te; 
    gettimeofday(&te, NULL); // get current time
    long long milliseconds = te.tv_sec*1000LL + te.tv_usec/1000; // caculate milliseconds
    // printf("milliseconds: %lld\n", milliseconds);
    
    if (!llStart)
        llStart = milliseconds;

    //arduino is milliseconds from start
    return milliseconds - llStart;
}
#endif

