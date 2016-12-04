/*
    Copyright 2016 Chris Arrant
    
    This file is part of FireModule.

    FireModule is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    FireModule is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with FireModule.  If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef utils_h
#define utils_h

#include <stdio.h>
#include <stdarg.h>
#include <Arduino.h>

#define LogError( f, ... ) \
    { dbg_printf( "%s:%d ", __PRETTY_FUNCTION__, __LINE__); \
     dbg_printf( f, ##__VA_ARGS__); \
    dbg_printf("\n"); }

#define LogDebug( f, ... ) \
    { dbg_printf( "%s:%d ", __PRETTY_FUNCTION__, __LINE__); \
     dbg_printf( f, ##__VA_ARGS__); \
    dbg_printf("\n"); }


void dbg_printf(char *fmt, ... );
void dbg_printf(const __FlashStringHelper *fmt, ... );

#endif // #ifndef utils_h


