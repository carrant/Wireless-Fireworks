/*
    Copyright 2016 Chris Arrant
    
    This file is part of FireControl.

    FireControl is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    FireControl is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with FireControl.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "utils.h"

void dbg_printf(char *fmt, ... )
{
  char buf[128]; // resulting string limited to 128 chars
  va_list args;
  va_start (args, fmt );
  vsnprintf(buf, 128, fmt, args);
  va_end (args);
  Serial.print(buf);
}

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



//------------------------------------------------------------------------------
//  GetCSV
//! Return strValue from strCSV
//------------------------------------------------------------------------------
String GetCSV(const String& strCSV, uint8_t u8Index, const char* pstrComma)
{
    if (!pstrComma || *pstrComma == '\0')
        pstrComma = ",";

    String strValue;
    size_t pos0 = 0;
    size_t pos1 = 0;

    uint8_t i;
    for ( i = 0; i < u8Index; i++ )
    {
        pos1 = strCSV.indexOf(pstrComma, pos0);
        if (pos1 == -1)
            break;
        pos0 = pos1 + 1;
    }

    if (i == u8Index)
    {
        pos1 = strCSV.indexOf(pstrComma, pos0);
        if (pos1 != -1)
            strValue = strCSV.substring(pos0, pos1);
        else
            strValue = strCSV.substring(pos0);
    }
    
    strValue.trim();
    return strValue;
}

