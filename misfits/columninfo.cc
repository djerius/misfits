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
	    col.ttype  == ttype &&
	    col.extent == col.extent;
    }

    void
    ColumnInfo::init( const File& file ) {

	char ttype_t[80];
	char tunit_t[80];

	if ( file.hdu_type() != BinaryTable )
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
	if ( TSTRING == column_type && naxis == 1 ) {

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


	if ( TSTRING == column_type )
	    width = 1;


	nelem_ = extent.nelem();

	// the column width for TBIT is reported as 1 (byte), which is
	// not useful in calculating the true number of bytes in the
	// column
	if ( TBIT == column_type ) {
	    nbytes = nelem_ / 8;
	    if ( nbytes * 8  < nelem_ ) nbytes += 1;
	}
	else {
	    nbytes = nelem_ * width;
	}
    }


    ColumnInfo::ColumnInfo( const File& file, const std::string& ttype ) {
	misFITS_CHECK_CFITSIO_EXPR
	    ( fits_get_colnum( file.fptr(), 0, const_cast<char*>(ttype.c_str()), &colnum, &status )
	      );
	init( file );
    }

    ColumnInfo::ColumnInfo( const File& file, int colnum ) : colnum( colnum ) {
	init( file );
    }

    ColumnInfo::ColumnInfo( const std::string& type, ColumnType column_type_, const std::string& unit,
			    const Extent& extent_, int colnum_ ) :
	ttype( type ), tunit( unit), column_type( column_type_), extent( extent_ ), colnum( colnum_ ) {

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

	repeat = nelem_ = extent.nelem();

	switch( column_type ) {

	case CT_BIT:
	    // Bit fields are special.  The repeat count is the number
	    // of bits, not the number of bytes used to encode
	    // the bits.
	    nbytes = nelem_ / 8;
	    if ( nbytes * 8  < nelem_ ) nbytes += 1;
	    break;

	case CT_STRING:
	    // strings are special.  FITS treats them as
	    // multi-dimensionalcharacter arrays, where TDIM[0] is
	    // the base number of characters in each "string".

	    width = 1;
	    nbytes = repeat;
	    nelem_ = extent.nelem() / width;
	    break;

	default:
	    nbytes = nelem_ * width;
	}

    }

    void
    ColumnInfo::insert( const misFITS::File& file ) {

	ostringstream tform;
	if ( repeat > 1 )
	    tform << repeat;

	tform << ColumnCode::code[column_type];

	misFITS_CHECK_CFITSIO_EXPR
	    (
	     fits_insert_col(file.fptr(),
			     colnum,
			     const_cast<char*>(ttype.c_str()),
			     const_cast<char*>(tform.str().c_str()),
			     &status)
	     );

	if ( CT_STRING == column_type ) {

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
