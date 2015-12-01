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

#include <misfits/config.hpp>
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

#define NROWS 20
#define NBITS 23

    boost::filesystem::remove( boost::filesystem::path( TEST_FITS_QFILENAME ) );

    die_if( fits_create_file( &fp, "!" TEST_FITS_QFILENAME, &status),
	    TEST_FITS_QFILENAME
	    );

    // for now, just create the most often used ones
    const char * const ttype[] = { "Icol", "Jcol", "Ecol", "Dcol", "Xcol" };
    const char * const tform[] = { "I"   , "J"   , "E"   , "D"   , quote(NBITS) "X"    };
    const char * const tunit[] = { "Iunt", "Junt", "Eunt", "Dunt", "Xunt" };

#define TFIELDS sizeof(ttype) / sizeof(*ttype)

    die_if( fits_create_tbl( fp, BINARY_TBL, 0, TFIELDS,
			     const_cast<char**>( ttype ),
			     const_cast<char**>( tform ),
			     const_cast<char**>( tunit ),
			     "stuff",
			     &status ),
	    TEST_FITS_QFILENAME ": couldn't create binary table" );


    {
	double D = 3.14159;
	char *Ds = "3.14159";
	die_if(  fits_write_key( fp, TDOUBLE, "PIE", &D, "DOUBLE",  &status ), TEST_FITS_QFILENAME ": couldnt create PIE keyword" );
	die_if(  fits_write_key( fp, TSTRING, "PIESTR", Ds, "STRING",    &status ), TEST_FITS_QFILENAME ": couldnt create PIESTRING keyword"  );
    }

    {
	int extver = 1;
	die_if(  fits_write_key( fp, TINT, "EXTVER", &extver, "",  &status ), TEST_FITS_QFILENAME ": couldnt create EXTVER keyword" );
    }

    int nbytes = NBITS / 8;
    if ( nbytes * 8 < NBITS ) nbytes++;

    for ( int i = 1 ; i <= NROWS ; i++ ) {

	double D = 1.0 / i;
	float  E = 2.0 / i;
	short  I = i + 1;
	int    J = i + 2;
	vector<uint8_t> X(nbytes);

	X.assign(X.size(), 0 );

	// set the bit s.t. i == 1 => MSB, i == NROWS => LSB
	// this is left justified in the bit array, as specified
	// by the FITS standard.

	int byte = (i - 1) / 8;
	int shift = ( 8 - (i - byte * 8) );
	X[byte] |= 1<<shift;

	die_if( fits_write_col( fp, TSHORT,  1, i, 1, 1, &I, &status ),
		"writing column Dcol" );

	die_if( fits_write_col( fp, TINT,    2, i, 1, 1, &J, &status ),
		"writing column Dcol" );

	die_if( fits_write_col( fp, TFLOAT,  3, i, 1, 1, &E, &status ),
		"writing column Dcol" );

	die_if( fits_write_col( fp, TDOUBLE, 4, i, 1, 1, &D, &status ),
		"writing column Dcol" );

	die_if( fits_write_col( fp, TBYTE,   5, i, 1, nbytes, &X[0], &status ),
		"writing column Xcol" );
    }


    die_if( fits_close_file( fp, &status ),
	    TEST_FITS_QFILENAME ": couldn't close file" );

}
