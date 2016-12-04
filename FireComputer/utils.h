/*
    Copyright 2016 Chris Arrant
    
    This file is part of FireComputer.

    FireComputer is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    FireComputer is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with FireComputer.  If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef utils_h
#define utils_h

#include <stdio.h>
#include <stdarg.h>
#include <string>
#include <stdint.h>


#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wwrite-strings"

#define LogError( f, ... ) \
    { dbg_printf( "%s:%d ", __PRETTY_FUNCTION__, __LINE__); \
     dbg_printf( f, ##__VA_ARGS__); \
    dbg_printf("\n"); }

#define LogDebug( f, ... ) \
    { dbg_printf( "%s:%d ", __PRETTY_FUNCTION__, __LINE__); \
     dbg_printf( f, ##__VA_ARGS__); \
    dbg_printf("\n"); }

#ifdef ARDUINO

#define LogSerialResp( f, ... ) \
    { dbg_printf( f, ##__VA_ARGS__); }\
    
#else

#define LogSerialResp( f, ... ) \
    { m_serial.write( f, ##__VA_ARGS__); }\
    
#endif

//#pragma GCC diagnostic pop

void dbg_printf(char *fmt, ... );
#ifdef ARDUINO
void dbg_printf(const __FlashStringHelper *fmt, ... );
#endif

#ifndef ARDUINO
long long millis();
#endif

std::string Trim(const std::string& strIn);
std::string GetCSV(const std::string& strCSV, uint32_t u32Index, const char* pstrComma = NULL);

#endif // #ifndef utils_h


