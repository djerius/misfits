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
	char typechar_t[80];

	misFITS_CHECK_CFITSIO_EXPR
	    ( fits_get_bcolparmsll( file.fptr(), colnum,
				    ttype_t, tunit_t, typechar_t,
				    NULL, NULL, NULL, NULL, NULL, &status )
	      );

	tunit = string( tunit_t );
	ttype = string( ttype_t );
	string typechar( typechar_t );

	int typecode;

	misFITS_CHECK_CFITSIO_EXPR
	    ( fits_get_coltypell( file.fptr(), colnum,
				  &typecode,
				  &repeat,
				  &width,
				  &status )
	      );

	column_type = static_cast<ColumnType>(typecode);

	// get cell extents
	{
	    int naxis;
	    misFITS_CHECK_CFITSIO_EXPR
		( fits_read_tdim( file.fptr(), colnum, 0, &naxis, NULL, &status )
		  );

	    extent.resize(naxis);
	    misFITS_CHECK_CFITSIO_EXPR
		( fits_read_tdimll( file.fptr(),
				    colnum,
				    naxis,
				    &naxis,
				    const_cast<LONGLONG*>(&(extent()[0])),
				    &status )
		  );
	}

	LONGLONG nelem_ = nelem();

	// turn repeat count in bits into 8bit bytes.
	switch ( typecode ) {

	case TSTRING:
	    // FIXME. this is confuuused.
	    nelem_ = 1;
	    width = 1;
	    break;
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
