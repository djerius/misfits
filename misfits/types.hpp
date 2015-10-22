// -*-c++-*-

// --8<--8<--8<--8<--
//
// Copyright (C) 2015 Smithsonian Astrophysical Observatory
//
// This file is part of misfits
//
// misfits is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or (at
// your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <http://www.gnu.org/licenses/>.
//
// -->8-->8-->8-->8--

#ifndef misFITS_TYPES_H
#define misFITS_TYPES_H

#include <string>
#include <map>

#include <cfitsio/fitsio.h>

namespace misFITS {

    enum StorageType {
	SC_BYTE        = TBYTE,
	SC_DOUBLE      = TDOUBLE,
	SC_FLOAT       = TFLOAT,
	SC_INT         = TINT,
	SC_LONG        = TLONG,
	SC_SHORT       = TSHORT,
	SC_STRING      = TSTRING,
	SC_UINT        = TUINT,
	SC_ULONG       = TULONG,
	SC_USHORT      = TUSHORT,
	SC_UNDEF       = 0
    };

    enum HDU_Type {
	Image = IMAGE_HDU,
	AsciiTable = ASCII_TBL,
	BinaryTable =  BINARY_TBL
    };


    template <typename T> struct StorageCode;
    template <> struct StorageCode<double>         {
	static const StorageType type = SC_DOUBLE;
	static double default_value () { return 0.0 ;}
    };

    template <> struct StorageCode<float>          {
	static const StorageType type = SC_FLOAT;
	static float default_value () { return 0.0 ;}
    };

    template <> struct StorageCode<int>            {
	static const StorageType type = SC_INT;
	static int default_value () { return 0; }
    };

    template <> struct StorageCode<long>           {
	static const StorageType type = SC_LONG;
	static long default_value () { return 0; }
    };

    template <> struct StorageCode<short>          {
	static const StorageType type = SC_SHORT;
	static short default_value () { return 0; }
    };

    template <> struct StorageCode<unsigned char>  {
	static const StorageType type = SC_BYTE;
	static unsigned char default_value () { return 0; }
    };

    template <> struct StorageCode<unsigned int>   {
	static const StorageType type = SC_UINT;
	static unsigned int default_value () { return 0; }
    };

    template <> struct StorageCode<unsigned long>  {
	static const StorageType type = SC_ULONG;
	static unsigned long default_value () { return 0; }
    };

    template <> struct StorageCode<unsigned short> {
	static const StorageType type = SC_USHORT;
	static unsigned short default_value () { return 0; }
    };

    template <> struct StorageCode<std::string> {
	static const char* default_value () { return ""; }
    };


    enum ColumnType {
	CT_BIT 	       = 'X',
	CT_BYTE        = 'B',
	CT_LOGICAL     = 'L',
	CT_STRING      = 'A',
	CT_SHORT       = 'I',
	CT_INT32BIT    = 'J',
	CT_LONG        = 'J',
	CT_LONGLONG    = 'K',
	CT_FLOAT       = 'E',
	CT_DOUBLE      = 'D',
	CT_COMPLEX     = 'C',
	CT_DBL_COMPLEX = 'M'
    };

    class ColumnCode {

    public:
	typedef std::map<char,const char*> Map;
	static Map code;

    };



}

#endif // ! misFITS_TYPES_H
