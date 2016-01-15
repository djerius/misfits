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

#include "fits_p.hpp"

namespace misFITS {


    ///////////
    // Constructors //
    ///////////

    Table::Table( WeakFilePtr file, int hdu_num ) : HDU( file, hdu_num  ) {
	refresh();
    }

    Table::Table( WeakFilePtr file, const std::string& extname, int extver ) : HDU( file, extname, extver  ) {
	refresh();
    }

    Table::Table( const std::string& extname, int extver ) {

	misFITS_CHECK_CFITSIO_EXPR( fits_create_tbl( file->fptr(),
						     HDU_Type::BinaryTable,
						     0, 0, NULL, NULL, NULL,
						     extname.c_str(), &status  ) );

	file->write_key( Keyword<int>( "EXTVER", extver ) );
	hdu_num = file->hdu_num();

	refresh();
    }

    void
    Table::refresh( ) {

	resetHDU chdu( file, hdu_num );

	// make sure we're really at a table
	int type = boost::native_value( file->hdu_type() );
	if ( type != BINARY_TBL && type != ASCII_TBL )
	    throw Exception::Assert( "Expected CHDU to be a table, but it's not" );

	HDU::refresh();

	int ncols = num_columns();
	columns.clear();

	LONGLONG offset = 1;
	for ( int colnum = 1 ; colnum <= ncols ; colnum++ ) {
	    columns.push_back( ColumnInfo( *file.get(), colnum, offset ) );
	    offset += columns[colnum-1].nbytes;
	}
    }


    ///////////////////////////
    // Table Column Routines //
    ///////////////////////////


    bool
    Table::exists_column( const std::string& templt ) const {
	int colnum;
	int status = 0;

	resetHDU chdu( file, hdu_num );

	return ! fits_get_colnum( file->fptr(), CASEINSEN, const_cast<char*>(templt.c_str()),
					   &colnum, &status );
    }

    const ColumnInfo&
    Table::colinfo( int colnum ) {
	return columns.at(colnum - 1);
    }

    const ColumnInfo&
    Table::colinfo( const string& colname ) {
	int colnum;

	resetHDU chdu( file, hdu_num );

	misFITS_CHECK_CFITSIO_EXPR
	    ( fits_get_colnum( file->fptr(), 0, const_cast<char*>(colname.c_str()), &colnum, &status )
	      );
	return columns.at(colnum - 1);
    }

    Table&
    Table::add( const ColumnInfo& ci) {

	ColumnInfo copy = ci;

	resetHDU chdu( file );

	if ( copy.colnum == 0 )
	    copy.colnum = num_columns() + 1;

	copy.insert( *file.get() );

	refresh();

	return *this;
    }

    Table&
    Table::add( const std::string& ttype,
		ColumnType column_type,
		const std::string& tunit,
		const Extent& extent,
		int colnum ) {

	resetHDU chdu( file );

	if ( colnum == 0 )
	    colnum = num_columns() + 1;

	ColumnInfo( ttype, column_type, tunit, extent, colnum ).insert( *file.get() );

	refresh();

	return *this;
    }

    void
    Table::delete_column( int colnum ) {

	resetHDU chdu( file, hdu_num );

	misFITS_CHECK_CFITSIO_EXPR
	    (
	     fits_delete_col( file->fptr(), colnum, &status )
	     );

	refresh();
    }

    void
    Table::delete_column( const std::string& name ) {

	resetHDU chdu( file, hdu_num );

	misFITS_CHECK_CFITSIO_EXPR
	    (
	     fits_delete_col( file->fptr(), colinfo(name).colnum, &status )
	     );
    }

    int
    Table::num_columns( ) const {
	int num_cols;

	resetHDU chdu( file, hdu_num );

	misFITS_CHECK_CFITSIO_EXPR
	    ( fits_get_num_cols( file->fptr(), &num_cols, &status )
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

	resetHDU chdu( file, hdu_num );

	misFITS_CHECK_CFITSIO_EXPR
	    (
	     fits_get_num_rowsll( file->fptr(), &num_rows, &status )
	     );

	return num_rows;
    }

    ///////////////////////////
    // Table Header Routines //
    ///////////////////////////


    // copy table to other file
    TablePtr
    Table::copy( misFITS::FilePtr& ofile, const TableCopy& what, int morekeys ) const {

	resetHDU chdu( file, hdu_num );

	switch( boost::native_value( what ) ) {

	case TableCopy::Header :
	    file->copy( ofile, FileCopy::CurrentHeader );
	    break;

	case TableCopy::HDU :
	    file->copy( ofile, FileCopy::CurrentHDU, morekeys );
	    break;

	}


	return TablePtr( new Table( ofile ) );
    }



}


