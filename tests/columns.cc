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
#include <sstream>

#include "columns.hpp"

using namespace std;

namespace misFITS_Test {


  void
  Columns::add( const char* type,
		const char* form,
		const char *unit,
		const char *dim
		) {

    ttype.push_back( type );
    tform.push_back( form );
    tunit.push_back( unit );
    tdim.push_back( dim );
  }

  TestFitsPtr
  Columns::create( std::string filename ) {

    fitsfile *fp;

    filename.insert( 0, "!" );

    misFITS_CHECK_CFITSIO_EXPR( fits_create_file( &fp, const_cast<char*>(filename.c_str()), &status) );

    vector<char*> type_t;
    vector<char*> form_t;
    vector<char*> unit_t;

    for( int col = 0 ; col < ttype.size() ; ++col ) {
      type_t.push_back( const_cast<char*>( ttype[col].c_str() ) );
      form_t.push_back( const_cast<char*>( tform[col].c_str() ) );
      unit_t.push_back( const_cast<char*>( tunit[col].c_str() ) );
    }

    misFITS_CHECK_CFITSIO_EXPR
      (
       fits_create_tbl( fp, BINARY_TBL, 0, ttype.size(),
			&type_t[0],
			&form_t[0],
			&unit_t[0],
			"stuff",
			&status )
       );

    for ( int col = 0 ; col < ttype.size() ; ++ col ) {
      if ( ! tdim[col].empty() )
	misFITS_CHECK_CFITSIO_EXPR
	  (
	   char keyname[8];
	   sprintf( keyname, "TDIM%d", col+1 );
	   fits_write_key( fp, TSTRING, keyname, const_cast<char*>(tdim[col].c_str()), NULL, &status );
	   );
    }

    return TestFitsPtr( fp, closeTestFitsPtr );
  }


  ColInfo::ColInfo( const TestFitsPtr& fpp, int colnum ) {

    fitsfile* fp = fpp.get();

    {
      int status = 0;
      char keyname[8];
      sprintf( keyname, "TDIM%d", colnum );

      tdim[0] = '\0';
      fits_read_key_str(fp, keyname, tdim, NULL, &status );

      if ( status && status != KEY_NO_EXIST )
	throw misFITS::Exception::CFITSIO (status );

    }

    misFITS_CHECK_CFITSIO_EXPR
      (
       fits_get_bcolparmsll( fp, colnum,
			     ttype,
			     tunit,
			     typechar,
			     &repeat,
			     &scale,
			     &zero,
			     &nulval,
			     tdisp,
			     &status )
       );


    misFITS_CHECK_CFITSIO_EXPR
      (
       fits_get_coltypell( fp, colnum,
			   &typecode,
			   &repeat,
			   &width,
			   &status )
       );



    misFITS_CHECK_CFITSIO_EXPR
      (
       fits_read_tdimll( fp, colnum,
			 10,
			 &naxis,
			 naxes,
			 &status )
       );

  }

}
