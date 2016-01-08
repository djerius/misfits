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
#include <vector>
#include <map>

#include <boost/core/scoped_enum.hpp>

#include <fitsio.h>

namespace misFITS {

    enum StorageType {
	SC_BYTE        = TBYTE,
	SC_DOUBLE      = TDOUBLE,
	SC_FLOAT       = TFLOAT,
	SC_INT         = TINT,
	SC_LONG        = TLONG,
	SC_LONGLONG    = TLONGLONG,
	SC_SHORT       = TSHORT,
	SC_STRING      = TSTRING,
	SC_UINT        = TUINT,
	SC_ULONG       = TULONG,
	SC_USHORT      = TUSHORT,
	SC_UNDEF       = 0
    };

    BOOST_SCOPED_ENUM_DECLARE_BEGIN( HDU_Type )
    {
	Image 	    = IMAGE_HDU,
	AsciiTable  = ASCII_TBL,
	BinaryTable = BINARY_TBL,
	Any         = ANY_HDU
    }
    BOOST_SCOPED_ENUM_DECLARE_END( HDU_Type)


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

    template <> struct StorageCode<unsigned int>   {
	static const StorageType type = SC_UINT;
	static unsigned int default_value () { return 0; }
    };


    template <> struct StorageCode<long>           {
	static const StorageType type = SC_LONG;
	static long default_value () { return 0; }
    };

    template <> struct StorageCode<unsigned long>  {
	static const StorageType type = SC_ULONG;
	static unsigned long default_value () { return 0; }
    };

    template <> struct StorageCode<LONGLONG>           {
	static const StorageType type = SC_LONGLONG;
	static long default_value () { return 0; }
    };

    template <> struct StorageCode<short>          {
	static const StorageType type = SC_SHORT;
	static short default_value () { return 0; }
    };

    template <> struct StorageCode<unsigned short> {
	static const StorageType type = SC_USHORT;
	static unsigned short default_value () { return 0; }
    };

    template <> struct StorageCode<char>  {
	static const StorageType type = SC_BYTE;
	static unsigned char default_value () { return 0; }
    };

    template <> struct StorageCode<unsigned char>  {
	static const StorageType type = SC_BYTE;
	static unsigned char default_value () { return 0; }
    };

    template <> struct StorageCode<std::string> {
	static const StorageType type = SC_STRING;
	static const char* default_value () { return ""; }
    };

    template <> struct StorageCode< std::vector<std::string> > {
	static const StorageType type = SC_STRING;
    };

    template <StorageType T> struct NativeType;

    template <> struct NativeType<SC_DOUBLE> { typedef double 	       storage_type; };
    template <> struct NativeType<SC_FLOAT>  { typedef float 	       storage_type; };
    template <> struct NativeType<SC_INT>    { typedef int 	       storage_type; };
    template <> struct NativeType<SC_LONG>   { typedef long 	       storage_type; };
    template <> struct NativeType<SC_SHORT>  { typedef short 	       storage_type; };
    template <> struct NativeType<SC_BYTE>   { typedef unsigned char   storage_type; };
    template <> struct NativeType<SC_UINT>   { typedef unsigned int    storage_type; };
    template <> struct NativeType<SC_ULONG>  { typedef unsigned long   storage_type; };
    template <> struct NativeType<SC_USHORT> { typedef unsigned short  storage_type; };


    // these must be listed such that the last entry for a valid
    // CFITSIO enum is the largest


    BOOST_SCOPED_ENUM_DECLARE_BEGIN( ColumnType )
    {
	    Bit 	  = TBIT,
	    Byte          = TBYTE,
	    Logical       = TLOGICAL,
	    String        = TSTRING,
	    Short         = TSHORT,
	    Long          = TINT32BIT,
	    Int32bit      = TINT32BIT,
	    LongLong      = TLONGLONG,
	    Float         = TFLOAT,
	    Double        = TDOUBLE,
	    Complex       = TCOMPLEX,
	    DoubleComplex = TDBLCOMPLEX,
	    UnsignedShort,
	    UnsignedLong
    }
    BOOST_SCOPED_ENUM_DECLARE_END( ColumnType)

    class ColumnCode {

    public:
	typedef std::map<ColumnType,const char*> Map;
	static Map code;

    };


    struct FileCopy {
	enum Flag
	    {
		PreviousHDUs   = 1,
		CurrentHDU     = 2,
		FollowingHDUs  = 4,
		AllHDUs        = PreviousHDUs | CurrentHDU | FollowingHDUs,
		CurrentHeader  = 8
	    };
    };

}

#endif // ! misFITS_TYPES_H
