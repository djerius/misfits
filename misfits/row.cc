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

namespace misFITS {

    namespace Entry {

	/////////////////////////////
        // Specialized Columns	   //
        /////////////////////////////

	Column<BitStore>::Column( const ColumnInfo& info, BitStore* base ) :
	    base_( base ),
	    colnum_( info.colnum ),
	    nelem_( info.nelem() ) {

	    if ( CT_BIT != info.column_type )
		throw( Exception::Assert( "can't use an misFITS::BitStore object with a non bit FITS column" ) );

	    // FITS bits are left justified in the bytes, with
	    // unused bits set to zero at the end.  e.g., if the
	    // format is 5X, it is stored in the 8 bit byte as
	    // XXXXX000. Preallocate enough bits for the total number of bytes
	    // so that the value can be left shifted before being output
	    base_->resize( nelem_ );
	    nbits_ = base_->num_blocks() * BitStore::bits_per_block;
	    base_->resize( nbits_ );
	    nbytes_ = base_->num_blocks();
	    buffer.resize( nbytes_ );
	}

	void
	Column<BitStore>::read( const File& file, LONGLONG firstrow ) {

	    file.read_col( colnum_, firstrow, 1, buffer.size(), reinterpret_cast<NativeType<SC_BYTE>::storage_type*>(&buffer[0]) );

	    boost::from_block_range(buffer.rbegin(), buffer.rend(), *base_ );
	    (*base_) >>= nbits_ - nelem_;
	}

	void
	Column<BitStore>::write( const File& file, LONGLONG firstrow ) {

	    (*base_) <<= nbits_ - nelem_;
	    boost::to_block_range(*base_, buffer.rbegin());

	    file.write_col( colnum_, firstrow, 1, nbytes_, reinterpret_cast<NativeType<SC_BYTE>::storage_type*>(&buffer[0]) );
	}



	template<>
	Column<byte_t>::Column( const ColumnInfo& info, byte_t* base ) :
	    base_( base ),
	    colnum_( info.colnum ),
	    nelem_( info.nelem() ) {

	    // if reading bits, nelem_ is the number of 8 bit bytes
	    if ( CT_BIT == info.column_type ) {
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
	    if ( CT_BIT == info.column_type ) {
		nelem_ /= 8;
		if ( nelem_ * 8 < info.nelem() ) ++nelem_;
	    }

	    base_->resize( nelem_ );
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

    Row::Row( own_or_observe::rptr<Table>* table_ )  {
	init();
	table.set( table_ );
    }

    Row::Row( TablePtr& table_ )  {
	init();
	table.set<own_or_observe::observed>( table_ );
    }

    Row::Row( Table& table_ ) {
	init();
	table.set<own_or_observe::observed>( &table_ );
    }

    Row::Row( FilePtr& file ) {
	init();
	TablePtr fp( file->table() );
	table.set<own_or_observe::owned>( fp );
    }

    Row::Row( File& file ) {
	init();
	TablePtr tp( file.table() );
	table.set<own_or_observe::owned>( tp );
    }



    bool
    Row::read(){

	if ( ! entries.size() )
	    throw Exception::Assert( "row object was not assigned any columns to read" );

	if ( idx() > table->num_rows() )
	    return false;

	for_each( entries.begin(), entries.end(),
		  boost::bind( &Entry::ColumnBase::read, _1, boost::ref(*table->file()),
			       idx() )
		  );

	if ( auto_advance() )
	    advance();

	return true;
    }

    void
    Row::write() {

	if ( ! entries.size() )
	    throw Exception::Assert( "row object was not assigned any columns to write" );

	for_each( entries.begin(), entries.end(),
		  boost::bind( &Entry::ColumnBase::write, _1,
			       boost::ref(*table->file()),
			       idx() )
		  );

	if ( auto_advance() )
	    advance();
    }



}
