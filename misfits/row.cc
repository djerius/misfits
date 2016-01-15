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

#include <cmath>

#include <boost/bind.hpp>
#include <boost/core/ref.hpp>

#include <misfits/fits.hpp>
#include <misfits/table.hpp>
#include <misfits/row.hpp>

#include "fits_p.hpp"

namespace misFITS {

    namespace Entry {

	/////////////////////////////
        // Specialized Columns	   //
        /////////////////////////////

	//-----------------------------------------

	// FITS bits are left justified in the bytes, with
	// unused bits set to zero at the end.  e.g., if the
	// format is 5X, it is stored in the 8 bit byte as
	// XXXXX000.

	Column<BitSet>::Column( const ColumnInfo& info, BitSet* base ) :
	    base_( base ),
	    colnum_( info.colnum ),
	    nelem_( info.nelem() ) {

	    if ( ColumnType::Bit != info.column_type )
		throw( Exception::Assert( "can't use an misFITS::BitSet object with a non bit FITS column" ) );

	    base_->resize( nelem_ );
	    nbytes_ = base_->num_blocks();
	    buffer.resize( nbytes_ );
	}

	void
	Column<BitSet>::read( const File& file, LONGLONG firstrow ) {

	    file.read_col( colnum_, firstrow, 1, buffer.size(), reinterpret_cast<NativeType<SC_BYTE>::storage_type*>(&buffer[0]) );

	    boost::from_block_range(bitset_input_iterator( buffer.begin() ),
				    bitset_input_iterator( buffer.end() ),
				    *base_ );
	}

	void
	Column<BitSet>::write( const File& file, LONGLONG firstrow ) {

	    boost::to_block_range(*base_, bitset_output_iterator( buffer.begin()) );

	    file.write_col( colnum_, firstrow, 1, nbytes_, reinterpret_cast<NativeType<SC_BYTE>::storage_type*>(&buffer[0]) );
	}



	//-----------------------------------------


	template<>
	Column<byte_t>::Column( const ColumnInfo& info, byte_t* base ) :
	    base_( base ),
	    colnum_( info.colnum ),
	    nelem_( info.nelem() ) {

	    // if reading bits, nelem_ is the number of 8 bit bytes
	    if ( ColumnType::Bit == info.column_type ) {
		nelem_ /= 8;
		if ( nelem_ * 8 < info.nelem() ) ++nelem_;
	    }
	}

	template<>
	Column< std::vector<byte_t> >::Column( const ColumnInfo& info, std::vector<byte_t>* base ) :
	    base_( base ),
	    colnum_( info.colnum ),
	    nelem_( info.nelem() ) {

	    // if reading bits, nelem_ is the number of 8 bit bytes
	    if ( ColumnType::Bit == info.column_type ) {
		nelem_ /= 8;
		if ( nelem_ * 8 < info.nelem() ) ++nelem_;
	    }

	    base_->resize( nelem_ );
	}


	//-----------------------------------------


	Column<std::string>::Column( const ColumnInfo& info, std::string* base ) :
	    base_( base ),
	    colnum_( info.colnum ),
	    nelem_( info.extent.nelem() ),
	    offset( info.offset ),
	    nbytes( info.nbytes )
	{

	    if ( ColumnType::String != info.column_type )
		throw Exception::Assert( "a std::string destination can only be used with a FITS 'A' column type" );

	    buffer.resize( nbytes );
	    base_->resize( nbytes );
	}


	void
	Column<std::string>::read( const File& file, LONGLONG firstrow ) {

	    // CFITSIO's TSTRING implementation won't allow reading in a multidim set of characters
	    // in one go into a buffer, and attempts to read TBYTE fail because of the "magic"
	    // associated with strings in CFITSIO, so, use a very low level routine
	    misFITS_CHECK_CFITSIO_EXPR
		(
		 fits_read_tblbytes( file.fptr(), firstrow, offset, nbytes,
				     reinterpret_cast<unsigned char*>(&buffer[0]),
				     &status )
		 );
	    base_->assign(&buffer[0], nbytes );
	    // FITS standard allows null terminated strings
	    std::string::size_type nchar = base_->find_first_of( '\0' );
	    if ( nchar != std::string::npos )
		base_->resize( nchar );
	}

	void
	Column<std::string>::write( const File& file, LONGLONG firstrow ) {

	    buffer.assign( nbytes, ' ' );
	    base_->copy( &buffer[0], nbytes );

	    misFITS_CHECK_CFITSIO_EXPR
		(
		 fits_write_tblbytes( file.fptr(), firstrow, offset, nbytes,
				      reinterpret_cast<unsigned char*>(&buffer[0]), &status )
		 );
	}


