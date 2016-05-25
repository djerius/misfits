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
	    base_( base ),
	    colnum_( info.colnum ),
	    nbits_( static_cast<BitSet::size_type>( info.nelem() ) )
	{

	    if ( ColumnType::Bit != info.column_type )
		throw( Exception::Assert( "can't use an misFITS::BitSet object with a non bit FITS column" ) );

	    base_->resize( nbits_ );


	    nbytes_ = base_->num_blocks();
	    max_bits_ = nbytes_ * BitSet::bits_per_block;

	    buffer.resize( nbytes_ );
	}

	void
	Column<BitSet>::read( const Table& table, LONGLONG firstrow ) {

	    table.read_col( colnum_, firstrow, 1, static_cast<LONGLONG>( buffer.size() ),
			    reinterpret_cast<NativeType<SC_BYTE>::storage_type*>(&buffer[0]) );

	    base_->resize( max_bits_ );
	    boost::from_block_range(bitset_input_iterator( buffer.begin() ),
	    			    bitset_input_iterator( buffer.end() ),
	    			    *base_ );
	    base_->resize( nbits_ );

	}

	void
	Column<BitSet>::write( const Table& table, LONGLONG firstrow ) {

	    boost::to_block_range(*base_, bitset_output_iterator( buffer.begin()) );

	    table.write_col( colnum_, firstrow, 1, static_cast<LONGLONG>( nbytes_ ),
			     reinterpret_cast<NativeType<SC_BYTE>::storage_type*>(&buffer[0]) );
	}



	//-----------------------------------------

	Column<bool>::Column( const ColumnInfo& info, bool* base ) :
		base_( base ),
		colnum_( info.colnum ),
		nelem_( static_cast<Buffer::size_type>( info.nelem() ) ) {

		if ( sizeof(bool) != sizeof( NativeType<SC_BYTE>::storage_type ) )
		    buffer.resize( nelem_ );
	    }

	void
	Column<bool>::read( const Table& table, LONGLONG firstrow ) {

	    if ( sizeof(bool) != sizeof( NativeType<SC_BYTE>::storage_type ) ) {

		table.read_col<ColumnType::Logical>( colnum_, firstrow, 1, static_cast<LONGLONG>( nelem_ ), &buffer[0] );

		for ( Buffer::size_type idx = 0 ; idx < nelem_ ; idx++ )
		    base_[idx] = buffer[idx];
	    }

	    else  {

		table.read_col<ColumnType::Logical>( colnum_, firstrow, 1, static_cast<LONGLONG>( nelem_ ),
						     reinterpret_cast<NativeType<SC_BYTE>::storage_type*>(base_) );

	    }

	}

	void
	Column<bool>::write( const Table& table, LONGLONG firstrow ) {

	    if ( sizeof(bool) != sizeof( NativeType<SC_BYTE>::storage_type ) ) {

		for ( Buffer::size_type idx = 0 ; idx < nelem_ ; idx++ )
		    buffer[idx] = base_[idx];

		table.write_col<ColumnType::Logical>( colnum_, firstrow, 1, static_cast<LONGLONG>( nelem_ ), &buffer[0] );
	    }

	    else {

		table.write_col<ColumnType::Logical>( colnum_, firstrow, 1, static_cast<LONGLONG>( nelem_ ),
						      reinterpret_cast<NativeType<SC_BYTE>::storage_type*>(base_) );

	    }


	}

	//-----------------------------------------

	// std::vector<bool> is a specialized monster that's not a vector of bools.

	Column< std::vector<bool> >::Column( const ColumnInfo& info, std::vector<bool>* base )
		: base_( base ),
		  colnum_( info.colnum ),
		  nelem_( static_cast<Buffer::size_type>( info.nelem() ) ) {
		base->resize( nelem_ );
		buffer.resize( nelem_ );
	    }

	void
	Column< std::vector<bool> >::read( const Table& table, LONGLONG firstrow ) {

	    table.read_col<ColumnType::Logical>( colnum_, firstrow, 1, static_cast<LONGLONG>( nelem_ ), &buffer[0] );

	    for ( Buffer::size_type idx = 0 ; idx < nelem_ ; idx++ )
		(*base_)[idx] = buffer[idx];

	}

	void
	Column< std::vector<bool> >::write( const Table& table, LONGLONG firstrow ) {

	    for ( Buffer::size_type idx = 0 ; idx < nelem_ ; idx++ )
		buffer[idx] = (*base_)[idx];

	    table.write_col<ColumnType::Logical>( colnum_, firstrow, 1, static_cast<LONGLONG>( nelem_ ), &buffer[0] );

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
	    nelem_( static_cast<Base::size_type>( info.nelem() ) ) {

	    // if reading bits, nelem_ is the number of 8 bit bytes
	    if ( ColumnType::Bit == info.column_type ) {
		Base::size_type nbits = nelem_;
		nelem_ /= 8;
		if ( nelem_ * 8 < nbits ) ++nelem_;
	    }

	    base_->resize( nelem_ );
	}


	//-----------------------------------------


	Column<std::string>::Column( const ColumnInfo& info, std::string* base ) :
	    base_( base ),
	    colnum_( info.colnum ),
	    nelem_( static_cast<Base::size_type>( info.extent.nelem() ) ),
	    offset( info.offset ),
	    nbytes( static_cast<Base::size_type>( info.nbytes ) )
	{

	    if ( ColumnType::String != info.column_type )
		throw Exception::Assert( "a std::string destination can only be used with a FITS 'A' column type" );

	    buffer.resize( nbytes );
	    base_->resize( nbytes );
	}


	void
	Column<std::string>::read( const Table& table, LONGLONG firstrow ) {

	    // CFITSIO's TSTRING implementation won't allow reading in
	    // a multidim set of characters in one go into a buffer,
	    // and attempts to read TBYTE fail because of the "magic"
	    // associated with strings in CFITSIO, so, use a very low
	    // level routine
	    table.read_bytes( firstrow, offset,
			      static_cast<LONGLONG>( nbytes ),
			      reinterpret_cast<unsigned char*>(&buffer[0])
			      );

	    base_->assign(&buffer[0], nbytes );
	    // FITS standard allows null terminated strings
	    std::string::size_type nchar = base_->find_first_of( '\0' );
	    if ( nchar != std::string::npos )
		base_->resize( nchar );
	}

	void
	Column<std::string>::write( const Table& table, LONGLONG firstrow ) {

	    buffer.assign( nbytes, ' ' );
	    base_->copy( &buffer[0], nbytes );

	    table.write_bytes( firstrow, offset,
			       static_cast<LONGLONG>(nbytes),
			       reinterpret_cast<unsigned char*>(&buffer[0])
			       );
	}


	//-----------------------------------------

	Column< std::vector<std::string> >::Column( const ColumnInfo& info, std::vector<std::string>* base ) :
	    base_( base ),
	    nelem_( static_cast<Base::size_type>( info.extent.nelem() ) ),
	    colnum_( info.colnum ),
	    offset( info.offset ),
	    nbytes( static_cast<Buffer::size_type>( info.nbytes ) ),
	    width( static_cast<Buffer::size_type>(info.extent[0] ) ) {

	    if ( ColumnType::String != info.column_type )
		throw Exception::Assert( "a vector<std::string> destination can only be used with a FITS 'A' column type" );

	    buffer.resize( nbytes );
	    base_->resize( nelem_ / width );
	    for_each( base_->begin(), base_->end(), bind2nd(mem_fun_ref( &std::string::reserve ), width ) );
	}


	void
	Column< std::vector<std::string> >::read( const Table& table, LONGLONG firstrow ) {

	    table.read_bytes( firstrow, offset,
			      static_cast<LONGLONG>( nbytes ),
			      reinterpret_cast<unsigned char*>(&buffer[0])
			      );

	    char* start = &buffer[0];

	    Base::iterator str = base_->begin();
	    Base::iterator end = base_->end();
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

		Buffer::size_type nchar = str->find_first_of( '\0' );
		if ( nchar != std::string::npos )
		    str->resize( nchar );
	    }

	}

	void
	Column< std::vector<std::string> >::write( const Table& table, LONGLONG firstrow ) {

	    buffer.assign( nbytes, ' ' );

	    char* start = &buffer[0];

	    Base::iterator str = base_->begin();
	    Base::iterator end = base_->end();
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

	    table.write_bytes( firstrow, offset,
			       static_cast<LONGLONG>(nbytes),
			       reinterpret_cast<unsigned char*>(&buffer[0])
			       );
	}

    }

}
