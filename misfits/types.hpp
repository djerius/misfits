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
#include <boost/container/vector.hpp>

#include <fitsio.h>

#include <misfits/config.hpp>
#include <misfits/bitset.hpp>

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
	SC_LOGICAL     = TLOGICAL,
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

    template <> struct StorageCode<bool>          {
	static const StorageType type = SC_LOGICAL;
	static short default_value () { return 0; }
    };

    template <> struct StorageCode<std::string> {
	static const StorageType type = SC_STRING;
	static const char* default_value () { return ""; }
    };

    template <> struct StorageCode< std::vector<std::string> > {
	static const StorageType type = SC_STRING;
    };


    // These are the types that CFITSIO maps to its enums when using
    // the generic datatype read/write routines.  misFITS should
    // really be using the type specific I/O routines and avoid the
    // dispatch tables, but that's for another release.

    template <StorageType T> struct NativeType;

    template <> struct NativeType<SC_DOUBLE>  { typedef double 	       	storage_type; };
    template <> struct NativeType<SC_FLOAT>   { typedef float 	       	storage_type; };
    template <> struct NativeType<SC_INT>     { typedef int 	       	storage_type; };
    template <> struct NativeType<SC_LONG>    { typedef long 	       	storage_type; };
    template <> struct NativeType<SC_SHORT>   { typedef short 	       	storage_type; };
    template <> struct NativeType<SC_BYTE>    { typedef unsigned char   storage_type; };
    template <> struct NativeType<SC_UINT>    { typedef unsigned int    storage_type; };
    template <> struct NativeType<SC_ULONG>   { typedef unsigned long   storage_type; };
    template <> struct NativeType<SC_USHORT>  { typedef unsigned short  storage_type; };
    template <> struct NativeType<SC_LOGICAL> { typedef bool  		storage_type; };



    namespace ColumnType {

	namespace ID {

	    typedef int type;

	   // these must be listed such that the last entry for a valid
	   // CFITSIO enum is the largest
	    enum {
		Bit 	      = TBIT,
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
		UShort,
		ULong
	    };

	    template <type T> struct NativeType 	    { };
	    template <> struct NativeType<Bit> 	   	    { typedef BitSet  	  		storage_type; };
	    template <> struct NativeType<Byte> 	    { typedef uint8_t  	  		storage_type; };
	    template <> struct NativeType<Logical> 	    { typedef bool  	  		storage_type; };
	    template <> struct NativeType<String> 	    { typedef std::string  		storage_type; };
	    template <> struct NativeType<Short>   	    { typedef int16_t 	  		storage_type; };

	    template <> struct NativeType<Long>    	    { typedef int32_t 	  		storage_type; };
	    // the following is the same as Long
	    // template <> struct NativeType<Int32bit>      { typedef int32_t      		storage_type; };

	    template <> struct NativeType<LongLong>    	    { typedef int64_t   	  	storage_type; };
	    template <> struct NativeType<Float>   	    { typedef float 	  		storage_type; };
	    template <> struct NativeType<Double>   	    { typedef double 	  		storage_type; };
	    template <> struct NativeType<UShort>   	    { typedef uint16_t 	  		storage_type; };
	    template <> struct NativeType<ULong>   	    { typedef uint32_t 	  		storage_type; };


	    // we don't  support complex types yet
	    // template <> struct NativeType<Complex>       { typedef std::complex<float> 	storage_type; };
	    // template <> struct NativeType<DoubleComplex> { typedef std::complex<double> 	storage_type; };

	}

	class Spec {

	public:
	    virtual ID::type id() = 0;
	    virtual char code() = 0;

	    // return the width in chars of a column with the given
	    // type and extent
	    size_t width( size_t repeat );

	protected:
	    friend shared_ptr<Spec> spec_from_id( ID::type id);
	    Spec() {};
	};


	template <ID::type T> class Impl : public Spec {

	public:
	    typedef typename ID::NativeType<T>::storage_type  atomic_type;
	    typedef std::vector<atomic_type> vector_type;
	    ID::type id() { return T; }
	    char code();
	};

	template <> class Impl< ID::Bit > : public Spec {

	public:
	    typedef ID::NativeType<ID::Bit>::storage_type atomic_type;
	    typedef atomic_type vector_type;
	    ID::type id() { return ID::Bit; }
	    char code()   { return 'X'; }
	};

	template <> class Impl< ID::Logical > : public Spec {

	public:
	    typedef ID::NativeType<ID::Logical>::storage_type atomic_type;
	    typedef boost::container::vector<atomic_type> vector_type;
	    ID::type id() { return ID::Logical; }
	    char code() { return 'L'; }
	};

	typedef shared_ptr<Spec> SpecPtr;

	SpecPtr spec_from_id( ID::type id );

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

    BOOST_SCOPED_ENUM_DECLARE_BEGIN( OpenMode )
    {
	ReadOnly = READONLY,
        ReadWrite = READWRITE
    }
    BOOST_SCOPED_ENUM_DECLARE_END( OpenMode )

    BOOST_SCOPED_ENUM_DECLARE_BEGIN( FlushMode )
    {
	File, Buffer
    }
    BOOST_SCOPED_ENUM_DECLARE_END( FlushMode )

}

#endif // ! misFITS_TYPES_H
