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
#include <sstream>

#include <misfits/config.hpp>
#include <boost/filesystem.hpp>
#include <fitsio.h>

#include "util.hpp"
#include "fiducial_data.hpp"

using namespace std;

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
GenFits::SetUp() {
    gen_fits( TEST_FITS_QFILENAME );
}

void
GenFits::TearDown() {
    boost::filesystem::remove( TEST_FITS_QFILENAME);
}


namespace misFITS_Test {

    void
    closeTestFitsPtr( fitsfile* fitsptr ) {
	misFITS_CHECK_CFITSIO_EXPR( fits_close_file( fitsptr, &status ) );
    }


    TestFitsPtr
    createFitsFile( const std::string& filename ) {

	fitsfile* fp;
	std::string wfile( filename );
	wfile.insert( 0, "!" );

	misFITS_CHECK_CFITSIO_EXPR( fits_create_file( &fp, const_cast<char*>(wfile.c_str()), &status) );


	return TestFitsPtr( fp, closeTestFitsPtr );
    }


    void
    createFitsTable( TestFitsPtr& fpp, const std::string& name, int extver) {

	fitsfile* fp = fpp.get();

	misFITS_CHECK_CFITSIO_EXPR
	    (
	     fits_create_tbl( fp, BINARY_TBL, 0, 0,
			      NULL,
			      NULL,
			      NULL,
			      "stuff",
			      &status );


	     double D = 3.14159;
	     const char *Ds = "3.14159";
	     fits_write_key( fp, TDOUBLE, "PIE", &D, "DOUBLE",    &status );
	     fits_write_key( fp, TSTRING, "PIESTR", const_cast<char*>(Ds), "STRING", &status );

	     fits_write_key( fp, TINT, "EXTVER", &extver, "",  &status );
	     );
    }



    void writeFidData( TestFitsPtr& fpp, const std::string& name, int extver ) {

	Fiducial::Data data;

	createFitsTable( fpp, name, extver );
	data.insert_columns( fpp );
	data.write( fpp );
    }

}

void
gen_fits ( const std::string& file ) {

    using namespace misFITS_Test;

    TestFitsPtr fpp( createFitsFile( file ) );

    writeFidData( fpp, "stuff" );


}
