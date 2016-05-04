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

	HDU_Type hdu_type = HDU_Type::BinaryTable;

	misFITS_CHECK_CFITSIO_EXPR( fits_create_tbl( file->fptr(),
						     boost::underlying_cast<int>( hdu_type ),
						     0, 0, NULL, NULL, NULL,
						     extname.c_str(), &status  ) );

	file->write_key( Keyword<int>( "EXTVER", extver ) );
	hdu_num = file->hdu_num();

	refresh();
    }

    void
    Table::refresh( ) {

	resetHDU chdu( *this );

	// make sure we're really at a table
	int type = boost::underlying_cast<int>( file->hdu_type() );
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
    Table::has_column( const std::string& templt ) const {
	int colnum;
	int status = 0;

	resetHDU chdu( *this );

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

	resetHDU chdu( *this );

	misFITS_CHECK_CFITSIO_EXPR
	    ( fits_get_colnum( file->fptr(), 0, const_cast<char*>(colname.c_str()), &colnum, &status )
	      );
	return columns.at(colnum - 1);
    }

    Table&
    Table::add( const ColumnInfo& ci) {

	ColumnInfo copy = ci;

	resetHDU chdu( *this );

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

	resetHDU chdu( *this );

	if ( colnum == 0 )
	    colnum = num_columns() + 1;

	ColumnInfo( ttype, column_type, tunit, extent, colnum ).insert( *file.get() );

	refresh();

	return *this;
    }

    void
    Table::resize( int colnum, const Extent& extent ) {

	resetHDU chdu( *this );

	ColumnInfo info = columns.at(colnum - 1);

	// do nothing if the extent hasn't changed
	if ( info.extent == extent )
	    return;

	misFITS_CHECK_CFITSIO_EXPR
	    (
	     fits_modify_vector_len( file->fptr(), colnum, extent.nelem(), &status )
	     );

	// CFITSIO fixes up everything but TDIM
	misFITS_CHECK_CFITSIO_EXPR
	    (
	     ostringstream tdim;
	     tdim << "TDIM" << colnum ;
	     fits_modify_key_str( file->fptr(), tdim.str().c_str(), to_string( extent ).c_str() , "&", &status );
	     );

	refresh();
    }


    void
    Table::resize( const std::string& name , const Extent& extent ) {

	resize( colinfo(name).colnum, extent );

    }

    void
    Table::delete_column( int colnum ) {

	resetHDU chdu( *this );

	misFITS_CHECK_CFITSIO_EXPR
	    (
	     fits_delete_col( file->fptr(), colnum, &status )
	     );

	refresh();
    }

    void
    Table::delete_column( const std::string& name ) {

	resetHDU chdu( *this );

	misFITS_CHECK_CFITSIO_EXPR
	    (
	     fits_delete_col( file->fptr(), colinfo(name).colnum, &status )
	     );
    }

    int
    Table::num_columns( ) const {
	int num_cols;

	resetHDU chdu( *this );

	misFITS_CHECK_CFITSIO_EXPR
	    ( fits_get_num_cols( file->fptr(), &num_cols, &status )
	      );

	return num_cols;
    }


    //-----------------------------------------
    template<typename T>
    void Table::read_col( int colnum, LONGLONG firstrow, LONGLONG firstelem, LONGLONG nelem, T* data ) const {

    	misFITS_CHECK_CFITSIO_EXPR
    	    (
    	     fits_read_col( file->fptr(),
    			    StorageCode<T>::type,
    			    colnum,
    			    firstrow, firstelem,
    			    nelem,
    			    NULL,
    			    data, NULL, &status)
    	     );

    }

#define READ_COL(r,d,T) \
    template void Table::read_col<T>( int colnum, LONGLONG firstrow, LONGLONG firstelem, LONGLONG nelem_, T* data ) const;

    misFITS_INSTANTIATE_OVER_STORAGE_TYPES(READ_COL)

    //-----------------------------------------

    template<typename T>
    void Table::write_col( int colnum, LONGLONG firstrow, LONGLONG firstelem, LONGLONG nelem, const T* data ) const {

	misFITS_CHECK_CFITSIO_EXPR
	    (
	     fits_write_col( file->fptr(),
			     StorageCode<T>::type,
			     colnum,
			     firstrow, firstelem,
			     nelem,
			     const_cast<T*>(data), &status)
	     );

    }

#define WRITE_COL(r,d,T) \
    template void Table::write_col<T>( int colnum, LONGLONG firstrow, LONGLONG firstelem, LONGLONG nelem, const T* data ) const;

    misFITS_INSTANTIATE_OVER_STORAGE_TYPES(WRITE_COL)

    //-----------------------------------------

    template<>
    void Table::read_col<ColumnType::Logical>( int colnum, LONGLONG firstrow, LONGLONG firstelem, LONGLONG nelem, NativeType<SC_BYTE>::storage_type* data ) const {

    	misFITS_CHECK_CFITSIO_EXPR
    	    (
    	     fits_read_col( file->fptr(),
    			    static_cast<int>(ColumnType::Logical),
    			    colnum,
    			    firstrow, firstelem,
    			    nelem,
    			    NULL,
    			    data, NULL, &status)
    	     );

    }

    template<>
    void Table::write_col<ColumnType::Logical>( int colnum, LONGLONG firstrow, LONGLONG firstelem, LONGLONG nelem, const NativeType<SC_BYTE>::storage_type* data ) const {

	misFITS_CHECK_CFITSIO_EXPR
	    (
	     fits_write_col( file->fptr(),
			     static_cast<int>(ColumnType::Logical),
			     colnum,
			     firstrow, firstelem,
			     nelem,
			     const_cast<NativeType<SC_BYTE>::storage_type*>(data), &status)
	     );

    }


    //-----------------------------------------

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

	resetHDU chdu( *this );

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

	resetHDU chdu( *this );

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


