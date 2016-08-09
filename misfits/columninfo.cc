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
#include "misfits/fits_p.hpp"
#include "misfits/table.hpp"

namespace misFITS {

    bool ColumnInfo::operator == (const ColumnInfo& col ) const {
	return
	    col.column_type->id()  == column_type->id() &&
	    col.extent       == extent;
    }

    void
    ColumnInfo::init( const File& file ) {

	char ttype_t[80];
	char tunit_t[80];

	if ( file.hdu_type() != HDU_Type::BinaryTable )
	    throw Exception::Assert( "only Binary Tables are supported" );

	misFITS_CHECK_CFITSIO_EXPR
	    ( fits_get_bcolparmsll( file.fptr(),
				    static_cast<int>( colnum ),
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
				  static_cast<int>( colnum ),
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
				  static_cast<int>( colnum ),
				  &typecode,
				  &repeat,
				  &twidth,
				  &status )
	      );
	width = twidth;
#endif // LONGLONG FUDGE

	column_type = ColumnType::spec_from_id(typecode);

	int naxis;
	misFITS_CHECK_CFITSIO_EXPR
	    ( fits_read_tdimll( file.fptr(), static_cast<int>( colnum ), 0, &naxis, NULL, &status )
	      );

	// handle CFITSIO idiosyncracies with 'A' columns. see tests/cfitsio.cc
	if ( ColumnType::ID::String == column_type->id() && naxis == 1 ) {

	    extent.resize(2);
	    extent[0] = width;

	    // N.B. CFITSIO sets width = repeat if no width is specified
	    extent[1] = repeat / width ;
	}
	else {

	    extent.resize( static_cast<ExtentType::size_type>( naxis) );
	    misFITS_CHECK_CFITSIO_EXPR
		( fits_read_tdimll( file.fptr(),
				    static_cast<int>( colnum ),
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
	if ( ColumnType::ID::Bit == column_type->id() ) {
	    nbytes = nelem_ / 8;
	    if ( nbytes * 8  < nelem_ ) nbytes += 1;
	}
	else {
	    nbytes = nelem_ * ( ColumnType::ID::String == column_type->id() ? 1 : width );
	}
    }



    ColumnInfo::ColumnInfo( const File& file, const std::string& ttype, LONGLONG offset_ ) : offset( offset_ ) {
	misFITS_CHECK_CFITSIO_EXPR
	    ( 
	     int colnum_;
	     fits_get_colnum( file.fptr(), 0, const_cast<char*>(ttype.c_str()), &colnum_, &status );
	     if ( ! status )
		 colnum = static_cast<TableColumnsType::size_type>( colnum_ );
	      );
	init( file );
    }

    ColumnInfo::ColumnInfo( const File& file, TableColumnsType::size_type colnum_, LONGLONG offset_ ) : offset( offset_ ), colnum( colnum_ ) {
	init( file );
    }

    ColumnInfo::ColumnInfo( const std::string& type, ColumnType::ID::type column_type_, const std::string& unit,
			    const Extent& extent_, TableColumnsType::size_type colnum_ ) :
	ttype( type ), tunit( unit), column_type( ColumnType::spec_from_id( column_type_ ) ), extent( extent_ ), colnum( colnum_ ) {

	long width;

	// width depends upon field type; this seems to be the only
	// means to get it from CFITSIO. note that for strings, width
	// will be the number of charactes in a string, not the width
	// of the storage type
	{
	    char tform_t[2];
	    tform_t[0] = column_type->code();
	    tform_t[1] = '\0';

	    misFITS_CHECK_CFITSIO_EXPR
		( fits_binary_tformll( tform_t, NULL, NULL, &width, &status )
		  );
	}

	LONGLONG nelem = extent.nelem();

	switch( column_type->id() ) {

	case ColumnType::ID::Bit:
	    // Bit fields are special.  The repeat count is the number
	    // of bits, not the number of bytes used to encode
	    // the bits.
	    nbytes = nelem / 8;
	    if ( nbytes * 8  < nelem ) nbytes += 1;
	    break;

	case ColumnType::ID::String:
	    // strings are special.  FITS treats them as
	    // multi-dimensionalcharacter arrays, where TDIM[0] is
	    // the base number of characters in each "string".
	    nbytes = nelem;
	    break;

	default:
	    nbytes = nelem * width;
	}

    }

    std::string
    ColumnInfo::tform() const {

	ostringstream tform;
	if ( extent.nelem() > 1 )
	    tform << extent.nelem();

	tform << column_type->code();

	return tform.str();
    }

    void
    ColumnInfo::insert( const misFITS::File& file ) {

	// TODO: test if this is really necessary
	if ( OpenMode::ReadOnly == file.mode )
	    throw Exception::CFITSIO( READONLY_FILE );

	std::string tform_str = tform();

	misFITS_CHECK_CFITSIO_EXPR
	    (
	     fits_insert_col(file.fptr(),
			     static_cast<int>( colnum ),
			     const_cast<char*>(ttype.c_str()),
			     const_cast<char*>(tform_str.c_str()),
			     &status)
	     );

	// if there's only a single dimension, don't write out a TDIM
	// keyword, as CIAO can't handle that for bitstrings (and it's
	// redundant, anyway)
	if ( extent.squeeze().naxes() > 1 )
	    misFITS_CHECK_CFITSIO_EXPR
		(
		 fits_write_tdimll( file.fptr(),
				    static_cast<int>( colnum ),
				    static_cast<int>( extent.naxes() ),
				    const_cast<LONGLONG*>(&(extent[0])),
				    &status )
		 );

    }
}

