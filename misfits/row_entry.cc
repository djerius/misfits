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

#include <misfits/row_entry.hpp>

#include "fits_p.hpp"

namespace misFITS {

    namespace RowEntry {

	/////////////////////////////
        // Specialized Columns	   //
        /////////////////////////////

	//-----------------------------------------

	// FITS bits are left justified in the bytes, with
	// unused bits set to zero at the end.  e.g., if the
	// format is 5X, it is stored in the 8 bit byte as
	// XXXXX000.

	Column<BitSet>::Column( const ColumnInfo& info, BitSet* base ) :
	    ColumnBase( info), base_( base )
	{

	    base_->resize( nelem_ );

	    switch( id_ ) {

	    case ColumnType::ID::Bit:

		natomic_ = base_->num_blocks();
		break;

	    case ColumnType::ID::Logical:

		natomic_ = nelem_;
		break;

	    default:
		throw( Exception::Assert( "can't only use a misFITS::BitSet object with BIT and LOGICAL FITS columns" ) );

	    }

	    max_bits_ = natomic_ * BitSet::bits_per_block;
	    buffer.resize( natomic_ );
	}

	void
	Column<BitSet>::read( const Table& table, LONGLONG firstrow ) {

	    table.read_col( colnum_, firstrow, 1, static_cast<LONGLONG>( buffer.size() ),
			    reinterpret_cast<NativeType<SC_BYTE>::storage_type*>(&buffer[0]) );


	    switch ( id_ ) {

	    case ColumnType::ID::Bit:

		base_->resize( max_bits_ );
		boost::from_block_range(bitset_input_iterator( buffer.begin() ),
					bitset_input_iterator( buffer.end() ),
					*base_ );
		base_->resize( nelem_ );
		break;

	    case ColumnType::ID::Logical:

		for ( Buffer::size_type idx = 0 ; idx < natomic_ ; idx++ )
		    (*base_)[idx] = buffer[idx];
		break;


	    default:
		throw( Exception::Assert( "internal error" ) );

	    }


	}

	void
	Column<BitSet>::write( const Table& table, LONGLONG firstrow ) {


	    switch ( id_ ) {

	    case ColumnType::ID::Bit:

		boost::to_block_range(*base_, bitset_output_iterator( buffer.begin()) );
		break;

	    case ColumnType::ID::Logical:

		for ( Buffer::size_type idx = 0 ; idx < natomic_ ; idx++ )
		    buffer[idx] = (*base_)[idx] ;
		break;


	    default:
		throw( Exception::Assert( "internal error" ) );

	    }

	    table.write_col( colnum_, firstrow, 1, static_cast<LONGLONG>( natomic_ ),
			     reinterpret_cast<NativeType<SC_BYTE>::storage_type*>(&buffer[0]) );
	}



	//-----------------------------------------

	Column<bool>::Column( const ColumnInfo& info, bool* base )
	    : ColumnBase( info ), base_( base ) {

		buffer.resize( natomic_ );
	}

	void
	Column<bool>::read( const Table& table, LONGLONG firstrow ) {

	    table.read_col<ColumnType::ID::Logical>( colnum_, firstrow, 1, static_cast<LONGLONG>( natomic_ ), &buffer[0] );

	    for ( Buffer::size_type idx = 0 ; idx < natomic_ ; idx++ )
		base_[idx] = buffer[idx];
	}

	void
	Column<bool>::write( const Table& table, LONGLONG firstrow ) {

	    for ( Buffer::size_type idx = 0 ; idx < natomic_ ; idx++ )
		buffer[idx] = base_[idx];

	    table.write_col<ColumnType::ID::Logical>( colnum_, firstrow, 1, static_cast<LONGLONG>( natomic_ ), &buffer[0] );
	}

	//-----------------------------------------


	template<>
	void ColumnInit<byte_t>::init() {

	    // if reading bits, nelem_ is the number of bits, must convert to bytes
	    if ( ColumnType::ID::Bit == id_ ) {
		natomic_ /= 8;
		if ( natomic_ * 8 < nelem_ ) ++natomic_;
	    }
	}


	//-----------------------------------------


	Column<std::string>::Column( const ColumnInfo& info, std::string* base ) :
	    ColumnBase( info ),
	    base_( base ),
	    offset( info.offset )
	{

	    if ( ColumnType::ID::String != info.column_type->id() )
		throw Exception::Assert( "a std::string destination can only be used with a FITS 'A' column type" );

	    natomic_ = info.nbytes;

	    buffer.resize( natomic_ );
	    base_->resize( natomic_ );
	}


	void
	Column<std::string>::read( const Table& table, LONGLONG firstrow ) {

	    // CFITSIO's TSTRING implementation won't allow reading in
	    // a multidim set of characters in one go into a buffer,
	    // and attempts to read TBYTE fail because of the "magic"
	    // associated with strings in CFITSIO, so, use a very low
	    // level routine
	    table.read_bytes( firstrow, offset,
			      static_cast<LONGLONG>( natomic_ ),
			      reinterpret_cast<unsigned char*>(&buffer[0])
			      );

	    base_->assign(&buffer[0], natomic_ );
	    // FITS standard allows null terminated strings
	    std::string::size_type nchar = base_->find_first_of( '\0' );
	    if ( nchar != std::string::npos )
		base_->resize( nchar );
	}

	void
	Column<std::string>::write( const Table& table, LONGLONG firstrow ) {

	    buffer.assign( natomic_, ' ' );
	    base_->copy( &buffer[0], natomic_ );

	    table.write_bytes( firstrow, offset,
			       static_cast<LONGLONG>(natomic_),
			       reinterpret_cast<unsigned char*>(&buffer[0])
			       );
	}


	//-----------------------------------------


    }

}
