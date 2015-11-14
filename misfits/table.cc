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
#include <memory>
#include <cstdarg>
#include <numeric>

using namespace std;


#include <misfits/types.hpp>
#include <misfits/fits.hpp>
#include <misfits/table.hpp>
#include <misfits/row.hpp>

namespace misFITS {


    ///////////
    // Constructors //
    ///////////

    Table::Table( WeakFilePtr file ) : HDU( file ) {
	refresh();
    }

    Table::Table( File& file ) : HDU( file ) {
	refresh();
    }

    Table::Table( const std::string& extname, int extver ) {

	misFITS_CHECK_CFITSIO_EXPR( fits_create_tbl( file->fptr(),
						     BinaryTable,
						     0, 0, NULL, NULL, NULL,
						     extname.c_str(), &status  ) );

	file->write_key( Keyword<int>( "EXTVER", extver ) );
	hdu_num = file->hdu_num();

	refresh();
    }


    void
    Table::refresh( ) {

	file()->movabs_hdu( hdu_num );

	// make sure we're really at a table
	int type = file()->hdu_type();
	if ( type != BINARY_TBL && type != ASCII_TBL )
	    throw Exception::Assert( "Expected CHDU to be a table, but it's not" );

	HDU::refresh();

	int ncols = num_columns();
	columns.clear();
	for ( int colnum = 1 ; colnum <= ncols ; colnum++ )
	    columns.push_back( ColumnInfo( *file(), colnum ) );
    }


    ///////////////////////////
    // Table Column Routines //
    ///////////////////////////


    bool
    Table::exists_column( const std::string& templt ) const {
	int colnum;
	int status = 0;
	fitsfile* fptr = file()->fptr();

	return ! fits_get_colnum( fptr, CASEINSEN, const_cast<char*>(templt.c_str()),
				  &colnum, &status );
    }

    const ColumnInfo&
    Table::colinfo( int colnum ) {
	return columns.at(colnum - 1);
    }

    const ColumnInfo&
    Table::colinfo( const string& colname ) {
	int colnum;
	misFITS_CHECK_CFITSIO_EXPR
	    ( fits_get_colnum( file()->fptr(), 0, const_cast<char*>(colname.c_str()), &colnum, &status )
	      );
	return columns.at(colnum - 1);
    }

    Table&
    Table::add( const std::string& ttype,
		ColumnType typecode,
		const Extent& extent,
		int colnum ) {

	LONGLONG repeat;
	LONGLONG width = 1;

	LONGLONG nelem = extent.nelem();

	switch( typecode ) {

	case CT_BIT:
	    repeat = nelem * 8;
	    break;

	case CT_STRING:
	    repeat = 1;
	    width = 1;
	    break;

	default:
	    repeat = nelem;
	}

	ostringstream tform;
	if ( repeat > 1 )
	    tform << repeat;
	tform << ColumnCode::code[typecode];

	if ( width > 1 )
	    tform << width ;

	if ( colnum == 0 )
	    colnum = num_columns() + 1;

	misFITS_CHECK_CFITSIO_EXPR
	    (
	     fits_insert_col(file()->fptr(),
			     colnum,
			     const_cast<char*>(ttype.c_str()),
			     const_cast<char*>(tform.str().c_str()),
			     &status)
	     );

	LONGLONG* extentp = const_cast<LONGLONG*>(&(extent()[0]));
	misFITS_CHECK_CFITSIO_EXPR(
	fits_write_tdimll( file()->fptr(),
			   colnum,
			   extent.naxes(),
			   extentp,
			   &status )
	    );

	refresh();

	return *this;
    }

    void
    Table::delete_column( int colnum ) {

	misFITS_CHECK_CFITSIO_EXPR
	    (
	     fits_delete_col( file()->fptr(), colnum, &status )
	     );

	refresh();
    }

    void
    Table::delete_column( const std::string& name ) {

	misFITS_CHECK_CFITSIO_EXPR
	    (
	     fits_delete_col( file()->fptr(), colinfo(name).colnum, &status )
	     );
    }

    int
    Table::num_columns( ) const {
	int num_cols;
	misFITS_CHECK_CFITSIO_EXPR
	    ( fits_get_num_cols( file()->fptr(), &num_cols, &status )
	      );
	return num_cols;
    }

    ////////////////////////
    // Table Row Routines //
    ////////////////////////

    misFITS::Row
    Table::row() {

	return misFITS::Row( new own_or_observe::observed_ptr<Table>( this ) ) ;
    }

    LONGLONG
    Table::num_rows() const {

	LONGLONG num_rows;
	misFITS_CHECK_CFITSIO_EXPR
	    (
	     fits_get_num_rowsll( file()->fptr(), &num_rows, &status )
	     );

	return num_rows;
    }


    ///////////////////////////
    // Table Header Routines //
    ///////////////////////////


    // copy table header to other file
    TablePtr
    Table::copy_header( misFITS::FilePtr& ofile ) const {

	// FIXME: this should be undone with a scope guard.
	int current_hdu = file()->hdu_num();

	try {
	    file()->movabs_hdu( hdu_num );
	    file()->copy_header( ofile );
	}
	catch ( ... ) {
	    file()->movabs_hdu( current_hdu );
	}

	file()->movabs_hdu( current_hdu );
	    return TablePtr( new Table( ofile ) );
    }

    // copy table to other file
    TablePtr
    Table::copy( misFITS::FilePtr& ofile, int morekeys ) const {

	// FIXME: this should be undone with a scope guard.
	int current_hdu = file()->hdu_num();

	try {
	    file()->movabs_hdu( hdu_num );
	    file()->copy_hdu( ofile, morekeys );
	}
	catch (...) {
	    file()->movabs_hdu( current_hdu );
	    throw;
	}

	file()->movabs_hdu( current_hdu );
	return TablePtr( new Table( ofile ) );
    }



}


