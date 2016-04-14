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

#include <numeric>
#include <string>
#include <sstream>

using namespace std;

#include <fitsio.h>

#include "misfits/fits.hpp"
#include "misfits/table.hpp"

namespace misFITS {

    bool ColumnInfo::operator == (const ColumnInfo& col ) const {
	return
	    col.column_type  == column_type &&
	    col.extent       == extent;
    }

    void
    ColumnInfo::init( const File& file ) {

	char ttype_t[80];
	char tunit_t[80];

	if ( file.hdu_type() != HDU_Type::BinaryTable )
	    throw Exception::Assert( "only Binary Tables are supported" );

	misFITS_CHECK_CFITSIO_EXPR
	    ( fits_get_bcolparmsll( file.fptr(), colnum,
				    ttype_t, tunit_t,
				    NULL, // typechar
				    NULL, // repeat
				    NULL, // scale,
				    NULL, // zero,
				    NULL, // nulval,
				    NULL, // tdisp,
				    &status )
	      );

	tunit = string( tunit_t );
	ttype = string( ttype_t );

	int typecode;
	long width;
	LONGLONG repeat;

#if SIZEOF_LONG == SIZEOF_LONGLONG
	misFITS_CHECK_CFITSIO_EXPR
	    ( fits_get_coltypell( file.fptr(),
				  colnum,
				  &typecode,
				  &repeat,
				  reinterpret_cast<LONGLONG*>(&width),
				  &status )
	      );
#else
	// fits_get_coltypell is anomolous in requiring a LONGLONG
	// width. c.f. CFITSIO internals and other routines with a width parameter
	LONGLONG twidth;
	misFITS_CHECK_CFITSIO_EXPR
	    ( fits_get_coltypell( file.fptr(),
				  colnum,
				  &typecode,
				  &repeat,
				  &twidth,
				  &status )
	      );
	width = twidth;
#endif // LONGLONG FUDGE

	column_type = static_cast<ColumnType>(typecode);

	int naxis;
	misFITS_CHECK_CFITSIO_EXPR
	    ( fits_read_tdimll( file.fptr(), colnum, 0, &naxis, NULL, &status )
	      );

	// handle CFITSIO idiosyncracies with 'A' columns. see tests/cfitsio.cc
	if ( ColumnType::String == column_type && naxis == 1 ) {

	    extent.resize(2);
	    extent[0] = width;

	    // N.B. CFITSIO sets width = repeat if no width is specified
	    extent[1] = repeat / width ;
	}
	else {

	    extent.resize(naxis);
	    misFITS_CHECK_CFITSIO_EXPR
		( fits_read_tdimll( file.fptr(),
				    colnum,
				    naxis,
				    &naxis,
				    &extent[0],
				    &status )
		  );
	}


	LONGLONG nelem_ = extent.nelem();

	// the column width for TBIT is reported as 1 (byte), which is
	// not useful in calculating the true number of bytes in the
	// column
	if ( ColumnType::Bit == column_type ) {
	    nbytes = nelem_ / 8;
	    if ( nbytes * 8  < nelem_ ) nbytes += 1;
	}
	else {
	    nbytes = nelem_ * ( ColumnType::String == column_type ? 1 : width );
	}
    }



    ColumnInfo::ColumnInfo( const File& file, const std::string& ttype, LONGLONG offset ) : offset( offset ) {
	misFITS_CHECK_CFITSIO_EXPR
	    ( fits_get_colnum( file.fptr(), 0, const_cast<char*>(ttype.c_str()), &colnum, &status )
	      );
	init( file );
    }

    ColumnInfo::ColumnInfo( const File& file, int colnum, LONGLONG offset ) : offset( offset ), colnum( colnum ) {
	init( file );
    }

    ColumnInfo::ColumnInfo( const std::string& type, ColumnType column_type_, const std::string& unit,
			    const Extent& extent_, int colnum_ ) :
	ttype( type ), tunit( unit), column_type( column_type_), extent( extent_ ), colnum( colnum_ ) {

	long width;

	// width depends upon field type; this seems to be the only
	// means to get it from CFITSIO. note that for strings, width
	// will be the number of charactes in a string, not the width
	// of the storage type
	{
	    char tform_t[2];
	    tform_t[0] = ColumnCode::code[column_type][0];
	    tform_t[1] = '\0';

	    misFITS_CHECK_CFITSIO_EXPR
		( fits_binary_tformll( tform_t, NULL, NULL, &width, &status )
		  );
	}

	LONGLONG nelem = extent.nelem();

	switch( boost::native_value(column_type) ) {

	case ColumnType::Bit:
	    // Bit fields are special.  The repeat count is the number
	    // of bits, not the number of bytes used to encode
	    // the bits.
	    nbytes = nelem / 8;
	    if ( nbytes * 8  < nelem ) nbytes += 1;
	    break;

	case ColumnType::String:
	    // strings are special.  FITS treats them as
	    // multi-dimensionalcharacter arrays, where TDIM[0] is
	    // the base number of characters in each "string".
	    nbytes = nelem;
	    break;

	default:
	    nbytes = nelem * width;
	}

    }

    void
    ColumnInfo::insert( const misFITS::File& file ) {

	// TODO: test if this is really necessary
	if ( OpenMode::ReadOnly == file.mode )
	    throw Exception::CFITSIO( READONLY_FILE );

	ostringstream tform;
	if ( extent.nelem() > 1 )
	    tform << extent.nelem();

	tform << ColumnCode::code[column_type];

	misFITS_CHECK_CFITSIO_EXPR
	    (
	     fits_insert_col(file.fptr(),
			     colnum,
			     const_cast<char*>(ttype.c_str()),
			     const_cast<char*>(tform.str().c_str()),
			     &status)
	     );

	if ( ColumnType::String == column_type ) {

	    misFITS_CHECK_CFITSIO_EXPR
		(
		 fits_write_tdimll( file.fptr(),
				    colnum,
				    extent.naxes(),
				    const_cast<LONGLONG*>(&(extent[0])),
				    &status )
		 );

	}
	else {
	    misFITS_CHECK_CFITSIO_EXPR
		(
		 fits_write_tdimll( file.fptr(),
				    colnum,
				    extent.naxes(),
				    const_cast<LONGLONG*>(&(extent[0])),
				    &status )
		 );

	}

    }
}
