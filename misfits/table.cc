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

	misFITS_CHECK_CFITSIO_EXPR( fits_create_tbl( file_->fptr(),
						     boost::underlying_cast<int>( hdu_type ),
						     0, 0, NULL, NULL, NULL,
						     extname.c_str(), &status  ) );

	hdu_num_ = file_->hdu_num();

	set_keyword( Keyword<int>( "EXTVER", extver ) );

	refresh();
    }

    void
    Table::refresh( ) {

	set_as_chdu();

	// make sure we're really at a table
	int type = boost::underlying_cast<int>( file_->hdu_type() );
	if ( type != BINARY_TBL && type != ASCII_TBL )
	    throw Exception::Assert( "Expected CHDU to be a table, but it's not" );

	HDU::refresh();

	Columns::size_type ncols = num_columns();
	columns.clear();

	LONGLONG offset = 1;
	for ( Columns::size_type colnum = 1 ; colnum <= ncols ; colnum++ ) {
	    columns.push_back( ColumnInfo( *file_.get(), colnum, offset ) );
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

	set_as_chdu();

	return ! fits_get_colnum( file_->fptr(), CASEINSEN, const_cast<char*>(templt.c_str()),
					   &colnum, &status );
    }

    const ColumnInfo&
    Table::colinfo(  Columns::size_type colnum ) {
	return columns.at(colnum - 1);
    }

    const ColumnInfo&
    Table::colinfo( const string& colname ) {
	int colnum;

	set_as_chdu();

	misFITS_CHECK_CFITSIO_EXPR
	    ( fits_get_colnum( file_->fptr(), 0, const_cast<char*>(colname.c_str()), &colnum, &status )
	      );
	return columns.at( static_cast<Columns::size_type>( colnum ) - 1);
    }

    Table&
    Table::add( const ColumnInfo& ci) {

	ColumnInfo copy = ci;

	set_as_chdu();

	if ( copy.colnum == 0 )
	    copy.colnum = num_columns() + 1;

	copy.insert( *file_.get() );

	refresh();

	return *this;
    }

    Table&
    Table::add( const std::string& ttype,
		ColumnType column_type,
		const std::string& tunit,
		const Extent& extent,
		Columns::size_type colnum ) {

	set_as_chdu();

	if ( colnum == 0 )
	    colnum = num_columns() + 1;

	ColumnInfo( ttype, column_type, tunit, extent, colnum ).insert( *file_.get() );

	refresh();

	return *this;
    }

    void
    Table::resize( Columns::size_type colnum, const Extent& extent ) {

	set_as_chdu();

	ColumnInfo info = columns.at(colnum - 1);

	// do nothing if the extent hasn't changed
	if ( info.extent == extent )
	    return;

	misFITS_CHECK_CFITSIO_EXPR
	    (
	     fits_modify_vector_len( file_->fptr(), static_cast<int>( colnum ),
				     extent.nelem(), &status )
	     );

	// CFITSIO fixes up everything but TDIM
	misFITS_CHECK_CFITSIO_EXPR
	    (
	     ostringstream tdim;
	     tdim << "TDIM" << colnum ;
	     fits_modify_key_str( file_->fptr(), tdim.str().c_str(), to_string( extent ).c_str() , "&", &status );
	     );

	refresh();
    }


    void
    Table::resize( const std::string& name , const Extent& extent ) {

	resize( colinfo(name).colnum, extent );

    }

    void
    Table::delete_column( Columns::size_type colnum ) {

	set_as_chdu();

	misFITS_CHECK_CFITSIO_EXPR
	    (
	     fits_delete_col( file_->fptr(), static_cast<int>( colnum ), &status )
	     );

	refresh();
    }

    void
    Table::delete_column( const std::string& name ) {

	set_as_chdu();

	misFITS_CHECK_CFITSIO_EXPR
	    (
	     fits_delete_col( file_->fptr(), static_cast<int>( colinfo(name).colnum ), &status )
	     );
    }

    Table::Columns::size_type
    Table::num_columns( ) const {
	int num_cols;

	set_as_chdu();

	misFITS_CHECK_CFITSIO_EXPR
	    ( fits_get_num_cols( file_->fptr(), &num_cols, &status )
	      );

	return static_cast<Columns::size_type>( num_cols );
    }

    // copy a column to another table
    void
    Table::copy_column( Table& dest, const std::string& name, ColumnCopy::Flag how ) {

	vector<std::string> names;
	names.push_back( name );

	copy_columns( dest, names, how );
    }


    // copy columns to another table
    void
    Table::copy_columns( Table& dest, const std::vector<std::string>& names, ColumnCopy::Flag how ) {

	// do nothing if nothing is to be done. later logic assumes
	// names is not empty
	if ( names.empty() )
	    return;

	set_as_chdu();
	dest.set_as_chdu();

	// find out which columns need to be created in the
	// destination. create them all at once for efficiency
	std::vector<ColumnInfo> src_ci;
	std::vector<std::string> dest_ttype;
	std::vector<std::string> dest_tform;

	for( std::vector<std::string>::size_type idx = 0 ; idx < names.size() ; ++idx ) {

	    const std::string& name = names[idx];

	    if ( ! has_column( name ) )
		throw Exception::Assert( std::string("request to copy a non-existent column: ") + name );

	    ColumnInfo ci = colinfo( name );
	    src_ci.push_back( ci );

	    if ( ! dest.has_column( name ) ) {
		dest_ttype.push_back( src_ci.back().ttype );
		dest_tform.push_back( src_ci.back().tform() );
	    }
	    else {

		switch ( how & ColumnCopy::FlagMask ) {

		case ColumnCopy::NoDuplicates :
		    throw Exception::Assert( std::string("destination table already has a column named '") + name + "'" );
		    break;

		case ColumnCopy::Replace :
		    dest.delete_column( name );
		    dest_ttype.push_back( src_ci.back().ttype );
		    dest_tform.push_back( src_ci.back().tform() );
		    break;

		case ColumnCopy::OverWrite :
		    // nothing to do
		    break;

		default:
		    throw Exception::Assert( "uknown misFITS::ColumnCopy flag" );

		}
	    }
	}

	// insert the new (or replaced) columns
	if ( ! dest_ttype.empty() ) {

	    std::vector<const char*> dest_ttype_ptr;
	    std::vector<const char*> dest_tform_ptr;

	    for( std::vector<string>::size_type idx = 0 ; idx < dest_ttype.size() ; ++idx ) {
		dest_ttype_ptr.push_back( dest_ttype[idx].c_str() );
		dest_tform_ptr.push_back( dest_tform[idx].c_str() );
	    }

	    misFITS_CHECK_CFITSIO_EXPR
		( fits_insert_cols( dest.file_->fptr(),
				    static_cast<int>(dest.num_columns() + 1),
				    static_cast<int>( dest_ttype.size() ),
				    const_cast<char**>( &dest_ttype_ptr[0] ),
				    const_cast<char**>( &dest_tform_ptr[0] ),
				    &status )
		  );
	}

	// if not enough rows, must get CFITSIO to allocate space for
	// them.  This should only be done if there is actually something to copy;
	// that is checked above at the entry to this method.
	if ( num_rows() > dest.num_rows()
	     && (    ( how & ColumnCopy::ExtendTable )
		  || ( how & ColumnCopy::ExtendTableIfEmpty && dest.num_rows() == 0 )
		)
	   ) {

	    misFITS_CHECK_CFITSIO_EXPR
		( fits_insert_rows( dest.file_->fptr(),
				    dest.num_rows(),
				    num_rows() - dest.num_rows(),
				    &status )
		  );

	}
	// make sure we know where everything is
	dest.refresh();

	for( std::vector<ColumnInfo>::size_type idx = 0 ; idx < src_ci.size() ; ++idx ) {

	    misFITS_CHECK_CFITSIO_EXPR
		( fits_copy_col( file_->fptr(), dest.file_->fptr(),
				 static_cast<int>( src_ci[idx].colnum ),
				 static_cast<int>( dest.colinfo( src_ci[idx].ttype).colnum ),
				 0,
			     &status )
	      );

	}

    }

    //-----------------------------------------

    void
    Table::read_bytes( LONGLONG firstrow, LONGLONG offset, LONGLONG nbytes, unsigned char* data ) const {

	misFITS_CHECK_CFITSIO_EXPR
	    (
	     fits_read_tblbytes( file_->fptr(), firstrow, offset,
				 nbytes,
				 data,
				 &status )
	     );
    }

    void
    Table::write_bytes( LONGLONG firstrow, LONGLONG offset, LONGLONG nbytes, unsigned char* data ) const {

	misFITS_CHECK_CFITSIO_EXPR
	    (
	     fits_write_tblbytes( file_->fptr(), firstrow, offset,
				  nbytes,
				  data,
				  &status )
	     );
    }


    //-----------------------------------------

    template<typename T>
    void Table::read_col( Columns::size_type colnum, LONGLONG firstrow, LONGLONG firstelem, LONGLONG nelem, T* data ) const {

    	misFITS_CHECK_CFITSIO_EXPR
    	    (
    	     fits_read_col( file_->fptr(),
    			    StorageCode<T>::type,
    			    static_cast<int>( colnum ),
    			    firstrow, firstelem,
    			    nelem,
    			    NULL,
    			    data, NULL, &status)
    	     );

    }

#define READ_COL(r,d,T) \
    template void Table::read_col<T>( Columns::size_type colnum, LONGLONG firstrow, LONGLONG firstelem, LONGLONG nelem_, T* data ) const;

    misFITS_INSTANTIATE_OVER_STORAGE_TYPES(READ_COL)

    //-----------------------------------------

    template<typename T>
    void Table::write_col( Columns::size_type colnum, LONGLONG firstrow, LONGLONG firstelem, LONGLONG nelem, const T* data ) const {

	misFITS_CHECK_CFITSIO_EXPR
	    (
	     fits_write_col( file_->fptr(),
			     StorageCode<T>::type,
			     static_cast<int>( colnum ),
			     firstrow, firstelem,
			     nelem,
			     const_cast<T*>(data), &status)
	     );

    }

#define WRITE_COL(r,d,T) \
    template void Table::write_col<T>( Columns::size_type colnum, LONGLONG firstrow, LONGLONG firstelem, LONGLONG nelem, const T* data ) const;

    misFITS_INSTANTIATE_OVER_STORAGE_TYPES(WRITE_COL)

    //-----------------------------------------

    template<>
    void Table::read_col<ColumnType::Logical>( Columns::size_type colnum, LONGLONG firstrow, LONGLONG firstelem, LONGLONG nelem, NativeType<SC_BYTE>::storage_type* data ) const {

    	misFITS_CHECK_CFITSIO_EXPR
    	    (
    	     fits_read_col( file_->fptr(),
    			    static_cast<int>(ColumnType::Logical),
    			    static_cast<int>( colnum ),
    			    firstrow, firstelem,
    			    nelem,
    			    NULL,
    			    data, NULL, &status)
    	     );

    }

    template<>
    void Table::write_col<ColumnType::Logical>( Columns::size_type colnum, LONGLONG firstrow, LONGLONG firstelem, LONGLONG nelem, const NativeType<SC_BYTE>::storage_type* data ) const {

	misFITS_CHECK_CFITSIO_EXPR
	    (
	     fits_write_col( file_->fptr(),
			     static_cast<int>(ColumnType::Logical),
			     static_cast<int>( colnum ),
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

	set_as_chdu();

	misFITS_CHECK_CFITSIO_EXPR
	    (
	     fits_get_num_rowsll( file_->fptr(), &num_rows, &status )
	     );

	return num_rows;
    }

    ///////////////////////////
    // Table Header Routines //
    ///////////////////////////


    // copy table to other file
    TablePtr
    Table::copy( misFITS::FilePtr& ofile, const TableCopy& what, int morekeys ) const {

	set_as_chdu();

	switch( boost::native_value( what ) ) {

	case TableCopy::Header :
	    file_->copy( ofile, FileCopy::CurrentHeader );
	    break;

	case TableCopy::HDU :
	    file_->copy( ofile, FileCopy::CurrentHDU, morekeys );
	    break;

	}


	return TablePtr( new Table( ofile ) );
    }



}


