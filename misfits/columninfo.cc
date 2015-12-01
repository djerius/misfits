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

using namespace std;

#include <cfitsio/fitsio.h>

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

	// handle CFITSIO idiosyncracies with 'A' columns
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


	if ( TSTRING == column_type ) {
	    width = extent[0];
	    extent.erase(extent.begin());
	}
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



}
