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

#include <string>
#include <cstdlib>

#include <iostream>
#include <limits.h>

#include <boost/filesystem.hpp>

#include <cfitsio/fitsio.h>

using namespace std;

#include "util.hpp"

void
die_if( int status, const char* prefix ) {

    static char cptr[40];

    if ( 0 == status )
	return;

    fits_get_errstatus( status, cptr );

    cerr << prefix << ": " << cptr << endl;
    exit(EXIT_FAILURE);
}

void
gen_fits ( ) {

    int status = 0;
    fitsfile *fp;

    boost::filesystem::remove( boost::filesystem::path( TEST_FITS_QFILENAME ) );

    die_if( fits_create_file( &fp, "!" TEST_FITS_QFILENAME, &status),
	    TEST_FITS_QFILENAME
	    );

    // for now, just create the most often used ones
    const char * const ttype[] = { "Icol", "Jcol", "Ecol", "Dcol" };
    const char * const tform[] = { "I"   , "J"   , "E"   , "D"    };
    const char * const tunit[] = { "Iunt", "Junt", "Eunt", "Dunt" };

#define TFIELDS sizeof(ttype) / sizeof(*ttype)

    die_if( fits_create_tbl( fp, BINARY_TBL, 0, TFIELDS,
			     const_cast<char**>(ttype ),
			     const_cast<char**>( tform ),
			     const_cast<char**>( tunit ),
			     "stuff",
			     &status ),
	    TEST_FITS_QFILENAME ": couldn't create binary table" );


    for ( int i = 1 ; i <= 20 ; i++ ) {

	double D = 1.0 / i;
	float  E = 2.0 / i;
	short  I = i + 1;
	int    J = i + 2;

	die_if( fits_write_col( fp, TSHORT,  1, i, 1, 1, &I, &status ),
		"writing column Dcol" );

	die_if( fits_write_col( fp, TINT,    2, i, 1, 1, &J, &status ),
		"writing column Dcol" );

	die_if( fits_write_col( fp, TFLOAT,  3, i, 1, 1, &E, &status ),
		"writing column Dcol" );

	die_if( fits_write_col( fp, TDOUBLE, 4, i, 1, 1, &D, &status ),
		"writing column Dcol" );
    }

    die_if( fits_close_file( fp, &status ),
	    TEST_FITS_QFILENAME ": couldn't close file" );

}