	//-----------------------------------------

	Column< std::vector<std::string> >::Column( const ColumnInfo& info, std::vector<std::string>* base ) :
	    base_( base ),
	    colnum_( info.colnum ),
	    nelem_( info.extent.nelem() ),
	    offset( info.offset ),
	    nbytes( info.nbytes ),
	    width( info.extent[0] ) {

	    if ( ColumnType::String != info.column_type )
		throw Exception::Assert( "a vector<std::string> destination can only be used with a FITS 'A' column type" );

	    buffer.resize( nbytes );
	    base_->resize( nelem_ / width );
	    for_each( base_->begin(), base_->end(), bind2nd(mem_fun_ref( &std::string::reserve ), width ) );
	}


	void
	Column< std::vector<std::string> >::read( const File& file, LONGLONG firstrow ) {

	    misFITS_CHECK_CFITSIO_EXPR
		(
		 fits_read_tblbytes( file.fptr(), firstrow, offset, nbytes,
				     reinterpret_cast<unsigned char*>(&buffer[0]),
				     &status )
		 );

	    char* start = &buffer[0];

	    std::vector<std::string>::iterator str = base_->begin();
	    std::vector<std::string>::iterator end = base_->end();
	    for ( ; str < end ; ++str ) {
		str->assign( start, width );
		start += width;

		// FITS standard allows null terminated strings See 7.3.3.1 of the FITS
		// paper at
		// http://fits.gsfc.nasa.gov/standard30/fits_standard30aa.pdf
		// however, what happens when the cell has a multi-dimensional
		// character array?  May each array be null terminated, or does
		// the data get truncated after the first NULL prior to dividing it into
		// the separate arrays?

		// CFITSIO has the second behavior.  This code follows the first behavior.

		std::string::size_type nchar = str->find_first_of( '\0' );
		if ( nchar != std::string::npos )
		    str->resize( nchar );
	    }

	}

	void
	Column< std::vector<std::string> >::write( const File& file, LONGLONG firstrow ) {

	    buffer.assign( nbytes, ' ' );

	    char* start = &buffer[0];

	    std::vector<std::string>::iterator str = base_->begin();
	    std::vector<std::string>::iterator end = base_->end();
	    for ( ; str < end ; ++str, start += width ) {
		std::string::size_type maxc = str->length();
		if ( width > maxc ) {
		    str->copy( start, maxc );
		    start[maxc] = '\0';
		}
		else {
		    str->copy( start, width );
		}
	    }

	    misFITS_CHECK_CFITSIO_EXPR
		(
		 fits_write_tblbytes( file.fptr(), firstrow, offset, nbytes,
				      reinterpret_cast<unsigned char*>(&buffer[0]), &status )
		 );

	}

    }

    //////////////////////
    // Row Constructors //
    //////////////////////

    void
    Row::init( ) {
	idx(1);
	auto_advance( true );
    }

    Row::Row( own_or_observe::rptr<Table>* table )  {
	init();
	table_.set( table );
    }

    Row::Row( TablePtr& table )  {
	init();
	table_.set<own_or_observe::observed>( table );
    }

    Row::Row( Table& table ) {
	init();
	table_.set<own_or_observe::observed>( &table );
    }

    Row::Row( FilePtr& file, int hdu_num ) {
	init();
	TablePtr fp = TablePtr( new Table( file, hdu_num ) );
	table_.set<own_or_observe::owned>( fp );
    }

    Row::Row( FilePtr& file, const std::string& extname, int extver ) {
	init();
	TablePtr fp = TablePtr( new Table( file, extname, extver ) );
	table_.set<own_or_observe::owned>( fp );
    }

    bool
    Row::read(){

	resetHDU chdu( table_->file, table_->hdu_num );

	if ( ! entries.size() )
	    throw Exception::Assert( "row object was not assigned any columns to read" );

	if ( idx() > table_->num_rows() )
	    return false;

	for_each( entries.begin(), entries.end(),
		  boost::bind( &Entry::ColumnBase::read, _1, boost::ref(*table_->file()),
			       idx() )
		  );

	if ( auto_advance() )
	    advance();

	return true;
    }

    void
    Row::write() {

	resetHDU chdu( table_->file, table_->hdu_num );

	if ( ! entries.size() )
	    throw Exception::Assert( "row object was not assigned any columns to write" );

	for_each( entries.begin(), entries.end(),
		  boost::bind( &Entry::ColumnBase::write, _1,
			       boost::ref(*table_->file()),
			       idx() )
		  );

	if ( auto_advance() )
	    advance();
    }



}
